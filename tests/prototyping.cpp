#include "Prototype.hpp"
#include "Circuit.hpp"
#include "ComponentMenu.hpp"
#include "Shortcuts.hpp"
#include "6502.hpp"
#include "Track.hpp"
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

static void routing_anchors()
{
  Timer t;Fixture c(t);
  // Reproduce a saved file with a routing start left behind its input.
  c.LoadText(".chipsets:\ninput a(1) [0,0]\nPROBE p [15,0]\n.tracks:\nw [b -8,4] {}\n.links:\na:1 w:1@1\np:1 w:2@1\n");
  Screen s("fixture.rs");LoopData ld(c,t,s);
  auto &input=c.At("a");
  size_t node=Track::NoNode;
  auto track=c.FindTrackAttachedTo(&input,1,&node);
  CHECK(track && node==0);
  CHECK(track->GetNodePosition(node).x==0 && track->GetNodePosition(node).y==0);
  CHECK(track->FindNode(s,{-8,4})==Track::NoNode);
  auto mouse=[&](double x,double y){bunny_test_mouse={int(s.pic->buffer.width/2+x*s.pin_size),int(s.pic->buffer.height/2+y*s.pin_size)};};
  mouse(0,0);screen_click(GO_DOWN,BMB_LEFT,ld);
  mouse(5,3);
  CHECK(c.Save("before-preview.rs"));
  s.Draw(c);
  CHECK(input.GetPosition().x==0 && input.GetPosition().y==0);
  CHECK(track->GetNodePosition(node).x==0 && track->GetNodePosition(node).y==0);
  CHECK(c.Save("after-preview.rs"));
  CHECK(get("before-preview.rs")==get("after-preview.rs"));
  screen_click(GO_UP,BMB_LEFT,ld);
  CHECK(input.GetPosition().x==5 && input.GetPosition().y==3);
  CHECK(track->GetNodePosition(node).x==5 && track->GetNodePosition(node).y==3);
  CHECK(track->FindNode(s,{0,0})==Track::NoNode && track->FindNode(s,{5,3})==node);
  track->MoveNode(node,{20,20});
  CHECK(track->GetNodePosition(node).x==5 && track->GetNodePosition(node).y==3);
  // Moving a selection containing both the input and its track applies the
  // displacement only once, including the temporary drag preview and undo.
  track->Move({2,-1});input.Move({2,-1});
  CHECK(track->GetNodePosition(node).x==7 && track->GetNodePosition(node).y==2);
  track->Move({-2,1});input.Move({-2,1});
  CHECK(track->GetNodePosition(node).x==5 && track->GetNodePosition(node).y==3);
  step(c,t);expect(c,"p",TRUE);
  CHECK(c.Save("bare-anchor.rs"));
  CHECK(get("bare-anchor.rs").find("b 5,3] {}")!=std::string::npos);
  Fixture copy(t);copy.Load("bare-anchor.rs");
  auto copied=copy.FindTrackAttachedTo(&copy.At("a"),1);
  CHECK(copied && copied->GetNodePosition(0).x==5 && copied->GetNodePosition(0).y==3);
  // Laying the first segment freezes the pin's current position, even if no
  // draw or save refreshed the old stored coordinates after the last move.
  input.Move({2,-1});
  CHECK(track->GetStep(node)==std::make_pair(7,2));
  track->AddNodeAfter(node,{12,2});
  CHECK(track->GetNodePosition(node).x==7 && track->GetNodePosition(node).y==2);
  input.Move({0,5});
  CHECK(track->GetNodePosition(node).x==7 && track->GetNodePosition(node).y==2);
  track->InsertNodeOnSegment(0,{9,2});
  CHECK(track->GetNodePosition(node).x==7 && track->GetNodePosition(node).y==2);
  CHECK(c.Save("routed-anchor.rs"));
  Fixture routed(t);routed.Load("routed-anchor.rs");
  auto fixed=routed.FindTrackAttachedTo(&routed.At("a"),1);
  CHECK(fixed && fixed->GetNodePosition(0).x==7 && fixed->GetNodePosition(0).y==2);
  CHECK(routed.At("a").GetPosition().y==7);
  step(routed,t);expect(routed,"p",TRUE);
  while(!track->GetSegments().empty())CHECK(track->DeleteBranchFromSegment(0));
  CHECK(track->GetNodePosition(0).x==7 && track->GetNodePosition(0).y==7);
  s.selected_components.clear();s.selected_tracks.clear();
  size_t before=t.GetTime();c.Draw(s);
  CHECK(t.GetTime()==before);expect(c,"p",TRUE);
  // Dashed airwires spanning far outside the viewport stay bounded and do not
  // change simulation state. The suite timeout catches unbounded subdivision.
  s.DashedLine({-1000000,0},{1000000,0},GRAY(160));
  s.DashedLine({0,-1000000},{0,1000000},GRAY(160));
}

