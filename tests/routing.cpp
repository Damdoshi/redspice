#include "Circuit.hpp"
#include "Shortcuts.hpp"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

extern "C" { bunny_declare_context(screen); }
using namespace hbs;
namespace fs=std::filesystem;
static size_t checks;
#define CHECK(x) do {++checks;if(!(x))throw std::runtime_error(std::string(__func__)+":"+std::to_string(__LINE__)+": " #x);} while(0)
struct Fixture : Circuit
{
  explicit Fixture(Timer &t):Circuit(t){}
  IComponent &Part(const std::string &name){return *circuit.at(name);}
  Track &Wire(const std::string &name="w"){return *tracks.at(name);}
  void Text(const std::string &text)
  {std::ofstream out("fixture.rs");out<<text;out.close();Load("fixture.rs");}
};
static std::string read(const std::string &name)
{std::ifstream in(name);return std::string(std::istreambuf_iterator<char>(in),{});}
static bool same(Position a,Position b){return std::hypot(a.x-b.x,a.y-b.y)<1e-7;}
static RoutingBudget generous(){RoutingBudget b;b.milliseconds=1000;b.max_checks=1000000;b.max_expansions=10000;return b;}
static std::string signature(const Track &track)
{
  auto g=track.GetLayout();std::ostringstream out;out.precision(17);
  for(auto n:g.nodes)out<<n.first.x<<','<<n.first.y<<':'<<n.second<<' ';
  for(auto e:g.segments)out<<e.from<<'-'<<e.to<<' ';
  for(size_t a:g.attachment_nodes)out<<'@'<<a;
  return out.str();
}
static void connected(const Track &track)
{
  auto g=track.GetLayout();std::vector<bool> seen(g.nodes.size(),false);
  CHECK(!g.attachment_nodes.empty() && g.attachment_nodes[0]<g.nodes.size());
  std::vector<size_t> todo{g.attachment_nodes[0]};
  while(!todo.empty())
    {
      size_t n=todo.back();todo.pop_back();if(seen[n])continue;seen[n]=true;
      for(auto e:g.segments)
        {if(e.from==n)todo.push_back(e.to);if(e.to==n)todo.push_back(e.from);}
    }
  const auto &attachments=track.GetAttachments();
  for(size_t i=0;i<g.attachment_nodes.size();++i)
    {
      CHECK(g.attachment_nodes[i]<seen.size() && seen[g.attachment_nodes[i]]);
      CHECK(same(g.nodes[g.attachment_nodes[i]].first,attachments[i].component->GetPinPosition(attachments[i].pin)));
    }
}
static const std::string base=
  ".chipsets:\ninput a(1) [0,0]\nPROBE p [14,0]\n.tracks:\nw [b 0,0 b 6,0 b 12,0] {1-2 2-3}\n.links:\na:1 w:1@1\np:1 w:2@3\n";

static void simple_and_saved()
{
  Timer t;Fixture c(t);c.Text(base);c.Compute();auto time=t.GetTime();
  CHECK(c.GetOutputValue("p")==TRUE);
  auto &a=c.Part("a");auto before=c.CaptureRouting({&a});CHECK(before.size()==1);
  a.Move({0,3});auto result=c.Reroute(before);
  CHECK(result.repaired==1 && result.pending==0 && !result.limited && result.expanded==0);
  connected(c.Wire());CHECK(c.GetOutputValue("p")==TRUE && t.GetTime()==time);
  // The far end of the manually drawn track stays fixed.
  CHECK(same(c.Part("p").GetPinPosition(1),{12,0}));
  c.ValidateRouting();CHECK(!c.Wire().HasRoutingError());
  CHECK(c.Save("routed.rs"));Fixture copy(t);copy.Load("routed.rs");connected(copy.Wire());
  copy.Compute();CHECK(copy.GetOutputValue("p")==TRUE);CHECK(copy.Save("copy.rs"));
  CHECK(read("routed.rs")==read("copy.rs"));
  // A caller's invalid layout cannot partially alter the live document.
  auto g=c.Wire().GetLayout();auto saved=signature(c.Wire());g.segments.push_back({0,999999});
  bool rejected=false;try{c.Wire().SetLayout(g);}catch(const std::exception&){rejected=true;}
  CHECK(rejected && signature(c.Wire())==saved);
}

