#include "Editor.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
namespace {
using namespace silicon;
unsigned checks=0;
void Require(bool value,const char *message){++checks;if(!value)throw std::runtime_error(std::string("Hierarchy: ")+message);}
template<class Action> void Reject(Action action){bool rejected=false;try{action();}catch(const std::exception &){rejected=true;}Require(rejected,"invalid design rejected");}
void PortAt(Document &d,Kind kind,const std::string &name,Point p){Item i;i.kind=kind;i.name=name;i.box={p.x,p.y,1,1};d.Add(i);}
void Instance(Document &d,const std::string &name,const std::string &file,Point offset,int rotation=0){Item i;i.kind=Kind::Instance;i.name=name;i.source=file;i.box={offset.x,offset.y,1,1};i.rotation=rotation;d.Add(i);}
Document Cell(){Document d=Example("nmos-inverter");d.SetPackage("");for(Item &i:d.items){if(i.kind>=Kind::Input && i.kind<=Kind::Ground){i.kind=Kind::Terminal;i.pin=0;}}return d;}
void Interface(Document &d,const std::string &prefix,Transform t){PortAt(d,Kind::Input,prefix+"A",t.Apply(Point{5,-3}));PortAt(d,Kind::Supply,prefix+"VCC",t.Apply(Point{0,-7}));PortAt(d,Kind::Output,prefix+"Y",t.Apply(Point{0,1}));PortAt(d,Kind::Ground,prefix+"GND",t.Apply(Point{10,1}));}
}
void RunHierarchyTests(){
  using namespace silicon;namespace fs=std::filesystem;
  fs::path folder=fs::temp_directory_path()/("bcad-ic-hierarchy-"+std::to_string(getpid()));fs::create_directories(folder/"moved");
  try{
    Document cell=Cell();cell.Save((folder/"cell.rsi").string());
    Require(!cell.Packaged(),"a reusable cell has no package");
    for(const Item &i:cell.items)Require(!(i.kind>=Kind::Input && i.kind<=Kind::Ground),"internal terminals are not external contacts");
    Document bare;Item metal;metal.box={0,0,5,1};bare.Add(metal);bare.Save((folder/"bare.rsi").string());
    Document bare_parent;Instance(bare_parent,"a",(folder/"bare.rsi").string(),{});Instance(bare_parent,"b",(folder/"bare.rsi").string(),{20,0});
    auto empty_interface=Compile(bare_parent);Require(empty_interface->nets.Valid() && empty_interface->nets.ports.empty(),"cells without any contact or terminal can be instanced");Require(empty_interface->nets.nodes==2,"two geometry-only instances remain distinct");

    Document parent;Instance(parent,"left",(folder/"cell.rsi").string(),{});Instance(parent,"right",(folder/"cell.rsi").string(),{30,0});
    Interface(parent,"L",{});Interface(parent,"R",{{30,0},0});
    parent.Save((folder/"parent.rsi").string());parent=Document::Load((folder/"parent.rsi").string());
    auto design=Compile(parent);Require(design->nets.Valid(),"hierarchical geometry extracts");Require(design->instances.size()==3,"both instance paths recorded");Require(design->nets.devices.size()==4,"each inverter contributes a transistor and a load");
    Require(design->nets.ports.size()==8,"child terminals do not become package contacts");
    Simulator simulation(design->nets,"nmos");simulation.Drive("LA",State::Low);simulation.Drive("RA",State::High);Require(simulation.Step(),"hierarchical solve converges");
    Require(simulation.PortState("LY")==State::High && simulation.PortState("RY")==State::Low,"instances have independent voltages");
    auto left=design->View("left"),right=design->View("right");
    int left_y=left.NodeAt({0,1},Layer::Metal1),right_y=right.NodeAt({0,1},Layer::Metal1);
    Require(left_y!=right_y,"instance-local nodes map to different simulation nodes");Require(simulation.Get(left_y)==State::High && simulation.Get(right_y)==State::Low,"inspection maps to the actual instance");
    Require(design->Current(Designs()),"source snapshot is current");

    Document nested;Instance(nested,"module",(folder/"parent.rsi").string(),{100,40},1);auto deep=Compile(nested);
    Require(deep->instances.count("module/left") && deep->instances.count("module/right"),"hierarchy is recursive");Require(deep->nets.Valid(),"nested rotated cells extract");
    for(int turn=0;turn<4;++turn){Document rotated;Transform transform{{80,-60},turn};Instance(rotated,"gate",(folder/"cell.rsi").string(),transform.offset,turn);Interface(rotated,"",transform);auto model=Compile(rotated);Require(model->nets.Valid(),"all cell orientations extract");Simulator s(model->nets,"nmos");s.Drive("A",State::High);Require(s.Step() && s.PortState("Y")==State::Low,"rotation preserves transistor behavior");for(Point p:{Point{0,0},Point{3,-7},Point{-9,8}})Require(transform.Inverse(transform.Apply(p))==p,"cell transform round trip");}

    parent.SetPackage("DIP",14);Require(parent.Packaged() && parent.package.pins==14,"package is optional metadata");parent.SetPackage("DIP");Require(parent.package.pins==14,"unused package pin positions are retained");
    parent.Validate();std::string saved=parent.Serialize();Require(Document::Parse(saved).Serialize()==saved,"package and hierarchy round trip");
    Document duplicate=parent;duplicate.items.back().pin=duplicate.items[2].pin;Reject([&]{duplicate.Validate();});
    parent.Save((folder/"moved/parent.rsi").string());auto moved=Document::Load((folder/"moved/parent.rsi").string());Require(Compile(moved)->nets.Valid(),"save-as rebases cell references");Require(ReadFile((folder/"moved/parent.rsi").string()).find("../cell.rsi")!=std::string::npos,"saved references remain portable");

    DesignRepository repository;Document changed=cell;for(Item &i:changed.items)if(i.kind==Kind::Resistor)i.value*=2;repository.Publish((folder/"cell.rsi").string(),changed);
    Require(!design->Current(repository),"unsaved source changes invalidate compiled instances");auto overridden=Compile(Document::Load((folder/"parent.rsi").string()),repository);bool load_changed=false;for(const Device &d:overridden->nets.devices)load_changed=load_changed || (d.type==Device::RESISTOR && d.value==20000);Require(load_changed,"open document definition is used by both instances");

    Document recursive;recursive.origin=(folder/"self.rsi").string();Instance(recursive,"self","self.rsi",{});recursive.Save((folder/"self.rsi").string());Reject([&]{Compile(Document::Load((folder/"self.rsi").string()));});
    Document a,b;a.origin=(folder/"a.rsi").string();b.origin=(folder/"b.rsi").string();Instance(a,"b","b.rsi",{});Instance(b,"a","a.rsi",{});a.Save((folder/"a.rsi").string());b.Save((folder/"b.rsi").string());Reject([&]{Compile(Document::Load((folder/"a.rsi").string()));});
    Document missing;Instance(missing,"lost",(folder/"missing.rsi").string(),{});Reject([&]{Compile(missing);});
    Reject([&]{Document::Parse(saved.substr(0,saved.rfind("end ")));});

    Editor editor(Document::Load((folder/"parent.rsi").string()),(folder/"parent.rsi").string());editor.Step();
    Editor observer;observer.Bind(editor.session,"left");Require(observer.read_only && !observer.Dirty(),"observing an instance does not edit its definition");
    int y=observer.nets.NodeAt({0,1},Layer::Metal1);Require(observer.simulator==editor.simulator,"inspection shares the running simulation");
    editor.simulator->Drive("LA",State::High);editor.Step();Require(observer.simulator->Get(y)==State::Low,"live view follows its owning instance");
    auto snapshot=observer.document.Serialize();observer.Key("a",true);observer.Key("delete");Require(observer.document.Serialize()==snapshot,"live inspection is protected against accidental edits");
    editor.InsertInstance((folder/"bare.rsi").string(),{80,0});Require(editor.SelectedInstance()!=nullptr,"UI inserts a cell instance");editor.Key("d",true);Require(editor.design && editor.design->nets.Valid(),"UI duplicates references without duplicating names");editor.Key("z",true);Require(editor.design && editor.design->nets.Valid(),"undo restores hierarchical references");
    fs::remove_all(folder);std::cout<<checks<<" verifications de hierarchie et boitiers reussies\n";
  }catch(...){fs::remove_all(folder);throw;}
}