static size_t preview_lines;
static unsigned preview_color;
static void capture_preview(const char *kind,t_bunny_position,t_bunny_position,unsigned color)
{ if(std::string(kind)=="line") {++preview_lines;preview_color=color;} }

static void routing_gestures()
{
  Timer t;Fixture c(t);
  c.LoadText(".chipsets:\ninput a(1) [0,0]\nPROBE p [16,0]\n.links:\n");
  Screen s("fixture.rs");LoopData ld(c,t,s);
  auto mouse=[&](double x,double y){bunny_test_mouse={
    int(s.pic->buffer.width/2+(x-s.camera.x)*s.pin_size),
    int(s.pic->buffer.height/2+(y-s.camera.y)*s.pin_size)};};
  auto click=[&](t_bunny_mouse_button button){
    CHECK(screen_click(GO_DOWN,button,ld)==GO_ON);
    CHECK(screen_click(GO_UP,button,ld)==GO_ON);
  };
  Position from,to;unsigned color;
  CHECK(!s.drawing_mode && !s.GetRoutePreview(c,from,to,color));

  // Ctrl routes from a pin without moving its component or changing SEL/DRAW.
  bunny_test_keys[BKS_LCONTROL]=true;
  mouse(0,0);click(BMB_LEFT);
  auto track=s.active_track;
  CHECK(track && s.active_node==0 && !s.drawing_mode);
  CHECK(track->GetNbrSteps()==1 && track->GetSegments().empty());
  CHECK(c.At("a").GetPosition().x==0 && c.At("a").GetPosition().y==0);
  mouse(4,-2);
  CHECK(s.GetRoutePreview(c,from,to,color));
  CHECK(from.x==0 && from.y==0 && to.x==4 && to.y==-2 && color==Screen::Blue);
  auto pin=c.At("p").GetPinPosition(1);
  mouse(pin.x+.2,pin.y+.2);
  CHECK(s.GetRoutePreview(c,from,to,color));
  CHECK(to.x==pin.x && to.y==pin.y);
  c.CreateUserTrackAt({-5.25,-3.25});
  mouse(-5,-3);
  CHECK(s.GetRoutePreview(c,from,to,color));
  CHECK(to.x==-5.25 && to.y==-3.25);

  // Drawing the rubber band neither places copper nor changes a saved file.
  mouse(4,-2);
  CHECK(c.Save("route-before-preview.rs"));
  auto time=t.GetTime();ld.documents[0]->dirty=false;
  preview_lines=0;bunny_test_draw=capture_preview;
  s.DrawRoutePreview(c);
  bunny_test_draw=nullptr;
  CHECK(preview_lines>0 && preview_color==Screen::Blue);
  CHECK(t.GetTime()==time && !ld.documents[0]->dirty);
  CHECK(track->GetNbrSteps()==1 && track->GetSegments().empty());
  CHECK(c.Save("route-after-preview.rs"));
  CHECK(get("route-before-preview.rs")==get("route-after-preview.rs"));
  click(BMB_LEFT);
  CHECK(s.active_track==track && track->GetNbrSteps()==2 && track->GetSegments().size()==1);
  auto endpoint=s.active_node;

  // Ctrl still selects its alternate toolbar, including while a route is active.
  CHECK(CurrentShortcutMode()==SHORTCUT_CONTROL);
  bunny_test_mouse={s.pic->buffer.width*5/(2*ShortcutBarCellCount),20};
  click(BMB_LEFT); // This Ctrl slot is unassigned; it must not route or save.
  CHECK(!ld.file_browser && !s.panning && ld.documents[0]->dirty);
  CHECK(s.active_track==track && s.active_node==endpoint && track->GetNbrSteps()==2);
  bunny_test_keys[BKS_LSHIFT]=true;
  CHECK(CurrentShortcutMode()==SHORTCUT_CONTROL_SHIFT);
  bunny_test_keys[BKS_LCONTROL]=false;
  CHECK(CurrentShortcutMode()==SHORTCUT_SHIFT);
  bunny_test_mouse={s.pic->buffer.width*7/(2*ShortcutBarCellCount),20};
  bunny_test_keys[BKS_LCONTROL]=true;
  click(BMB_LEFT); // Ctrl+Shift+F3: SAVE COPY, with the route paused.
  CHECK(ld.file_browser && ld.file_browser_mode==FILE_BROWSER_SAVE_COPY);
  CHECK(s.active_track==track && s.active_node==endpoint && track->GetNbrSteps()==2);
  screen_key(GO_UP,BKS_ESCAPE,ld);
  CHECK(!ld.file_browser && !ld.quit_requested);
  bunny_test_keys[BKS_LSHIFT]=false;
  bunny_test_keys[BKS_LCONTROL]=false;
  bunny_test_keys[BKS_LCONTROL]=true;
  screen_key(GO_DOWN,BKS_S,ld);
  CHECK(!ld.documents[0]->dirty && s.active_track==track && s.active_node==endpoint);

  // Releasing Ctrl keeps the pending route. Left drag pans even on an input.
  bunny_test_keys[BKS_LCONTROL]=false;screen_key(GO_UP,BKS_LCONTROL,ld);
  CHECK(s.active_track==track && s.active_node==endpoint && !s.IsRoutingMode());
  CHECK(s.GetRoutePreview(c,from,to,color));
  mouse(0,0);screen_click(GO_DOWN,BMB_LEFT,ld);
  CHECK(s.panning && !s.dragging_selection && !s.selecting);
  CHECK(!s.GetRoutePreview(c,from,to,color));
  auto camera=s.camera;
  bunny_test_mouse.x+=70;bunny_test_mouse.y+=20;
  t_bunny_position delta{70,20};screen_move(&delta,ld);
  CHECK(s.camera.x!=camera.x && s.camera.y!=camera.y);
  // Pressing Ctrl during a pan must not reinterpret the mouse release.
  bunny_test_keys[BKS_LCONTROL]=true;
  screen_click(GO_UP,BMB_LEFT,ld);
  CHECK(!s.panning && s.active_track==track && s.active_node==endpoint);
  CHECK(track->GetNbrSteps()==2 && c.At("a").GetPosition().x==0 && c.At("a").GetPosition().y==0);
  mouse(9,-5);click(BMB_LEFT);
  CHECK(track->GetNbrSteps()==3 && track->GetSegments().size()==2);
  CHECK(track->GetNodePosition(s.active_node).x==9 && track->GetNodePosition(s.active_node).y==-5);
  s.ZoomAt(bunny_test_mouse,4);mouse(12,-6);
  CHECK(s.GetRoutePreview(c,from,to,color));
  CHECK(from.x==9 && from.y==-5 && to.x==12 && to.y==-6);
  screen_key(GO_DOWN,BKS_V,ld);
  CHECK(s.GetRoutePreview(c,from,to,color) && color==Screen::Red);

  // A right drag keeps the route; a short right click detaches it while paused.
  bunny_test_keys[BKS_LCONTROL]=false;
  screen_click(GO_DOWN,BMB_RIGHT,ld);
  bunny_test_mouse.x+=20;delta={20,0};screen_move(&delta,ld);
  screen_click(GO_UP,BMB_RIGHT,ld);
  CHECK(!s.panning && s.active_track==track && !s.context_menu);
  auto nodes=track->GetNbrSteps(),segments=track->GetSegments().size();
  click(BMB_RIGHT);
  CHECK(!s.active_track && s.active_node==Track::NoNode && !s.context_menu);
  CHECK(track->GetNbrSteps()==nodes && track->GetSegments().size()==segments);
  CHECK(!s.GetRoutePreview(c,from,to,color));

  // An existing component drag still finishes if Ctrl is pressed mid-gesture.
  mouse(0,0);screen_click(GO_DOWN,BMB_LEFT,ld);
  CHECK(s.dragging_selection);
  mouse(2,2);bunny_test_keys[BKS_RCONTROL]=true;
  screen_click(GO_UP,BMB_LEFT,ld);
  CHECK(!s.dragging_selection && !s.active_track);
  CHECK(c.At("a").GetPosition().x==2 && c.At("a").GetPosition().y==2);
  mouse(-8,4);click(BMB_LEFT);
  CHECK(s.active_track && !s.drawing_mode && s.IsRoutingMode());
  // Releasing Ctrl between mouse down/up must not turn that click into a drag.
  mouse(-4,6);screen_click(GO_DOWN,BMB_LEFT,ld);
  auto temporary=s.active_track;
  bunny_test_keys[BKS_RCONTROL]=false;screen_key(GO_UP,BKS_RCONTROL,ld);
  screen_click(GO_UP,BMB_LEFT,ld);
  CHECK(s.active_track==temporary && temporary->GetNbrSteps()==2);
  CHECK(c.At("a").GetPosition().x==2 && c.At("a").GetPosition().y==2);
  click(BMB_RIGHT);

  // Permanent DRAW still works without Ctrl. Releasing a camera drag over the
  // shortcut bar stops panning and does not trigger NEW or another toolbar action.
  s.drawing_mode=true;
  mouse(-10,-6);click(BMB_LEFT);
  CHECK(s.active_track && s.GetRoutePreview(c,from,to,color));
  track=s.active_track;nodes=track->GetNbrSteps();
  bunny_test_keys[BKS_SPACE]=true;
  mouse(-5,-5);screen_click(GO_DOWN,BMB_LEFT,ld);
  CHECK(s.panning);
  bunny_test_mouse={5,10};delta={0,0};screen_move(&delta,ld);
  bunny_test_keys[BKS_SPACE]=false;
  screen_click(GO_UP,BMB_LEFT,ld);
  CHECK(!s.panning && ld.documents.size()==1 && !ld.file_browser);
  CHECK(s.active_track==track && track->GetNbrSteps()==nodes && s.drawing_mode);
  // Continue inside the canvas: after this long pan the former test position
  // falls in the new favorites column at the left edge of the window.
  bunny_test_mouse={s.pic->buffer.width/2,s.pic->buffer.height/2};click(BMB_LEFT);
  CHECK(track->GetNbrSteps()==nodes+1);
  click(BMB_RIGHT);
  CHECK(!s.active_track);
}

