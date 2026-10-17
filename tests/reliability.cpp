#include <algorithm>
#include <cerrno>
#include <climits>
#include <cmath>
#include <csignal>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "Circuit.hpp"
#include "Clock.hpp"
#include "Terminal.hpp"
#include "Link.hpp"
#include "Memory8.hpp"
#include "SafeFile.hpp"
#include "Shell.hpp"
#include "4522.hpp"
#include "4089.hpp"
#include "74190.hpp"

extern "C" { bunny_declare_context(screen); }
namespace fs = std::filesystem;
#define CHECK(condition) do { if (!(condition)) throw std::runtime_error(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " #condition); } while (0)

static int fail_write, fail_sync, fail_close, fail_rename, fail_link;
static bool partial_write, interrupt_write, stop_during_write, stop_after_rename;
extern "C" ssize_t __real_write(int, const void *, size_t);
extern "C" int __real_fsync(int);
extern "C" int __real_close(int);
extern "C" int __real_rename(const char *, const char *);
extern "C" int __real_link(const char *, const char *);
extern "C" ssize_t __wrap_write(int fd, const void *buffer, size_t count)
{
  if (interrupt_write) { interrupt_write = false; errno = EINTR; return (-1); }
  if (fail_write && --fail_write == 0) { errno = ENOSPC; return (-1); }
  if (partial_write) count = std::min<size_t>(count, 7);
  ssize_t result = __real_write(fd, buffer, count);
  if (stop_during_write) { stop_during_write = false; raise(SIGSTOP); }
  return (result);
}
extern "C" int __wrap_fsync(int fd)
{
  if (fail_sync && --fail_sync == 0) { errno = EIO; return (-1); }
  return (__real_fsync(fd));
}
extern "C" int __wrap_close(int fd)
{
  int result = __real_close(fd);
  if (fail_close && --fail_close == 0) { errno = EIO; return (-1); }
  return (result);
}
extern "C" int __wrap_rename(const char *from, const char *to)
{
  if (fail_rename && --fail_rename == 0) { errno = EIO; return (-1); }
  int result = __real_rename(from, to);
  if (stop_after_rename) { stop_after_rename = false; raise(SIGSTOP); }
  return (result);
}
extern "C" int __wrap_link(const char *from, const char *to)
{
  if (fail_link && --fail_link == 0) { errno = EIO; return (-1); }
  return (__real_link(from, to));
}

static void put(const std::string &path, const std::string &text)
{
  std::ofstream stream(path, std::ios::binary);
  stream.write(text.data(), text.size());
  stream.close();
  CHECK(stream.good());
}
static std::string get(const std::string &path) { return hbs::ReadFile(path, 64 * 1024 * 1024); }
static void rejects(const std::function<void()> &operation)
{
  bool rejected = false;
  try { operation(); } catch (const std::exception &) { rejected = true; }
  CHECK(rejected);
}
static const std::string direct = ".chipsets:\ninput a(1) [0,0]\noutput z [4,0]\n.links:\na:1 z:1\n";
struct MemoryLoader : hbs::Memory8Utility { using Memory8Utility::LoadFile; };
static void load(hbs::Circuit &circuit, const std::string &text)
{
  put("input.rs", text);
  circuit.Load("input.rs");
}

