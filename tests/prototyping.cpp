#include "Prototype.hpp"
#include "Circuit.hpp"
#include "ComponentMenu.hpp"
#include "6502.hpp"
#include <array>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

extern "C" { bunny_declare_context(screen); }
using namespace hbs;
namespace fs=std::filesystem;
static size_t checks;
#define CHECK(x) do { ++checks; if(!(x))throw std::runtime_error(std::string(__func__)+":"+std::to_string(__LINE__)+": " #x); } while(0)

static void put(const fs::path &path,const std::string &text)
{ std::ofstream f(path,std::ios::binary); f<<text; f.close(); CHECK(f.good()); }
static std::string get(const fs::path &path)
{ std::ifstream f(path,std::ios::binary); return std::string(std::istreambuf_iterator<char>(f),{}); }
struct Fixture : Circuit
{
  explicit Fixture(Timer &timer):Circuit(timer){}
  IComponent &At(const std::string &name) { return *circuit.at(name); }
  Prototype &Part(const std::string &name) { return dynamic_cast<Prototype&>(At(name)); }
  void LoadText(const std::string &text) { put("fixture.rs",text); Load("fixture.rs"); }
};
static void step(Fixture &c,Timer &t) { t.Tick(); c.Compute(); }
static void expect(Fixture &c,const std::string &name,Tristate value)
{ auto actual=c.GetOutputValue(name); if(actual!=value)std::cerr<<name<<" got "<<actual<<", expected "<<value<<'\n'; CHECK(actual==value); }

static void gates()
{
  const Tristate states[]={FALSE,TRUE,UNDEFINED,BROKEN,HIGH_IMPEDANCE};
  const Tristate and_table[5][5]={
    {FALSE,FALSE,FALSE,BROKEN,FALSE}, {FALSE,TRUE,UNDEFINED,BROKEN,UNDEFINED},
    {FALSE,UNDEFINED,UNDEFINED,BROKEN,UNDEFINED}, {BROKEN,BROKEN,BROKEN,BROKEN,BROKEN},
    {FALSE,UNDEFINED,UNDEFINED,BROKEN,UNDEFINED}};
  const Tristate or_table[5][5]={
    {FALSE,TRUE,UNDEFINED,BROKEN,UNDEFINED}, {TRUE,TRUE,TRUE,BROKEN,TRUE},
    {UNDEFINED,TRUE,UNDEFINED,BROKEN,UNDEFINED}, {BROKEN,BROKEN,BROKEN,BROKEN,BROKEN},
    {UNDEFINED,TRUE,UNDEFINED,BROKEN,UNDEFINED}};
  const Tristate xor_table[5][5]={
    {FALSE,TRUE,UNDEFINED,BROKEN,UNDEFINED}, {TRUE,FALSE,UNDEFINED,BROKEN,UNDEFINED},
    {UNDEFINED,UNDEFINED,UNDEFINED,BROKEN,UNDEFINED}, {BROKEN,BROKEN,BROKEN,BROKEN,BROKEN},
    {UNDEFINED,UNDEFINED,UNDEFINED,BROKEN,UNDEFINED}};
  for(int kind=PT_AND;kind<=PT_XNOR;++kind)
    {
      Timer t; Input a(t,"a","0","-10,0"),b(t,"b","0","-10,5");
      Prototype gate(t,PrototypeKind(kind),"gate","","0,0");
      gate.SetLink(1,a,1);
      if(gate.GetPinCount()==3)gate.SetLink(2,b,1);
      for(unsigned i=0;i<5;++i)for(unsigned j=0;j<5;++j)
        {
          t.Tick(); a.SetValue(states[i]); b.SetValue(states[j]);
          Tristate expected=UNDEFINED;
          if(kind==PT_BUF||kind==PT_NOT)expected=states[i]==HIGH_IMPEDANCE?UNDEFINED:states[i];
          else if(kind==PT_AND||kind==PT_NAND)expected=and_table[i][j];
          else if(kind==PT_OR||kind==PT_NOR)expected=or_table[i][j];
          else expected=xor_table[i][j];
          if(kind==PT_NOT||kind==PT_NAND||kind==PT_NOR||kind==PT_XNOR)
            { if(expected==TRUE)expected=FALSE; else if(expected==FALSE)expected=TRUE; }
          CHECK(gate.Compute(gate.GetPinCount())==expected);
          CHECK(!gate.DrivesPin(1) && gate.DrivesPin(gate.GetPinCount()));
          CHECK(a.Compute(1)==states[i]);
        }
      bool rejected=false;
      try{gate.SetLink(gate.GetPinCount()+1,a,1);}catch(const std::exception&){rejected=true;}
      CHECK(rejected);
      gate.DisconnectFrom(&a);gate.DisconnectFrom(&b);
    }
  Timer t; Fixture c(t);
  c.LoadText(".chipsets:\ninput a(1) [-10,0]\nET gate [0,0]\nsonde p [8,0]\n.links:\na:1 gate:1\na:1 gate:2\ngate:3 p:1\n");
  c.Compute(); expect(c,"p",TRUE);
  CHECK(c.GetOutputNum()==1 && !c.At("p").DrivesPin(1));
  CHECK(c.At("gate").GetType()=="AND" && c.At("p").GetType()=="PROBE");
  CHECK(PrototypeColor(FALSE)==Screen::Red && PrototypeColor(TRUE)==Screen::Green);
  CHECK(PrototypeColor(UNDEFINED)==GRAY(128) && PrototypeColor(BROKEN)==Screen::Yellow);
  CHECK(PrototypeColor(HIGH_IMPEDANCE)==Screen::Purple);
  for(auto state:states) {c.SetValue("a",state);step(c,t);expect(c,"p",state==HIGH_IMPEDANCE?UNDEFINED:state);}
}