static bool same(Position a,Position b)
{ return std::hypot(a.x-b.x,a.y-b.y)<1e-9; }

static void rotation_geometry()
{
  Timer t;Fixture c(t);Screen s("fixture.rs");
  // Every built-in package keeps its anchor and pin numbering through four
  // turns, including the multiple-inheritance terminal and the CPU's 40 pins.
  auto types=c.GetCreatableTypes();types.push_back("screen");
  for(const auto &type:types)
    {
      if(type=="wire" || type=="2716")continue;
      auto part=c.CreateUserComponent(type,{7,-5});
      std::vector<Position> pins;
      for(size_t pin=1;pin<=part->GetPinCount();++pin)pins.push_back(part->GetPinPosition(pin));
      for(int turn=1;turn<=4;++turn)
        {
          part->Rotate(1);
          CHECK(same(part->GetPosition(),{7,-5}) && part->GetRotation()==turn%4);
          double angle=turn*std::acos(-1.0)/2;
          for(size_t pin=1;pin<=pins.size();++pin)
            {
              Position delta=pins[pin-1]-Position{7,-5};
              Position expected={7+delta.x*std::cos(angle)-delta.y*std::sin(angle),
                                 -5+delta.x*std::sin(angle)+delta.y*std::cos(angle)};
              CHECK(same(part->GetPinPosition(pin),expected));
            }
          part->Draw(s);
        }
    }
  auto gate=c.CreateUserComponent("AND",{0,0});gate->Rotate(1);
  CHECK(same(gate->GetPinPosition(1),{1,-3}) && same(gate->GetPinPosition(3),{0,3}));
  CHECK(gate->IsUnder(s,{0,3}) && !gate->IsUnder(s,{3,0}));
  auto dip=c.CreateUserComponent("4001",{0,0});dip->Rotate(-1);
  CHECK(dip->IsUnder(s,{5,-1}) && !dip->IsUnder(s,{1,5}));
  auto terminal=c.CreateUserComponent("terminal",{0,0});terminal->Rotate(1);
  CHECK(terminal->IsUnder(s,{-4,1}) && same(terminal->GetPinPosition(9),{-8,0}));
  auto package=c.CreateUserComponent("rotation_package",{10,-10});package->Rotate(1);
  CHECK(same(package->GetPinPosition(2),{8,-6}) && package->IsUnder(s,{9,-8}));
  auto contents=c.CreateUserComponent("rotation_contents",{10,10});contents->Rotate(1);
  CHECK(same(contents->GetPinPosition(1),{10,10}) && same(contents->GetPinPosition(2),{10,14}));
  CHECK(contents->IsUnder(s,{10,14}));
  CHECK(c.Save("rotations.rs"));
  Fixture copy(t);copy.Load("rotations.rs");
  for(const auto &name:{gate->GetName(),dip->GetName(),terminal->GetName(),package->GetName(),contents->GetName()})
    {
      auto &before=c.At(name),&after=copy.At(name);
      CHECK(after.GetRotation()==before.GetRotation());
      for(size_t pin=1;pin<=before.GetPinCount();++pin)CHECK(same(before.GetPinPosition(pin),after.GetPinPosition(pin)));
    }
  CHECK(copy.Save("rotations-copy.rs"));
  CHECK(get("rotations.rs")==get("rotations-copy.rs"));
  auto saved=get("rotations-copy.rs");
  for(const std::string angle:{"45","360","nan","90,0","90oops",""})
    {
      bool rejected=false;
      try{copy.LoadText(".chipsets:\nAND bad [0,0,"+angle+"]\n.links:\n");}
      catch(const std::exception&){rejected=true;}
      CHECK(rejected && copy.Save("rotation-rejected.rs"));
      CHECK(get("rotation-rejected.rs")==saved);
    }
}

