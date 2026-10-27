#include "SiliconChip.hpp"
#include <algorithm>
#include <functional>
#include <numeric>
#include <set>
#include <sstream>
#include <stdexcept>

namespace {
silicon::State FromBcad(hbs::Tristate s){switch(s){case hbs::FALSE:return silicon::State::Low;case hbs::TRUE:return silicon::State::High;case hbs::BROKEN:return silicon::State::Broken;case hbs::HIGH_IMPEDANCE:return silicon::State::Floating;default:return silicon::State::Unknown;}}
hbs::Tristate ToBcad(silicon::State s){switch(s){case silicon::State::Low:return hbs::FALSE;case silicon::State::High:return hbs::TRUE;case silicon::State::Broken:return hbs::BROKEN;case silicon::State::Floating:return hbs::HIGH_IMPEDANCE;default:return hbs::UNDEFINED;}}
struct BoardTerminal {
  hbs::IComponent *component=nullptr;size_t pin=0;
  bool operator<(const BoardTerminal &o) const {if(component!=o.component)return std::less<hbs::IComponent*>()(component,o.component);return pin<o.pin;}
};
struct BoardPath {
  BoardTerminal terminal;bool weak=false;unsigned levels=3;
  bool operator<(const BoardPath &o) const {if(terminal<o.terminal)return true;if(o.terminal<terminal)return false;if(weak!=o.weak)return weak<o.weak;return levels<o.levels;}
};
struct MergeNodes {
  std::vector<int> parent;
  explicit MergeNodes(int n):parent(n){std::iota(parent.begin(),parent.end(),0);}
  int Root(int n){int r=n;while(parent[r]!=r)r=parent[r];while(parent[n]!=n){int old=parent[n];parent[n]=r;n=old;}return r;}
  void Join(int a,int b){a=Root(a);b=Root(b);if(a!=b)parent[b]=a;}
};
}

namespace hbs {
// All user-designed ICs driven by one board timer share the same electrical
// solve. Connections between their pads remain electrical connections, even
// for bidirectional pins or feedback between two separate packages.
class SiliconNetwork {
  struct Driver {BoardTerminal terminal;int node;bool weak;unsigned levels;bool passive;};
  const Timer &timer;
  std::vector<SiliconChip*> chips;
  std::shared_ptr<silicon::Simulator> simulator;
  size_t evaluated=std::numeric_limits<size_t>::max();
  bool evaluating=false;
  std::string signature;