static void contacts()
{
  for(bool reverse:{false,true})
    {
      Timer t; Fixture c(t);
      c.LoadText(".chipsets:\ninput a(1) [-12,0]\ninput b(Z) [-12,8]\nSWITCH sw(open) [0,0]\nPROBE left [-5,6]\nPROBE right [8,6]\n.links:\n"+
        std::string(reverse?"b:1 sw:2\na:1 sw:1\n":"a:1 sw:1\nb:1 sw:2\n")+
        "sw:1 left:1\nsw:2 right:1\n");
      c.Compute(); expect(c,"right",HIGH_IMPEDANCE); expect(c,"left",TRUE);
      CHECK(c.Part("sw").Toggle()); step(c,t); expect(c,"right",TRUE);expect(c,"left",TRUE);
      c.SetValue("b",FALSE); step(c,t); expect(c,"right",BROKEN);expect(c,"left",BROKEN);
      c.Part("sw").Toggle();step(c,t);expect(c,"right",FALSE);expect(c,"left",TRUE);
      c.Part("sw").Toggle();c.SetValue("a",HIGH_IMPEDANCE);step(c,t);
      expect(c,"left",FALSE);expect(c,"right",FALSE); // both directions
    }
  Timer t; Fixture c(t);
  c.LoadText(".chipsets:\ninput a(1) [-20,0]\ninput b(Z) [-20,8]\nSWITCH s(closed) [0,0]\nSWITCH r(closed) [8,0]\nPROBE p [0,8]\nPROBE q [8,8]\n.links:\na:1 s:1\ns:2 r:1\nr:2 s:1\ns:2 p:1\nr:2 q:1\nb:1 r:1\n");
  c.Compute();expect(c,"p",TRUE);expect(c,"q",TRUE);
  c.SetValue("b",FALSE);step(c,t);expect(c,"q",BROKEN);expect(c,"p",BROKEN);
  c.SetValue("a",HIGH_IMPEDANCE);step(c,t);expect(c,"p",FALSE);expect(c,"q",FALSE);
  c.SetValue("b",HIGH_IMPEDANCE);step(c,t);expect(c,"p",HIGH_IMPEDANCE);expect(c,"q",HIGH_IMPEDANCE);
}