static void obstacles_and_layers()
{
  for(bool top:{false,true})
    {
      Timer t;Fixture c(t);
      std::string text=base;
      auto at=text.find(".links:");
      text.insert(at,std::string("wall [")+(top?"t ":"b ")+"3,-1 "+(top?"t ":"b ")+"3,4] {1-2}\n");
      c.Text(text);auto wall=signature(c.Wire("wall"));auto &a=c.Part("a");
      auto before=c.CaptureRouting({&a});a.Move({0,3});auto result=c.Reroute(before,generous());
      CHECK(result.repaired==1 && !result.limited);connected(c.Wire());
      CHECK(signature(c.Wire("wall"))==wall);
      c.ValidateRouting();CHECK(!c.Wire().HasRoutingError());
      if(top)CHECK(result.expanded==0);
      else
        {
          CHECK(result.expanded>0);
          auto g=c.Wire().GetLayout();bool passed=false;
          for(auto e:g.segments)
            {
              Position a=g.nodes[e.from].first,b=g.nodes[e.to].first;
              if(std::min(a.x,b.x)<3 && std::max(a.x,b.x)>3)
                {double y=a.y+(b.y-a.y)*(3-a.x)/(b.x-a.x);CHECK(y<-1.24 || y>4.24);passed=true;}
              else if(a.x==3 || b.x==3)
                {Position p=a.x==3?a:b;CHECK(p.y<-1.24 || p.y>4.24);passed=true;}
            }
          CHECK(passed);
        }
    }
  // An unconnected pad is an obstacle on both layers.
  Timer t;Fixture c(t);
  std::string text=base;text.insert(text.find(".tracks:"),"input loose [3,1]\n");c.Text(text);
  auto before=c.CaptureRouting({&c.Part("a")});c.Part("a").Move({0,3});
  CHECK(c.Reroute(before,generous()).repaired==1);connected(c.Wire());
  auto g=c.Wire().GetLayout();
  for(auto e:g.segments)
    for(int i=0;i<=100;++i)
      {
        Position a=g.nodes[e.from].first,b=g.nodes[e.to].first;
        CHECK(std::hypot(a.x+(b.x-a.x)*i/100-3,a.y+(b.y-a.y)*i/100-1)>.59);
      }
}

static void junctions_and_vias()
{
  const std::string common=".chipsets:\ninput a(1) [0,0]\nPROBE p [8,0]\nPROBE q [2,-6]\n.tracks:\nw [b 0,0 b 6,0 b 0,-6] {1-2 1-3}\n.links:\na:1 w:1@1\np:1 w:2@2\nq:1 w:3@3\n";
  Timer t;Fixture c(t);c.Text(common);
  auto before=c.CaptureRouting({&c.Part("a")});c.Part("a").Move({2,2});
  CHECK(c.Reroute(before,generous()).repaired==1);connected(c.Wire());
  auto g=c.Wire().GetLayout();bool junction=false;
  for(size_t n=0;n<g.nodes.size();++n)
    if(same(g.nodes[n].first,{0,0}))
      {size_t degree=0;for(auto e:g.segments)if(e.from==n || e.to==n)++degree;CHECK(degree==3);junction=true;}
  CHECK(junction);
  c.Text(".chipsets:\ninput a(1) [0,0]\ninput b(1) [0,0]\nPROBE p [8,0]\n.tracks:\nw [b 0,0 b 6,0] {1-2}\n.links:\na:1 w:1@1\nb:1 w:2@1\np:1 w:3@2\n");
  before=c.CaptureRouting({&c.Part("a")});c.Part("a").Move({0,3});
  CHECK(c.Reroute(before,generous()).repaired==1);connected(c.Wire());
  CHECK(c.Wire().GetAttachments()[0].node!=c.Wire().GetAttachments()[1].node);
  for(bool at_pin:{false,true})
    {
      c.Text(std::string(".chipsets:\ninput a(1) [0,0]\nPROBE p [8,0]\n.tracks:\nw [b 0,0 b ")+
        (at_pin?"0,0 t 0,0":"3,0 t 3,0")+" t 6,0] {1-2 2-3 3-4}\n.links:\na:1 w:1@1\np:1 w:2@4\n");
      before=c.CaptureRouting({&c.Part("a")});c.Part("a").Move({0,3});
      CHECK(c.Reroute(before,generous()).repaired==1);connected(c.Wire());
      g=c.Wire().GetLayout();size_t vias=0;
      for(auto e:g.segments)
        if(g.nodes[e.from].second!=g.nodes[e.to].second)
          {++vias;CHECK(same(g.nodes[e.from].first,g.nodes[e.to].first));CHECK(same(g.nodes[e.from].first,{at_pin?0.0:3.0,0}));}
      CHECK(vias==1);
    }
}