static void atomic_files()
{
  const std::string old = "previous complete file", next(4096, 'N');
  for (int kind = 0; kind < 4; ++kind)
    {
      put("atomic.rs", old);
      fail_write = kind == 0 ? 2 : 0;
      fail_sync = kind == 1 ? 1 : 0;
      fail_close = kind == 2 ? 1 : 0;
      fail_rename = kind == 3 ? 1 : 0;
      partial_write = true;
      CHECK(!hbs::AtomicWriteFile("atomic.rs", next));
      fail_write = fail_sync = fail_close = fail_rename = 0;
      partial_write = false;
      CHECK(get("atomic.rs") == old);
    }
  CHECK(chmod("atomic.rs", 0640) == 0);
  partial_write = interrupt_write = true;
  CHECK(hbs::AtomicWriteFile("atomic.rs", next));
  partial_write = false;
  CHECK(get("atomic.rs") == next);
  struct stat status;
  CHECK(stat("atomic.rs", &status) == 0 && (status.st_mode & 0777) == 0640);
  CHECK(!hbs::AtomicWriteFile("atomic.rs", old, true));
  CHECK(get("atomic.rs") == next);
  CHECK(symlink("atomic.rs", "alias.rs") == 0);
  CHECK(hbs::AtomicWriteFile("alias.rs", old));
  CHECK(fs::is_symlink("alias.rs") && get("atomic.rs") == old);
  fail_link = 1;
  CHECK(!hbs::AtomicWriteFile("exclusive.rs", next, true));
  CHECK(!fs::exists("exclusive.rs"));
  fail_sync = 2; // Directory sync can fail after publication; contents must stay whole.
  CHECK(!hbs::AtomicWriteFile("atomic.rs", next));
  CHECK(get("atomic.rs") == next);
  for (const auto &entry : fs::directory_iterator("."))
    CHECK(entry.path().filename().string().find(".tmp.") == std::string::npos);

  for (bool committed : {false, true})
    {
      put("atomic.rs", old);
      pid_t child = fork();
      CHECK(child >= 0);
      if (child == 0)
        {
          partial_write = !committed;
          stop_during_write = !committed;
          stop_after_rename = committed;
          hbs::AtomicWriteFile("atomic.rs", next);
          _exit(99);
        }
      int result;
      CHECK(waitpid(child, &result, WUNTRACED) == child && WIFSTOPPED(result));
      CHECK(kill(child, SIGKILL) == 0);
      CHECK(waitpid(child, &result, 0) == child && WIFSIGNALED(result));
      CHECK(get("atomic.rs") == (committed ? next : old));
      for (const auto &entry : fs::directory_iterator("."))
        if (entry.path().filename().string().find(".tmp.") != std::string::npos)
          fs::remove(entry.path());
    }
}

static void loading_and_roundtrips(const std::string &root)
{
  hbs::Timer timer;
  hbs::Circuit circuit(timer);
  rejects([&]{ circuit.Compute(0); });
  rejects([&]{ circuit.Compute(1); });
  load(circuit, direct);
  CHECK(circuit.GetOutputValue("z") == hbs::TRUE);
  rejects([&]{ circuit.Compute(2); });
  rejects([&]{ circuit.GetOutputName(0); });
  CHECK(!circuit.RenameComponent(circuit.CreateUserComponent("input", {0,0}), "bad:name"));
  rejects([&]{ circuit.CreateUserComponent("missing-type", {0,0}); });
  CHECK(circuit.Save("good.rs"));
  std::string saved = get("good.rs");
  for (const std::string &bad : {
      ".chipsets:\ninput a [0,0]\n.links:\na:1 missing:1\n",
      ".chipsets:\ninput a [0,0]\noutput z [1,0]\n.links:\na:1 z:2\n",
      ".chipsets:\ninput a [nan,0]\n.links:\n",
      ".chipsets:\ninput a [1e99,0]\n.links:\n",
      ".chipsets:\n.tracks:\na []\nb []\n.links:\na:1 b:1\n",
      ".chipsets:\ninput a [0,0]\n.tracks:\nx []\n.links:\na:1 x:999999999999999999999999\n"})
    {
      rejects([&]{ load(circuit, bad); });
      CHECK(circuit.GetOutputValue("z") == hbs::TRUE);
      CHECK(circuit.Save("after-error.rs"));
      CHECK(get("after-error.rs") == saved);
    }
  rejects([&]{ circuit.Load("."); });
  CHECK(mkfifo("pipe.rs", 0600) == 0);
  rejects([&]{ circuit.Load("pipe.rs"); });
  CHECK(circuit.Load("new.rs", true));
  CHECK(get("new.rs").find(".links:") != std::string::npos);
  CHECK(!circuit.HasComponent("a"));

  std::string rom = (fs::path(root)/"res/03_advanced/misc/rom.bin").string();
  load(circuit, ".chipsets:\n2716 rom("+rom+") [0.12345678901234566,-2]\ninput a(Z) [0,0]\noutput z [4,0]\n.links:\na:1 z:1\n.package:\nline [0,0] [3,4]\npin a [0,0]\npin z [4,0]\n");
  CHECK(circuit.Save("roundtrip.rs", {1.1234567890123457, 2}, 30));
  CHECK(get("roundtrip.rs").find(rom) != std::string::npos);
  CHECK(get("roundtrip.rs").find(".package:") != std::string::npos);
  CHECK(get("roundtrip.rs").find("input a(Z)") != std::string::npos);
  hbs::Circuit other(timer);
  CHECK(other.Load("roundtrip.rs"));
  CHECK(other.GetPinCount() == 2);
  CHECK(other.Save("roundtrip2.rs", {1.1234567890123457, 2}, 30));
  CHECK(get("roundtrip.rs") == get("roundtrip2.rs"));

  hbs::Track *track = other.CreateUserTrackAt({0,0});
  track->AddFreeNode({1,0}, hbs::ILink::BOTTOM);
  CHECK(other.Save("disconnected.rs"));
  CHECK(get("disconnected.rs").find("b 0,0 b 1,0] {}") != std::string::npos);
  CHECK(other.Load("disconnected.rs"));
  CHECK(other.GetLinkCount() == 2);
  CHECK(other.GetComponentCount(true)-other.GetComponentCount(false)==3);
  std::vector<uint8_t> memory(16, 99);
  put("empty.bin", "");
  CHECK(MemoryLoader::LoadFile("empty.bin", memory, 0xff));
  CHECK(std::all_of(memory.begin(), memory.end(), [](auto n){return n == 0xff;}));
  rejects([&]{ MemoryLoader::LoadFile(".", memory, 0xff); });
  auto packaged=other.CreateUserComponent("packaged",{3,4});
  CHECK(packaged->GetPinCount()==2);
  hbs::Input signal(timer,"signal","1","0,0");
  packaged->SetLink(1,signal,1,"");
  CHECK(packaged->Compute(2)==hbs::TRUE);
  packaged->DisconnectFrom(&signal);
}