static void discrete()
{
  for(const std::string type:{"NPN","PNP"})
    {
      Timer t;Fixture c(t);
      c.LoadText(".chipsets:\ninput base(0) [-12,0]\ntrue hi [-12,-8]\nfalse lo [-12,8]\nRESISTOR pull(10k) [0,-8]\n"+type+" transistor [0,0]\nPROBE p [10,0]\n.links:\nbase:1 transistor:1\nhi:1 pull:1\npull:2 transistor:2\nlo:1 transistor:3\ntransistor:2 p:1\n");
      for(auto state:{FALSE,TRUE,UNDEFINED,BROKEN,HIGH_IMPEDANCE})
        {
          c.SetValue("base",state);step(c,t);
          Tristate expected=state==BROKEN?BROKEN:
            state==UNDEFINED||state==HIGH_IMPEDANCE?UNDEFINED:
            (state==(type=="NPN"?TRUE:FALSE)?FALSE:TRUE);
          expect(c,"p",expected);
        }
    }
  Timer t;Fixture c(t);
  c.LoadText(".chipsets:\ninput a(0) [-15,0]\ninput b(0) [-15,7]\nfalse lo [-15,14]\nDIODE da [0,0]\nDIODE db [0,7]\nRESISTOR pull(10k) [7,14]\nPROBE p [12,0]\n.links:\na:1 da:1\nb:1 db:1\nda:2 p:1\ndb:2 p:1\nlo:1 pull:1\npull:2 p:1\n");
  for(unsigned a=0;a<2;++a)for(unsigned b=0;b<2;++b)
    {c.SetValue("a",a?TRUE:FALSE);c.SetValue("b",b?TRUE:FALSE);step(c,t);expect(c,"p",a||b?TRUE:FALSE);}
  c.LoadText(".chipsets:\ninput a(Z) [-10,0]\ntrue hi [10,0]\nDIODE d [0,0]\nPROBE p [-5,7]\n.links:\na:1 d:1\nhi:1 d:2\nd:1 p:1\n");
  c.Compute();expect(c,"p",HIGH_IMPEDANCE); // no backwards high propagation

  c.LoadText(".chipsets:\ninput a(1) [-10,0]\ninput k(0) [10,0]\nLED led [0,0]\nCAPACITOR cap(47uF) [0,6]\nPROBE p [10,6]\n.links:\na:1 led:1\nk:1 led:2\na:1 cap:1\ncap:2 p:1\n");
  c.Compute(); CHECK(c.Part("led").IsLit()); expect(c,"p",HIGH_IMPEDANCE);
  c.SetValue("k",TRUE);step(c,t);CHECK(!c.Part("led").IsLit());
  c.SetValue("k",FALSE);c.SetValue("a",UNDEFINED);step(c,t);CHECK(!c.Part("led").IsLit());
  expect(c,"p",HIGH_IMPEDANCE);

  // A direct driver wins over a weak resistor; equal weak opponents conflict.
  c.LoadText(".chipsets:\ntrue hi [-10,0]\nfalse lo [-10,8]\nRESISTOR up [0,0]\nRESISTOR down [0,8]\nPROBE p [10,0]\n.links:\nhi:1 up:1\nlo:1 down:1\nup:2 p:1\ndown:2 p:1\n");
  c.Compute();expect(c,"p",BROKEN);
}

static void buttons_and_roundtrip()
{
  for(const std::string type:{"PUSHBUTTON","PUSHBUTTON_COMPACT"})
    {
      Timer t;Fixture c(t);
      c.LoadText(".chipsets:\nfalse lo [-12,0]\ntrue hi [-12,8]\nRESISTOR pull(22k) [0,8]\n"+type+" button [0,0]\nPROBE l [-5,6]\nPROBE r [8,6]\nSWITCH sw(closed) [15,0]\nCAPACITOR cap(100uF16V) [15,8]\n.links:\nlo:1 button:1\nbutton:2 l:1\nbutton:4 r:1\nbutton:3 pull:2\nhi:1 pull:1\n");
      c.Compute();expect(c,"l",FALSE);expect(c,"r",TRUE);
      auto &button=c.Part("button");
      CHECK(button.GetPinCount()==4);
      auto p1=button.GetPinPosition(1),p2=button.GetPinPosition(2),p3=button.GetPinPosition(3);
      CHECK(p3.x-p1.x==3 && p1.x==p2.x && p2.y-p1.y==(type=="PUSHBUTTON"?3:1));
      CHECK(button.SetPressed(true));step(c,t);expect(c,"l",FALSE);expect(c,"r",FALSE);
      CHECK(c.Save("roundtrip.rs"));
      Fixture copy(t);copy.Load("roundtrip.rs");
      CHECK(!copy.Part("button").IsClosed() && copy.Part("sw").IsClosed());
      CHECK(copy.Part("pull").GetDefinitionValue()=="22k");
      CHECK(copy.Part("cap").GetDefinitionValue()=="100uF16V");
      for(size_t pin=1;pin<=4;++pin)
        {auto before=button.GetPinPosition(pin),after=copy.Part("button").GetPinPosition(pin);CHECK(before.x==after.x&&before.y==after.y);}
      CHECK(c.ReleaseMomentaryButtons());step(c,t);expect(c,"r",TRUE);
      CHECK(!c.ReleaseMomentaryButtons());
    }
}

