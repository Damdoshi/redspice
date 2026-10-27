#include "Editor.hpp"
#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
using namespace silicon;
static int checks=0;
#define CHECK(x) do{++checks;if(!(x))throw std::runtime_error(std::string(__func__)+":"+std::to_string(__LINE__)+" " #x);}while(0)
template<class F> void Reject(F f){bool rejected=false;try{f();}catch(const std::exception &){rejected=true;}CHECK(rejected);}
static Item Shape(Layer l,int x,int y,int w=1,int h=1){Item i;i.layer=l;i.box={x,y,w,h};return i;}
static void Truth(){
  for(const auto &name:Examples()){
    Document d=Example(name);CHECK(Document::Parse(d.Serialize()).Serialize()==d.Serialize());auto n=Extract(d);
    if(!n.Valid())for(const auto &err:n.diagnostics)std::cerr<<name<<": "<<err.message<<'\n';CHECK(n.Valid());CHECK(n.devices.size()>=1);
    Simulator s(n,d.technology);CHECK(s.Step());
    if(name=="nmos-sample-hold"){s.Drive("D",State::High);s.Drive("CLK",State::High);for(int k=0;k<30;++k)CHECK(s.Step());CHECK(s.PortState("Q")==State::High);double held=0;for(const Port &p:n.ports)if(p.name=="Q")held=s.Voltage(p.node);s.Drive("CLK",State::Low);s.Drive("D",State::Low);CHECK(s.Step());CHECK(s.PortState("Q")==State::High);for(const Port &p:n.ports)if(p.name=="Q")CHECK(std::abs(s.Voltage(p.node)-held)<.01);s.Drive("CLK",State::High);for(int k=0;k<20;++k)CHECK(s.Step());CHECK(s.PortState("Q")==State::Low);continue;}
    bool two=name=="nmos-nand" || name=="nmos-nor";
    for(int repeat=0;repeat<3;++repeat)for(int a=0;a<2;++a)for(int b=0;b<(two?2:1);++b){s.Drive("A",a?State::High:State::Low);if(two)s.Drive("B",b?State::High:State::Low);CHECK(s.Step());bool want=name=="nmos-nand"?!(a&&b):name=="nmos-nor"?!(a||b):!a;if(s.PortState("Y")!=(want?State::High:State::Low))std::cerr<<name<<" A="<<a<<" B="<<b<<" -> "<<Name(s.PortState("Y"))<<'\n';CHECK(s.PortState("Y")== (want?State::High:State::Low));}
    s.Drive("A",State::Unknown);s.Step();CHECK(s.PortState("Y")==State::Unknown);
    Reject([&]{s.Drive("Y",State::High);});Reject([&]{s.Drive("absent",State::Low);});CHECK(s.Csv().find("Y_V")!=std::string::npos);CHECK(n.Spice(d.title).find(".subckt REDCHIP")!=std::string::npos);
  }
}
static void Geometry(){
  Document d;d.Add(Shape(Layer::Metal1,0,0,5,1));d.Add(Shape(Layer::Metal2,2,-2,1,5));auto n=Extract(d);CHECK(n.Valid());CHECK(n.nodes==2);
  d.Add(Shape(Layer::Via,2,0));n=Extract(d);CHECK(n.Valid());CHECK(n.nodes==1);
  d.Add(Shape(Layer::Contact,2,0));CHECK(!Extract(d).Valid());
  Document corner;corner.Add(Shape(Layer::Metal1,0,0));corner.Add(Shape(Layer::Metal1,1,1));CHECK(Extract(corner).nodes==2);
  corner.Add(Shape(Layer::Metal1,1,0));CHECK(Extract(corner).nodes==1);
  d=Example("nmos-inverter");for(Item &i:d.items)if(i.layer==Layer::Contact && i.kind==Kind::Shape)i.box.x+=4;CHECK(!Extract(d).Valid());
  d=Example("nmos-inverter");for(Item &i:d.items)if(i.layer==Layer::Poly && i.kind==Kind::Shape)i.box.h=4;CHECK(!Extract(d).Valid());
  d=Example("nmos-inverter");for(Item &i:d.items)if(i.kind==Kind::Input)i.box={100,100,1,1};CHECK(!Extract(d).Valid());
  d=Example("nmos-inverter");auto before=Extract(d);for(Item &i:d.items){i.box.x+=100;i.box.y-=50;i.end.x+=100;i.end.y-=50;}auto after=Extract(d);CHECK(before.nodes==after.nodes);CHECK(after.Valid());CHECK(before.devices.size()==after.devices.size());
}
static void Electrical(){
  Netlist n;n.nodes=3;n.devices.push_back({Device::RESISTOR,0,1,-1,1000,{},"R1"});n.devices.push_back({Device::RESISTOR,1,2,-1,1000,{},"R2"});n.ports={{1,0,"VCC",Kind::Supply},{2,2,"GND",Kind::Ground},{3,1,"MID",Kind::Output}};
  Simulator s(n);CHECK(s.Step());CHECK(std::abs(s.Voltage(1)-2.5)<1e-4);
  n.nodes=4;Simulator floating(n);CHECK(floating.Step());CHECK(floating.Get(3)==State::Floating);
  n.ports.push_back({4,0,"ZERO",Kind::Input,State::Low});Simulator conflict(n);CHECK(conflict.Step());CHECK(conflict.Get(0)==State::Broken);
  n.ports.pop_back();n.devices[1].type=Device::CAPACITOR;n.devices[1].value=1e-6;Simulator rc(n);CHECK(rc.Step());CHECK(rc.Voltage(1)>4.9);rc.Drive("VCC",State::Low);CHECK(rc.Step(1e-4));CHECK(rc.Voltage(1)<4.6 && rc.Voltage(1)>4.4);
  Reject([&]{rc.Step(0);});Reject([&]{rc.Step(NAN);});
  Netlist invalid;invalid.nodes=MaxSimulationNodes+1;Reject([&]{Simulator over(invalid);});
}
static void Files(){
  Document d=Example("nmos-inverter");std::string text=d.Serialize();
  for(size_t at=0;at<text.find("item");++at)Reject([&]{Document::Parse(text.substr(0,at));});
  Reject([&]{Document::Parse(text+"nonsense\n");});Reject([&]{Document::Parse(text+text);});
  Document bad=d;bad.items[0].box.w=-1;Reject([&]{bad.Validate();});bad=d;bad.items[0].value=NAN;Reject([&]{bad.Validate();});
  bad=d;bad.items.push_back(bad.items[0]);Reject([&]{bad.Validate();});
  bad=d;for(Item &i:bad.items)if(i.kind==Kind::Input)i.name="Y";Reject([&]{bad.Validate();});
  auto dir=std::filesystem::temp_directory_path()/("redsilicon-tests-"+std::to_string(getpid()));std::filesystem::create_directory(dir);
  auto file=(dir/"chip.rsi").string();d.Save(file);CHECK(Document::Load(file).Serialize()==text);
  std::filesystem::create_symlink(file,dir/"link");Reject([&]{d.Save((dir/"link").string());});Reject([&]{ReadFile(dir.string());});CHECK(ReadFile(file)==text);std::filesystem::remove_all(dir);
}
static void Interface(){
  Editor e(Example("nmos-inverter"));e.Key("a",true);size_t count=e.document.items.size();auto initial=e.document.Serialize();
  for(int turn=0;turn<4;++turn)e.Key("pagedown");CHECK(e.nets.Valid());CHECK(e.document.Serialize()==initial);
  e.Key("right");CHECK(e.document.Serialize()!=initial);e.Key("z",true);CHECK(e.document.Serialize()==initial);e.Key("y",true);CHECK(e.document.Serialize()!=initial);e.Key("z",true);
  e.Key("a",true);e.Key("d",true);CHECK(e.document.items.size()==count*2);e.Key("z",true);CHECK(e.document.items.size()==count);
  e.Key("a",true);e.Key("delete");CHECK(e.document.items.empty());e.Key("z",true);CHECK(e.document.items.size()==count);
  for(const Item &i:e.document.items)if(i.kind==Kind::Input)e.selected={i.id};e.Key("toggle");CHECK(e.simulator->PortState("Y")==State::Low);e.Key("toggle");CHECK(e.document.Find(*e.selected.begin())->initial==State::Floating);
  auto time=e.simulator->tick;e.Key("watch");e.Key("break");CHECK(e.simulator->tick==time);
  e.Key("name");e.Key("backspace");e.Type('X');e.Key("enter");CHECK(e.document.Find(*e.selected.begin())->name=="X");
  Image img;e.Render(img);CHECK(img.Ppm().size()>1000000);CHECK(img.pixels[0]!=img.pixels[img.width*300+400]);
  e.Key("help");e.Render(img);e.Key("escape");CHECK(e.prompt.empty());
  Editor drawing;drawing.tool=Tool::Rectangle;drawing.layer=Layer::CollectorN;Point a=drawing.Screen({0,0}),b=drawing.Screen({8,8});drawing.MouseDown(a.x+2,a.y+2,0);drawing.MouseMove(b.x+2,b.y+2);drawing.MouseUp(b.x+2,b.y+2,0);CHECK(drawing.document.items.size()==1);CHECK(drawing.document.items[0].box.w==9);
  drawing.Key("z",true);CHECK(drawing.document.items.empty());
  e.Key("title");for(int k=0;k<300;++k)e.Type('x');e.Key("enter");CHECK(e.document.title.size()<256);CHECK(e.document.Serialize().size()>100);
  e.Key("name");e.Key("backspace");e.Type(0xe9);e.Key("backspace");e.Type('Z');e.Key("enter");CHECK(e.document.Find(*e.selected.begin())->name=="Z");
}
void RunWindowTests();
void RunHierarchyTests();
int main(){try{Truth();Geometry();Electrical();Files();Interface();RunWindowTests();RunHierarchyTests();std::cout<<checks<<" verifications du mode CI reussies\n";return 0;}catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}}