class CacheProbe : public hbs::AComponent<1>
{
  const std::string &GetType() const override { return (type = "probe"); }
public:
  explicit CacheProbe(hbs::Timer &timer) : AComponent(timer,"probe","0,0") {}
  hbs::Tristate Compute(size_t n) override
  {
    hbs::Tristate result;
    if (AlreadyComputed(n,result)) return result;
    auto pin=PresetOutput(n);
    hbs::Tristate again;
    CHECK(AlreadyComputed(n,again));
    return (pin->second=hbs::TRUE); // Catches erasure of a live cache iterator.
  }
};
class ClockProbe : public hbs::Clock
{
public:
  explicit ClockProbe(hbs::Timer &timer) : Clock(timer,"clock","0","0,0") {}
  size_t CachedTicks() const { return timeline.size(); }
};
static void simulation()
{
  hbs::Timer timer;
  CacheProbe probe(timer);
  CHECK(probe.Compute(1) == hbs::TRUE);
  for (int i=0;i<12;++i) timer.Tick();
  CHECK(probe.Compute(1) == hbs::TRUE);
  ClockProbe clock(timer);
  for (int i=0;i<10000;++i) { timer.Tick(); clock.Compute(1); }
  CHECK(clock.CachedTicks() <= 4);
  rejects([&]{ hbs::CMP_4522 invalid(timer,"bad","0,0","4522","test",0); });
  rejects([&]{ hbs::CMP_4089 invalid(timer,"bad","0,0","4089","test",0); });
  rejects([&]{ hbs::CMP_74190 invalid(timer,"bad","0,0","74190","test",0); });
  hbs::Circuit c(timer);
  load(c,direct);
  auto counter=c.CreateUserComponent("7493",{0,0});
  hbs::Input cp(timer,"cp","1","0,0"), low(timer,"low","0","0,0");
  counter->SetLink(14,cp,1,"");
  for(size_t pin:{1,2,3}) counter->SetLink(pin,low,1,"");
  counter->Compute(12);
  timer.Tick(); cp.SetValue(hbs::FALSE);
  CHECK(counter->Compute(12)==hbs::TRUE);
  counter->DisconnectFrom(&cp); counter->DisconnectFrom(&low);
  hbs::Track track(timer,"wire","");
  hbs::Input input(timer,"i","1","0,0");
  rejects([&]{ track.SetLink(SIZE_MAX,input,1,""); });
  rejects([&]{ input.SetLink(1,input,2,""); });
  CHECK(input.Compute(1) == hbs::TRUE);
  hbs::Link legacy;
  legacy.third.push_back({{3,4},hbs::ILink::BOTTOM});
  CHECK(legacy.GetStep(0) == std::make_pair(3,4));
  rejects([&]{legacy.GetStep(1);});
  std::ostringstream out,err;
  for(const char *bad : {"tick nope","simulate 999999999999999999999999","tick -1"})
    CHECK(hbs::ExecuteCommand(bad,c,timer,out,err,true,0).status == hbs::COMMAND_ERROR);
}