static void ready_pullup()
{
  // Exercise the new weak path with the W65C02S's real open-drain RDY driver.
  // Constant $CB on the data bus supplies reset vectors and then a WAI opcode.
  Timer t;Fixture c(t);
  std::string text=".chipsets:\nW65C02S cpu [0,0]\ninput phi(0)\ninput irq(1)\ntrue hi\nRESISTOR pull(10k)\nPROBE ready\n";
  for(unsigned bit=0;bit<8;++bit)
    text+="input d"+std::to_string(bit)+((0xcb>>bit)&1?"(1)\n":"(0)\n");
  text+=".links:\nphi:1 cpu:37\nirq:1 cpu:4\nhi:1 cpu:40\nhi:1 cpu:6\nhi:1 cpu:36\nhi:1 cpu:38\nhi:1 pull:1\npull:2 cpu:2\ncpu:2 ready:1\n";
  for(unsigned bit=0;bit<8;++bit)
    text+="d"+std::to_string(bit)+":1 cpu:"+std::to_string(33-bit)+"\n";
  c.LoadText(text);c.Compute();expect(c,"ready",TRUE);
  auto cycle=[&]{c.SetValue("phi",TRUE);step(c,t);c.SetValue("phi",FALSE);step(c,t);};
  for(unsigned i=0;i<20;++i)cycle();
  auto &cpu=dynamic_cast<CMP_6502&>(c.At("cpu"));
  CHECK(cpu.GetCore().IsWaiting());expect(c,"ready",FALSE);
  c.SetValue("irq",FALSE);cycle();
  CHECK(!cpu.GetCore().IsWaiting());expect(c,"ready",TRUE);
}

static void interaction()
{
  Timer t;Fixture c(t);
  c.LoadText(".chipsets:\ntrue hi [-20,10]\nSWITCH sw [0,0]\nPUSHBUTTON button [12,0]\nPROBE p [6,6]\n.links:\nhi:1 sw:1\nsw:2 p:1\n");
  Screen s("fixture.rs");LoopData ld(c,t,s);
  auto mouse=[&](double x,double y){bunny_test_mouse={int(s.pic->buffer.width/2+x*s.pin_size),int(s.pic->buffer.height/2+y*s.pin_size)};};
  mouse(0,0);screen_click(GO_UP,BMB_MIDDLE,ld);
  CHECK(c.Part("sw").IsClosed() && ld.documents[0]->dirty);expect(c,"p",TRUE);
  mouse(13,1);screen_click(GO_DOWN,BMB_MIDDLE,ld);CHECK(c.Part("button").IsClosed());
  mouse(-100,-100);screen_click(GO_UP,BMB_MIDDLE,ld);CHECK(!c.Part("button").IsClosed());
  mouse(13,1);screen_click(GO_DOWN,BMB_MIDDLE,ld);CHECK(c.Part("button").IsClosed());
  CHECK(ld.NewDocument());
  screen_click(GO_UP,BMB_MIDDLE,ld);CHECK(!c.Part("button").IsClosed());
  CHECK(ld.SelectDocument(0));
  mouse(13,1);screen_click(GO_DOWN,BMB_MIDDLE,ld);CHECK(c.Part("button").IsClosed());
  std::set<IComponent*> components{&c.Part("button")};std::set<Track*> tracks;
  CHECK(c.DeleteSelected(components,tracks));
  screen_click(GO_UP,BMB_MIDDLE,ld); // no stale pointer after deletion
}