static void fallback_and_budget()
{
  Timer t;Fixture c(t);c.Text(base);auto &a=c.Part("a");auto saved=signature(c.Wire());
  auto before=c.CaptureRouting({&a});a.Move({6,0});auto result=c.Reroute(before,generous());
  CHECK(result.repaired==0 && result.expanded==0 && signature(c.Wire())==saved);
  before=c.CaptureRouting({&a});a.Move({1,0});result=c.Reroute(before,generous());
  CHECK(result.repaired==0 && signature(c.Wire())==saved); // still too far from copper
  before=c.CaptureRouting({&a});a.Move({-3,0});result=c.Reroute(before,generous());
  CHECK(result.repaired==1);connected(c.Wire()); // back within the local radius
  c.Text(base);before=c.CaptureRouting({&c.Part("a")});c.Part("a").Move({0,3});saved=signature(c.Wire());
  RoutingBudget zero;zero.max_checks=0;result=c.Reroute(before,zero);
  CHECK(result.limited && result.repaired==0 && result.checks==0 && signature(c.Wire())==saved);
  RoutingBudget tiny=generous();tiny.max_checks=7;result=c.Reroute(before,tiny);
  CHECK(result.limited && result.checks<=7 && result.repaired==0 && signature(c.Wire())==saved);
  // Surround the moved terminal with another net. The original copper survives.
  std::string text=base;
  text.insert(text.find(".links:"),"cage [b -1,2 b 1,2 b 1,4 b -1,4] {1-2 2-3 3-4 4-1}\n");
  c.Text(text);before=c.CaptureRouting({&c.Part("a")});c.Part("a").Move({0,3});saved=signature(c.Wire());
  result=c.Reroute(before,generous());
  CHECK(result.repaired==0 && result.pending==1 && signature(c.Wire())==saved);
  RoutingBudget capped=generous();capped.max_expansions=1;result=c.Reroute(before,capped);
  CHECK(result.expanded<=1 && signature(c.Wire())==saved);
  // Two terminals of the same net form one transaction: if the second one is
  // trapped, the first terminal's successful draft must not reach the model.
  text=base;
  text.insert(text.find(".links:"),"cage [b 11,2 b 13,2 b 13,4 b 11,4] {1-2 2-3 3-4 4-1}\n");
  c.Text(text);before=c.CaptureRouting({&c.Part("a"),&c.Part("p")});
  c.Part("a").Move({0,3});c.Part("p").Move({0,3});saved=signature(c.Wire());
  result=c.Reroute(before,generous());
  CHECK(result.repaired==0 && result.pending==2 && signature(c.Wire())==saved);
  // Nothing physical exists to repair on a bare anchored start.
  c.Text(".chipsets:\ninput a(1) [0,0]\n.tracks:\nw [b 0,0] {}\n.links:\na:1 w:1@1\n");
  CHECK(c.CaptureRouting({&c.Part("a")}).empty());c.Part("a").Move({2,2});
  CHECK(same(c.Wire().GetNodePosition(0),{2,2}));
}

static void repeated_and_grouped()
{
  Timer t;Fixture c(t);c.Text(base);auto &a=c.Part("a");
  std::vector<double> times;
  for(int i=0;i<100;++i)
    {
      auto before=c.CaptureRouting({&a});a.Move({0,i%2?-3.0:3.0});
      auto begin=std::chrono::steady_clock::now();auto result=c.Reroute(before);
      times.push_back(std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-begin).count());
      CHECK(result.repaired==1 && result.checks<=100000 && result.expanded<=4096);
      CHECK(c.Wire().GetNbrSteps()<16);connected(c.Wire());
    }
  std::sort(times.begin(),times.end());
  std::cout<<"Local repair benchmark (sanitised): median "<<times[50]<<" ms, p95 "<<times[95]<<" ms\n";
  auto before=c.CaptureRouting({&a,&c.Part("p")});a.Move({0,5});c.Part("p").Move({0,5});
  CHECK(c.Reroute(before,generous()).repaired==2);connected(c.Wire());
  // Long and fractional original segments keep their distant geometry.
  c.Text(".chipsets:\ninput a(1) [.25,.5]\nPROBE p [32.25,10.5]\n.tracks:\nw [b .25,.5 b 30.25,10.5] {1-2}\n.links:\na:1 w:1@1\np:1 w:2@2\n");
  before=c.CaptureRouting({&c.Part("a")});c.Part("a").Move({1,2});
  CHECK(c.Reroute(before,generous()).repaired==1);connected(c.Wire());
  bool remote=false;auto g=c.Wire().GetLayout();
  for(auto e:g.segments)if(same(g.nodes[e.to].first,{30.25,10.5}) || same(g.nodes[e.from].first,{30.25,10.5}))remote=true;
  CHECK(remote);
}