static void rotation_editor()
{
  Timer t;Fixture c(t);
  c.LoadText(".chipsets:\ninput a(1) [-12,0]\nAND gate [0,0]\nPROBE p [12,0]\n.tracks:\nw [b 3,0] {}\n.links:\na:1 gate:1\na:1 gate:2\ngate:3 w:1@1\np:1 w:2@1\n");
  c.Compute();expect(c,"p",TRUE);
  Screen s("fixture.rs");LoopData ld(c,t,s);
  s.auto_routing=false; // Exercise the manual-copper mode as well.
  auto &gate=c.At("gate");auto time=t.GetTime();
  ld.documents[0]->dirty=false;
  screen_key(GO_DOWN,BKS_PAGEDOWN,ld);
  CHECK(gate.GetRotation()==0 && !ld.documents[0]->dirty);
  s.selected_components.insert(&gate);
  screen_key(GO_UP,BKS_PAGEDOWN,ld);CHECK(gate.GetRotation()==0);
  screen_key(GO_DOWN,BKS_PAGEDOWN,ld);
  CHECK(gate.GetRotation()==1 && ld.documents[0]->dirty && t.GetTime()==time);
  auto pin=c.GetPinAt(s,{1,-3});CHECK(pin.component==&gate && pin.pin==1);
  CHECK(c.GetComponent(s,{0,3})==&gate);
  auto track=c.FindTrackAttachedTo(&gate,3);
  CHECK(track && same(track->GetNodePosition(0),{0,3}));
  track->AddNodeAfter(0,{6,3});
  screen_key(GO_DOWN,BKS_PAGEUP,ld);
  CHECK(gate.GetRotation()==0 && same(track->GetNodePosition(0),{0,3}));
  CHECK(same(gate.GetPinPosition(3),{3,0}));
  expect(c,"p",TRUE);
  s.selected_components.insert(&c.At("p"));
  screen_key(GO_DOWN,BKS_PAGEUP,ld);
  CHECK(gate.GetRotation()==3 && c.At("p").GetRotation()==3);
  CHECK(same(gate.GetPosition(),{0,0}) && same(c.At("p").GetPosition(),{12,0}));
  CHECK(c.Save("rotation-before-draw.rs"));s.Draw(c);
  CHECK(c.Save("rotation-after-draw.rs"));
  CHECK(get("rotation-before-draw.rs")==get("rotation-after-draw.rs") && t.GetTime()==time);

  // Page keys keep their original navigation role while a panel is open.
  s.search_panel=true;s.search_query="CAPACITOR";
  screen_key(GO_DOWN,BKS_PAGEDOWN,ld);CHECK(s.search_offset==0 && gate.GetRotation()==3); // One matching capacitor: stay on that row.
  screen_key(GO_DOWN,BKS_PAGEUP,ld);CHECK(s.search_offset==0);
  screen_key(GO_DOWN,BKS_RETURN,ld);
  CHECK(s.placing_component && s.component_to_place->GetType()=="CAPACITOR");
  auto placed=s.component_to_place;
  screen_key(GO_DOWN,BKS_PAGEUP,ld);CHECK(placed->GetRotation()==3);
  bunny_test_mouse={s.pic->buffer.width/2+100,s.pic->buffer.height/2+100};
  bunny_test_keys[BKS_LSHIFT]=true;
  screen_click(GO_DOWN,BMB_LEFT,ld);screen_click(GO_UP,BMB_LEFT,ld);
  bunny_test_keys[BKS_LSHIFT]=false;
  CHECK(s.placing_component && s.component_to_place!=placed && s.component_to_place->GetRotation()==3);
  screen_key(GO_DOWN,BKS_PAGEDOWN,ld);CHECK(s.component_to_place->GetRotation()==0);
  screen_click(GO_DOWN,BMB_RIGHT,ld);screen_click(GO_UP,BMB_RIGHT,ld);
  CHECK(!s.placing_component && placed->GetRotation()==3);
  s.selected_components.insert(&gate);
  ld.BeginSaveAs(true);screen_key(GO_DOWN,BKS_PAGEDOWN,ld);
  CHECK(gate.GetRotation()==3 && ld.file_browser);
  screen_key(GO_UP,BKS_ESCAPE,ld);
  s.rename_mode=true;screen_key(GO_DOWN,BKS_PAGEUP,ld);
  CHECK(gate.GetRotation()==3);s.rename_mode=false;
}