  void BuildAndDrive(){
    std::map<SiliconChip*,int> offsets;int count=0;
    for(SiliconChip *chip:chips){offsets[chip]=count;count+=chip->session->design->nets.nodes;}
    if(count>silicon::MaxSimulationNodes)throw std::runtime_error("Budget du reseau des circuits integres depasse");
    MergeNodes merged(count);std::vector<Driver> drivers;std::set<std::pair<int,int>> weak_links;
    for(SiliconChip *chip:chips)for(size_t pin=1;pin<=chip->GetPinCount();++pin){
      int local=chip->package_nodes[pin-1];if(local<0)continue;int origin=offsets.at(chip)+local;
      std::set<BoardPath> visited;std::vector<BoardPath> pending{{{chip,pin},false,3}};
      while(!pending.empty()){
        BoardPath path=pending.back();pending.pop_back();if(!path.levels || !visited.insert(path).second)continue;
        if(visited.size()>200000)throw std::runtime_error("Budget de connexions du circuit depasse");
        IComponent &part=*path.terminal.component;size_t n=path.terminal.pin;
        std::vector<SignalConnection> next;part.GetConnections(n,next);
        if(auto *other=dynamic_cast<SiliconChip*>(&part)){
          auto found=offsets.find(other);
          if(found!=offsets.end() && n>=1 && n<=other->package_nodes.size() && other->package_nodes[n-1]>=0){
            int target=found->second+other->package_nodes[n-1];
            if(!path.weak && path.levels==3)merged.Join(origin,target);
            else if(path.levels==3 && target!=origin)weak_links.insert(std::minmax(origin,target));
          }
        }
        else if(part.IsPassivePin(n)){
          Tristate value=part.GetPassiveConnections(n,next);
          if(value!=HIGH_IMPEDANCE)drivers.push_back({path.terminal,origin,path.weak,path.levels,true});
        }
        else if(part.DrivesPin(n))drivers.push_back({path.terminal,origin,path.weak,path.levels,false});
        for(const SignalConnection &connection:next)if(connection.component)
          pending.push_back({{connection.component,connection.pin},path.weak || connection.weak,path.levels & connection.levels});
      }
    }
    std::vector<int> mapping(count);std::map<int,int> compact;
    for(int n=0;n<count;++n){int root=merged.Root(n);if(!compact.count(root))compact[root]=int(compact.size());mapping[n]=compact.at(root);}
    silicon::Netlist nets;nets.nodes=int(compact.size());std::ostringstream topology;
    for(int n:mapping)topology<<n<<',';
    for(SiliconChip *chip:chips)topology<<chip<<':'<<offsets.at(chip)<<';';
    for(SiliconChip *chip:chips){
      int offset=offsets.at(chip);const auto &source=chip->session->design->nets;
      for(silicon::Device device:source.devices){if(device.a>=0)device.a=mapping.at(offset+device.a);if(device.b>=0)device.b=mapping.at(offset+device.b);if(device.c>=0)device.c=mapping.at(offset+device.c);device.name=chip->name+"/"+device.name;nets.devices.push_back(device);}
      for(silicon::Port port:source.ports){port.node=mapping.at(offset+port.node);port.name=chip->name+"/"+port.name;nets.observations.push_back(port);}
      for(silicon::Port port:source.observations){port.node=mapping.at(offset+port.node);port.name=chip->name+"/"+port.name;nets.observations.push_back(port);}
    }
    for(auto edge:weak_links){int a=mapping.at(edge.first),b=mapping.at(edge.second);if(a!=b){nets.devices.push_back({silicon::Device::RESISTOR,a,b,-1,10000,{},"board-pull"});topology<<'R'<<a<<':'<<b<<';';}}
    // A driver may be reachable from several pads of the same physical net.
    // Keep it once so the number of observation pins does not change its load.
    std::set<std::tuple<IComponent*,size_t,int,bool,unsigned,bool>> seen;
    std::vector<Driver> unique;
    for(Driver driver:drivers){driver.node=mapping.at(driver.node);auto key=std::make_tuple(driver.terminal.component,driver.terminal.pin,driver.node,driver.weak,driver.levels,driver.passive);if(seen.insert(key).second)unique.push_back(driver);}
    int id=1;
    for(const Driver &driver:unique){silicon::Port p{id++,driver.node,"board-driver",silicon::Kind::Input,silicon::State::Floating,0,driver.weak?.0001:1};nets.ports.push_back(p);topology<<driver.terminal.component<<':'<<driver.terminal.pin<<':'<<driver.node<<':'<<driver.weak<<':'<<driver.levels<<';';}
    std::string current=topology.str();bool rebuilt=!simulator || signature!=current;
    if(rebuilt){simulator.reset(new silicon::Simulator(nets,"nmos",false));signature=current;}
    simulator->ClearDrives();id=1;
    for(const Driver &driver:unique){
      Tristate state;
      if(driver.passive){std::vector<SignalConnection> ignored;state=driver.terminal.component->GetPassiveConnections(driver.terminal.pin,ignored);}
      else state=driver.terminal.component->Compute(driver.terminal.pin);
      if((state==FALSE && !(driver.levels&1)) || (state==TRUE && !(driver.levels&2)))state=HIGH_IMPEDANCE;
      simulator->DriveItem(id++,FromBcad(state));
    }
    for(SiliconChip *chip:chips){
      auto &session=*chip->session;int offset=offsets.at(chip);
      if(rebuilt || session.simulator!=simulator){session.node_map.assign(mapping.begin()+offset,mapping.begin()+offset+session.design->nets.nodes);session.simulator=simulator;++session.generation;}
    }
  }
public:
  explicit SiliconNetwork(const Timer &t):timer(t){}
  static std::shared_ptr<SiliconNetwork> For(const Timer &timer){
    static std::map<const Timer*,std::weak_ptr<SiliconNetwork>> networks;
    auto shared=networks[&timer].lock();if(!shared){shared.reset(new SiliconNetwork(timer));networks[&timer]=shared;}
    return shared;
  }
  void Add(SiliconChip *chip){chips.push_back(chip);Invalidate();}
  void Remove(SiliconChip *chip){chips.erase(std::remove(chips.begin(),chips.end(),chip),chips.end());Invalidate();}
  void Invalidate(bool definition=false){if(definition)signature.clear();evaluated=std::numeric_limits<size_t>::max();}
  void Reset(){if(simulator)simulator->Reset();evaluated=std::numeric_limits<size_t>::max();}
  void Evaluate(){
    if(evaluating || evaluated==timer.GetTime())return;
    evaluating=true;
    try{BuildAndDrive();simulator->Step();evaluated=timer.GetTime();evaluating=false;}
    catch(...){evaluating=false;throw;}
  }
};
}