static void parser_corpus()
{
  std::vector<std::string> fixtures={direct,
    ".chipsets:\ninput a [1e2,-.5]\noutput z\n.tracks:\nt [t 0,0 b 1,0] {1-2}\n.links:\na:1 t:1@1\nz:1 t:2@2\n.package:\npin a [0,0]\n",
    ".chipsets:\ninput a [0,0]\n.links:\na:99999999999999999999999999 z:1\n"};
  size_t count=0;
  hbs::Timer timer;
  hbs::Circuit circuit(timer);
  for(const auto &text:fixtures)
    for(size_t length=0;length<=text.size();++length)
      {
        put("fuzz.rs",text.substr(0,length));
        try {circuit.Load("fuzz.rs"); circuit.Compute();} catch(const std::exception &) {}
        ++count;
      }
  uint32_t random=42;
  for(int trial=0;trial<1500;++trial)
    {
      std::string text=fixtures[trial%fixtures.size()];
      for(int mutation=0;mutation<3;++mutation)
        {
          random=random*1664525u+1013904223u;
          size_t at=random%text.size();
          random=random*1664525u+1013904223u;
          text[at]=(char)(random>>24);
        }
      put("fuzz.rs",text);
      try {circuit.Load("fuzz.rs"); circuit.Compute();} catch(const std::exception &) {}
      ++count;
    }
  rejects([&]{load(circuit,".chipsets:\nself x [0,0]\n.links:\n");});
  std::cout<<"PASS "<<count<<" truncated/mutated parser inputs and recursive circuit rejection\n";
}

static void screen_events()
{
  for (int i=0;i<4;++i)
    {
      bunny_test_fail_after=i;
      rejects([]{hbs::Screen screen("");});
      CHECK(bunny_test_allocations==0);
    }
  bunny_test_fail_after=-1;
  hbs::Timer timer;
  hbs::Circuit circuit(timer);
  load(circuit,direct);
  hbs::Screen screen("input.rs");
  LoopData data(circuit,timer,screen);
  screen.pin_size=0;
  CHECK(std::isfinite(screen.ScreenToWorld({1,1}).x));
  screen.ZoomAt({1,1},INT_MAX);
  CHECK(screen.pin_size==PINSIZE_DEFAULT*10);
  screen.ZoomAt({1,1},INT_MIN);
  CHECK(screen.pin_size==PINSIZE_DEFAULT/4);
  screen.Text({0,0},{8,8},0,"é\xff");
  auto component=circuit.CreateUserComponent("input",{0,0});
  screen.component_to_place=component;
  screen.placing_component=true;
  screen.selected_components.insert(component);
  screen.grabbed=component;
  screen_key(GO_DOWN,BKS_DELETE,data);
  CHECK(!screen.placing_component && screen.selected_components.empty());
  CHECK(screen.grabbed==NULL);
  screen.Draw(circuit);
  screen.search_panel=true;
  screen.search_query="2716";
  screen.search_offset=0;
  rejects([&]{circuit.CreateUserComponent("2716",{0,0});});
  screen_key(GO_DOWN,BKS_RETURN,data); // Missing ROM is an editor error, not an exception across C.
  if (!data.file_browser || data.browser_error.empty())
    std::cerr<<"editor state: browser="<<data.file_browser<<" error="<<data.browser_error
             <<" placing="<<screen.placing_component<<" query="<<screen.search_query<<std::endl;
  CHECK(data.file_browser && !data.browser_error.empty());
  CHECK(circuit.Save("usable.rs"));
  put("second.rs",direct);
  CHECK(data.OpenFile("second.rs"));
  CHECK(!data.SaveCurrentDocumentAs("input.rs"));
  CHECK(data.CloseCurrentDocument(true));
  CHECK(data.CloseCurrentDocument(true));
  rejects([&]{data.CurrentCircuit();});
  rejects([&]{data.CurrentTimer();});
}

static void pipe_and_cli(const std::string &binary)
{
  // Embedded use: default SIGPIPE disposition, no main() to ignore it for us.
  pid_t child=fork();
  CHECK(child>=0);
  if(child==0)
    {
      signal(SIGPIPE,SIG_DFL);
      int p[2];
      if(pipe(p)<0) _exit(90);
      close(p[0]); dup2(p[1],STDOUT_FILENO); close(p[1]);
      hbs::Timer timer;
      hbs::Terminal term(timer,"term","0,0");
      hbs::Input clock(timer,"clock","0","0,0");
      term.SetLink(9,clock,1,"");
      term.Compute(1);
      timer.Tick(); clock.SetValue(hbs::TRUE);
      try {term.Compute(1);} catch(const std::exception &) {_exit(0);}
      _exit(91);
    }
  int status;
  CHECK(waitpid(child,&status,0)==child && WIFEXITED(status) && WEXITSTATUS(status)==0);
  put("direct.rs",direct);
  put("commands.txt","tick 1\nexpect z=1\nexit\n");
  child=fork(); CHECK(child>=0);
  if(child==0)
    {
      execl(binary.c_str(),binary.c_str(),"--test","commands.txt","direct.rs",(char*)NULL);
      _exit(92);
    }
  CHECK(waitpid(child,&status,0)==child && WIFEXITED(status) && WEXITSTATUS(status)==0);
  child=fork();
  CHECK(child>=0);
  if(child==0)
    {
      int p[2]; if(pipe(p)<0) _exit(90);
      close(p[0]); dup2(p[1],STDOUT_FILENO); close(p[1]);
      execl(binary.c_str(),binary.c_str(),"--test","commands.txt","direct.rs",(char*)NULL);
      _exit(92);
    }
  CHECK(waitpid(child,&status,0)==child && WIFEXITED(status) &&
        (WEXITSTATUS(status)==1 || WEXITSTATUS(status)==2));
  child=fork(); CHECK(child>=0);
  if(child==0)
    {
      close(STDIN_FILENO);
      execl(binary.c_str(),binary.c_str(),"direct.rs","--simulate",(char*)NULL);
      _exit(92);
    }
  CHECK(waitpid(child,&status,0)==child && WIFEXITED(status) && WEXITSTATUS(status)==1);
}