static std::map<std::pair<int,int>,unsigned> drawn_pixels;
static void capture_pixels(const char *kind,t_bunny_position p,t_bunny_position,unsigned color)
{ if(std::string(kind)=="pixel")drawn_pixels[{p.x,p.y}]=color; }
static void rotation_video()
{
  Timer t;Fixture c(t);
  c.LoadText(".chipsets:\ninput red(1) [-10,0]\nfalse low [-10,4]\nscreen v(2x2) [0,0]\n.links:\nred:1 v:3\nred:1 v:4\nlow:1 v:1\nlow:1 v:2\nlow:1 v:5\nlow:1 v:6\nlow:1 v:7\nlow:1 v:8\n");
  step(c,t);auto time=t.GetTime();
  Screen s("fixture.rs");auto &video=c.At("v");
  const Position red_center[]={{3,1},{-1,3},{-3,-1},{1,-3}};
  for(auto center:red_center)
    {
      drawn_pixels.clear();bunny_test_draw=capture_pixels;video.Draw(s);bunny_test_draw=nullptr;
      auto key=std::make_pair(s.pic->buffer.width/2+int(center.x*s.pin_size),
                              s.pic->buffer.height/2+int(center.y*s.pin_size));
      CHECK(drawn_pixels.at(key)==Screen::Red && t.GetTime()==time);
      CHECK(video.IsUnder(s,{int(center.x),int(center.y)}));
      video.Rotate(1);
    }
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
      fs::create_directory("components");
      const std::string nested=".chipsets:\ninput a(1) [0,0]\noutput z [4,0]\n.tracks:\nw [b 0,0 b 4,0] {1-2}\n.links:\na:1 w:1@1\nz:1 w:2@2\n";
      put("components/rotation_contents.rs",nested);
      put("components/rotation_package.rs",nested+".package:\nline [0,-1] [4,-1]\nline [4,-1] [4,3]\nline [4,3] [0,3]\nline [0,3] [0,-1]\npin a [0,0]\npin z [4,2]\n");
      CHECK(setenv("BCAD_PATH",fs::absolute("components").c_str(),1)==0);
      gates();contacts();discrete();buttons_and_roundtrip();ready_pullup();interaction();routing_anchors();routing_gestures();
      rotation_geometry();rotation_editor();rotation_video();
      if(argc==3 && std::string(argv[1])=="--svg")svg(fs::absolute(argv[2]));
      CHECK(bunny_test_allocations==0);
      fs::current_path(original);fs::remove_all(directory);
      std::cout<<"PASS prototyping: "<<checks<<" checks (logic, passive networks, buttons, colours, anchors, routing, saves, editor)\n";
      return 0;
    }
  catch(const std::exception &e)
    {std::cerr<<e.what()<<"\nFixture directory: "<<directory<<'\n';return 1;}
}