void hbs::SiliconChip::ReadPackage(){
  if(!layout.Packaged() || layout.package.pins<1)throw std::runtime_error("Ce circuit integre n'a pas de boitier. Il peut etre utilise comme cellule dans un CI.");
  package_nodes.assign(layout.package.pins,-1);package_names.assign(layout.package.pins,"NC");
  if(!session->design->nets.Valid())throw std::runtime_error("Extraction du CI invalide: "+session->design->nets.diagnostics.front().message);
  for(const auto &port:session->design->nets.ports)if(port.kind>=silicon::Kind::Input && port.kind<=silicon::Kind::Ground){
    if(port.number<1 || port.number>layout.package.pins)throw std::runtime_error("Contact externe sans numero de broche de boitier");
    package_nodes[port.number-1]=port.node;package_names[port.number-1]=port.name;
  }
}
hbs::SiliconChip::SiliconChip(const Timer &t,const std::string &n,const std::string &file,const std::string &pos)
  :AComponent<64>(t,n,pos),source_path(silicon::ResolveDesignPath(file)),layout(silicon::Designs().Read(source_path)),session(new silicon::SimulationSession){
  session->design=silicon::Compile(layout);ReadPackage();network=SiliconNetwork::For(timer);network->Add(this);
}
hbs::SiliconChip::~SiliconChip(){network->Remove(this);}
const std::string &hbs::SiliconChip::GetType() const {static const std::string type="silicon";return type;}
std::string hbs::SiliconChip::GetDisplayType() const {return "Circuit integre / "+layout.title;}
bool hbs::SiliconChip::DrivesPin(size_t pin) const {return pin>=1 && pin<=package_nodes.size() && package_nodes[pin-1]>=0;}
hbs::Tristate hbs::SiliconChip::Compute(size_t pin){if(pin<1 || pin>GetPinCount())throw BadPin("silicon: broche invalide");network->Evaluate();return GetPinDisplayState(pin);}
hbs::Tristate hbs::SiliconChip::GetPinDisplayState(size_t pin) const {
  if(pin<1 || pin>package_nodes.size() || package_nodes[pin-1]<0)return HIGH_IMPEDANCE;
  int n=package_nodes[pin-1];if(!session->simulator || n>=int(session->node_map.size()))return UNDEFINED;
  return ToBcad(session->simulator->Get(session->node_map[n]));
}
void hbs::SiliconChip::SetLink(size_t pin,IComponent &part,size_t other,const std::string &pos){if(pin<1 || pin>GetPinCount())throw BadPin("silicon: broche invalide");AComponent<64>::SetLink(pin,part,other,pos);network->Invalidate();}
void hbs::SiliconChip::DisconnectFrom(const IComponent *part){AComponent<64>::DisconnectFrom(part);network->Invalidate();}
bool hbs::SiliconChip::Reload(){
  if(session->design->Current(silicon::Designs()))return false;
  auto next=silicon::Designs().Read(source_path);auto compiled=silicon::Compile(next);
  for(const auto &entry:links)if(entry.first>size_t(next.package.pins) && !entry.second.empty())throw std::runtime_error("La nouvelle definition retire une broche encore cablee");
  auto old=layout;auto old_design=session->design;
  layout=next;session->design=compiled;
  try{ReadPackage();}catch(...){layout=old;session->design=old_design;ReadPackage();throw;}
  network->Invalidate(true);session->node_map.clear();session->simulator.reset();++session->generation;return true;
}
void hbs::SiliconChip::ResetSimulation(){network->Reset();}
hbs::Position hbs::SiliconChip::GetPinPosition(size_t pin) const {if(pin<1 || pin>GetPinCount())throw BadPin("silicon: broche invalide");size_t rows=(GetPinCount()+1)/2;return pin<=rows?LocalToWorld({0,double(pin-1)}):LocalToWorld({double(layout.package.width),double(GetPinCount()-pin)});}
bool hbs::SiliconChip::IsUnder(const Screen &,const t_bunny_position &p) const {auto local=WorldToLocal({double(p.x),double(p.y)});return local.x>=0 && local.x<=layout.package.width && local.y>=-1 && local.y<=double((GetPinCount()+1)/2);}
void hbs::SiliconChip::Draw(Screen &screen) const {
  double rows=(GetPinCount()+1)/2.;double w=layout.package.width;
  screen.Square(LocalToWorld({.5,-.5}),RotatedSize({w-1,rows}),Screen::Teal);
  for(size_t p=1;p<=GetPinCount();++p){auto point=GetPinPosition(p);auto state=GetPinDisplayState(p);screen.Circle(point,{.4,.4},Screen::TristateColor[state-BROKEN],true);screen.Circle(point,{.4,.4},Screen::White,false);double x=p<=(GetPinCount()+1)/2?1.5:w-1.5;double y=p<=(GetPinCount()+1)/2?double(p-1):double(GetPinCount()-p);DrawLabel(screen,{x,y-.3},{5,7},Screen::White,package_names[p-1]);}
  DrawLabel(screen,{w/2,-1.5},{9,10},Screen::White,name);DrawLabel(screen,{w/2,rows/2},{6,7},Screen::Teal,layout.package.name);
}