static void corpus(const std::string &root)
{
  fs::current_path(root);
  // romdump.rs intentionally asks for an externally supplied rom.bin.
  CHECK(setenv("BCAD_DATADIR",(fs::path(root)/"res/03_advanced/misc").c_str(),1)==0);
  size_t count=0;
  for(const auto &entry:fs::recursive_directory_iterator("res"))
    if(entry.is_regular_file() && (entry.path().extension()==".rs" || entry.path().extension()==".nts"))
      {
        hbs::Timer timer;
        hbs::Circuit c(timer);
        c.Load(entry.path().string());
        for(int i=0;i<8;++i) {timer.Tick(); c.Compute();}
        ++count;
      }
  CHECK(count>0);
  std::cout<<"PASS "<<count<<" bundled circuits loaded and simulated\n";
}

static void catalogue()
{
  hbs::Timer timer;
  hbs::Circuit circuit(timer);
  size_t count=0;
  for(const auto &type:circuit.GetCreatableTypes())
    {
      if(type=="2716" || type=="terminal" || type=="wire" || type=="self") continue;
      auto component=circuit.CreateUserComponent(type,{0,0});
      hbs::Input source(timer,"stimulus","0","0,0");
      for(size_t pin=1;pin<=component->GetPinCount();++pin) component->SetLink(pin,source,1,"");
      for(auto state:{hbs::FALSE,hbs::TRUE,hbs::UNDEFINED,hbs::HIGH_IMPEDANCE,hbs::BROKEN})
        {
          timer.Tick(); source.SetValue(state);
          for(size_t pin=1;pin<=component->GetPinCount();++pin) component->Compute(pin);
        }
      component->DisconnectFrom(&source);
      ++count;
    }
  std::cout<<"PASS "<<count<<" catalogue components, every pin, 0/1/U/Z/B\n";
}

int main(int argc,char **argv)
{
  std::cout.setf(std::ios::unitbuf);
  if(argc!=3) return 2;
  std::string root=argv[1],binary=argv[2];
  char temporary[]="/tmp/bcad-reliability-XXXXXX";
  char *directory=mkdtemp(temporary);
  if(!directory) return 2;
  fs::path original=fs::current_path();
  try
    {
      fs::current_path(directory);
      fs::create_directory("components");
      put("components/self.rs",".chipsets:\nself child [0,0]\n.links:\n");
      put("components/packaged.rs",direct+".package:\npin a [0,0]\npin z [4,0]\n");
      CHECK(setenv("BCAD_PATH",fs::absolute("components").c_str(),1)==0);
      atomic_files(); std::cout<<"PASS atomic save, short writes, EINTR, ENOSPC, fsync/close/rename failures, SIGKILL\n";
      loading_and_roundtrips(root); std::cout<<"PASS transactional loads, bounds, ROM/package/precision round trips\n";
      simulation(); std::cout<<"PASS sparse simulation caches and bounded clock history\n";
      screen_events(); std::cout<<"PASS allocation failure cleanup and editor cancellation\n";
      parser_corpus();
      CHECK(bunny_test_allocations==0);
      pipe_and_cli(binary); std::cout<<"PASS closed pipe and closed stdin, embedded and CLI\n";
      catalogue();
      corpus(root);
      fs::current_path(original); fs::remove_all(directory);
    }
  catch(const std::exception &error)
    {
      std::cerr<<error.what()<<"\nFixture directory: "<<directory<<std::endl;
      return 1;
    }
}