static void editor()
{
  Timer t;Fixture c(t);c.Text(base);Screen s("fixture.rs");LoopData ld(c,t,s);
  auto mouse=[&](double x,double y){bunny_test_mouse={int(s.pic->buffer.width/2+x*s.pin_size),int(s.pic->buffer.height/2+y*s.pin_size)};};
  auto saved=signature(c.Wire());
  mouse(0,0);screen_click(GO_DOWN,BMB_LEFT,ld);mouse(0,3);
  s.Draw(c);CHECK(signature(c.Wire())==saved); // draw previews never run the router
  screen_click(GO_UP,BMB_LEFT,ld);connected(c.Wire());CHECK(ld.documents[0]->dirty);
  // Ctrl menu and keyboard toggle the same session-wide preference.
  bunny_test_keys[BKS_LCONTROL]=true;screen_key(GO_UP,BKS_F10,ld);
  CHECK(!s.auto_routing && CurrentShortcutMode()==SHORTCUT_CONTROL);
  bunny_test_keys[BKS_LCONTROL]=false;
  saved=signature(c.Wire());mouse(0,3);screen_click(GO_DOWN,BMB_LEFT,ld);mouse(0,5);screen_click(GO_UP,BMB_LEFT,ld);
  CHECK(signature(c.Wire())==saved && same(c.Part("a").GetPosition(),{0,5}));
  bunny_test_keys[BKS_LCONTROL]=true;
  bunny_test_mouse={s.pic->buffer.width*21/(2*ShortcutBarCellCount),20};
  screen_click(GO_DOWN,BMB_LEFT,ld);screen_click(GO_UP,BMB_LEFT,ld);
  CHECK(s.auto_routing);bunny_test_keys[BKS_LCONTROL]=false;
  // Moving selected copper remains an explicit manual operation.
  c.Text(base);s.selected_components={&c.Part("a")};s.selected_tracks={&c.Wire()};
  mouse(0,0);screen_click(GO_DOWN,BMB_LEFT,ld);mouse(0,2);screen_click(GO_UP,BMB_LEFT,ld);
  CHECK(same(c.Wire().GetNodePosition(0),{0,2}));
  CHECK(same(c.Wire().GetNodePosition(2),{12,2}));
  s.selected_components.clear();s.selected_tracks.clear();
  // Page keys also repair the nearby pin after a rotation.
  c.Text(".chipsets:\nBUF gate [0,0]\nPROBE p [11,0]\n.tracks:\nw [b 3,0 b 6,0 b 9,0] {1-2 2-3}\n.links:\ngate:2 w:1@1\np:1 w:2@3\n");
  s.selected_components={&c.Part("gate")};screen_key(GO_DOWN,BKS_PAGEDOWN,ld);
  CHECK(c.Part("gate").GetRotation()==1);connected(c.Wire());
  // A held routing cursor retains its node IDs while components are edited.
  s.active_track=&c.Wire();s.active_node=0;auto old=signature(c.Wire());
  screen_key(GO_DOWN,BKS_PAGEUP,ld);CHECK(signature(c.Wire())==old && s.active_node==0);
}

int main()
{
  fs::path original=fs::current_path();char directory[]="/tmp/bcad-routing-XXXXXX";
  if(!mkdtemp(directory))return 2;
  try
    {
      fs::current_path(directory);
      simple_and_saved();obstacles_and_layers();junctions_and_vias();fallback_and_budget();repeated_and_grouped();editor();
      CHECK(bunny_test_allocations==0);fs::current_path(original);fs::remove_all(directory);
      std::cout<<"PASS local router: "<<checks<<" checks (nets, obstacles, vias, budgets, saves, repeated moves, editor)\n";
      return 0;
    }
  catch(const std::exception &e){std::cerr<<e.what()<<"\nFixture directory: "<<directory<<'\n';return 1;}
}