static std::string xml(std::string s)
{
  std::string out;
  for(char c:s)out+=c=='&'?"&amp;":c=='<'?"&lt;":c=='>'?"&gt;":std::string(1,c);
  return out;
}
class SvgCanvas : public SymbolCanvas
{
  std::ostream &out;
  double x,y,scale;
  std::string Color(unsigned c)
  { std::ostringstream s;s<<'#'<<std::hex<<std::setfill('0')<<std::setw(6)<<(c>>8);return s.str(); }
public:
  SvgCanvas(std::ostream &out,double x,double y,double scale):out(out),x(x),y(y),scale(scale){}
  void Line(Position a,Position b,unsigned color) override
  {out<<"<path d=\"M "<<x+a.x*scale<<' '<<y+a.y*scale<<" L "<<x+b.x*scale<<' '<<y+b.y*scale<<"\" fill=\"none\" stroke=\""<<Color(color)<<"\" stroke-width=\"2\" stroke-linecap=\"round\"/>\n";}
  void Circle(Position p,Size radius,unsigned color,bool fill) override
  {out<<"<ellipse cx=\""<<x+p.x*scale<<"\" cy=\""<<y+p.y*scale<<"\" rx=\""<<radius.x*scale<<"\" ry=\""<<radius.y*scale<<"\" fill=\""<<(fill?Color(color):"none")<<"\" stroke=\""<<Color(color)<<"\" stroke-width=\"2\"/>\n";}
  void Text(Position p,double height,unsigned color,const std::string &text) override
  {out<<"<text x=\""<<x+p.x*scale<<"\" y=\""<<y+(p.y+height*.8)*scale<<"\" text-anchor=\"middle\" font-family=\"DejaVu Sans,sans-serif\" font-size=\""<<height*scale<<"\" fill=\""<<Color(color)<<"\">"<<xml(text)<<"</text>\n";}
};
static void svg(const fs::path &path)
{
  std::ofstream out(path);
  out<<"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"1400\" height=\"1230\" viewBox=\"0 0 1400 1230\"><rect width=\"1400\" height=\"1230\" fill=\"#11161d\"/>\n";
  SvgCanvas title(out,700,25,1);title.Text({0,0},25,Screen::White,"BCAD - primitives de prototypage");
  unsigned index=0;
  for(int k=PT_AND;k<PT_INVALID;++k)
    {
      auto kind=PrototypeKind(k);if(kind==PT_PROBE)continue;
      double x=175+(index%4)*350, y=155+(index/4)*205;
      bool button=kind==PT_PUSHBUTTON||kind==PT_PUSHBUTTON_COMPACT;
      if(button){x-=1.5*27;y-=(kind==PT_PUSHBUTTON?1.5:.5)*27;}
      SvgCanvas c(out,x,y,27);
      Tristate states[4]={TRUE,FALSE,TRUE,FALSE};
      DrawPrototypeSymbol(c,kind,PrototypeName(kind),kind==PT_RESISTOR?"10k":kind==PT_CAPACITOR?"47uF":"",false,states,kind==PT_LED);
      ++index;
    }
  const Tristate values[]={FALSE,TRUE,UNDEFINED,BROKEN,HIGH_IMPEDANCE};
  const char *labels[]={"FAUX","VRAI","UNDEF","BROKEN","HAUTE IMPEDANCE"};
  for(unsigned i=0;i<5;++i)
    {SvgCanvas c(out,170+i*270,1155,27);Tristate state[4]={values[i],UNDEFINED,UNDEFINED,UNDEFINED};DrawPrototypeSymbol(c,PT_PROBE,labels[i],"",false,state);}
  out<<"</svg>\n";out.close();CHECK(out.good());
}

int main(int argc,char **argv)
{
  fs::path original=fs::current_path();
  char directory[]="/tmp/bcad-prototyping-XXXXXX";
  char *made=mkdtemp(directory);if(!made)return 2;
  try
    {
      fs::current_path(directory);
      gates();contacts();discrete();buttons_and_roundtrip();ready_pullup();interaction();
      if(argc==3 && std::string(argv[1])=="--svg")svg(fs::absolute(argv[2]));
      CHECK(bunny_test_allocations==0);
      fs::current_path(original);fs::remove_all(directory);
      std::cout<<"PASS prototyping: "<<checks<<" checks (logic, passive networks, buttons, colours, saves, editor)\n";
      return 0;
    }
  catch(const std::exception &e)
    {std::cerr<<e.what()<<"\nFixture directory: "<<directory<<'\n';return 1;}
}
