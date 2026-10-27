#ifndef REDSILICON_HPP
#define REDSILICON_HPP
#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "ComponentDock.hpp"

namespace silicon {
constexpr int MaxSimulationNodes=1024;
enum class Layer { Active, Poly, Metal1, Metal2, CollectorN, CollectorP, Base, Emitter, Contact, Via };
enum class Kind { Shape, Input, Output, InOut, Supply, Ground, Probe, Note, Resistor, Capacitor, Current, Terminal, Instance };
enum class State { Low, High, Unknown, Floating, Broken };
struct Point { int x=0, y=0; };
bool operator==(Point a, Point b);
struct Rect {
  int x=0,y=0,w=1,h=1;
  bool Contains(Point p) const;
};
struct Item {
  int id=0;
  Kind kind=Kind::Shape;
  Layer layer=Layer::Metal1;
  Rect box;
  Point end;
  std::string name, group;
  double value=10000;
  State initial=State::Low;
  bool watch=false, breakpoint=false;
  std::string source;
  int rotation=0;
  int pin=0;
};
struct Package {
  std::string name;
  int pins=0;
  int width=8;
  bool Enabled() const { return !name.empty(); }
};
struct Document {
  std::string title="Nouvelle puce", technology="nmos";
  std::vector<Item> items;
  int next_id=1;
  Package package;
  bcad::ComponentDock favorites;
  std::string origin;
  int Add(Item item);
  Item *Find(int id);
  const Item *Find(int id) const;
  void Validate() const;
  std::string Serialize() const;
  std::string Snapshot() const;
  std::string ElectricalSnapshot() const;
  static Document Parse(const std::string &text);
  static Document Load(const std::string &path);
  void Save(const std::string &path) const;
  void SetPackage(const std::string &name,int pins=0);
  bool Packaged() const { return package.Enabled(); }
};
const char *Name(Layer layer);
const char *Name(Kind kind);
const char *Name(State state);
Layer ParseLayer(const std::string &text);
Kind ParseKind(const std::string &text);
State ParseState(const std::string &text);
struct Diagnostic { bool error=false; Point at; std::string message; int item=0; };
struct Device {
  enum Type { NPN, PNP, NMOS, RESISTOR, CAPACITOR, CURRENT } type=NPN;
  int a=-1,b=-1,c=-1; // BJT: collector/base/emitter; MOS: drain/gate/source
  double value=1;
  Point at;
  std::string name;
};
struct Port { int item=0,node=-1; std::string name; Kind kind=Kind::Input; State initial=State::Low; int number=0; double conductance=1; };
struct PixelKey {
  Layer layer; int x,y;
  bool operator<(const PixelKey &other) const;
};
struct Netlist {
  std::map<PixelKey,int> pixels;
  std::vector<Device> devices;
  std::vector<Port> ports;
  std::vector<Port> observations;
  std::vector<Diagnostic> diagnostics;
  int nodes=0;
  int NodeAt(Point p,Layer layer) const;
  bool Valid() const;
  std::string Spice(const std::string &title) const;
};
Netlist Extract(const Document &document);
Netlist ExtractFlat(const Document &document);
struct Transform {
  Point offset;
  int rotation=0;
  Point Apply(Point point) const;
  Point Inverse(Point point) const;
  Rect Apply(Rect rectangle) const;
  Transform Child(Point offset,int rotation) const;
};
std::string ResolveDesignPath(const std::string &path,const std::string &parent="");
class DesignRepository {
  std::map<std::string,Document> opened;
public:
  Document Read(const std::string &path) const;
  void Publish(const std::string &path,const Document &document);
  void Forget(const std::string &path);
};
DesignRepository &Designs();
struct InstanceInfo {
  std::string path,source,parent;
  int item=0;
  Document document;
  Transform transform;
};
struct CompiledDesign {
  Document flat;
  Netlist nets;
  std::map<std::string,InstanceInfo> instances;
  std::map<std::string,std::map<int,int>> item_ids;
  std::map<std::string,std::string> dependencies;
  bool Current(const DesignRepository &repository) const;
  Netlist View(const std::string &path,const std::vector<int> &node_map={}) const;
};
std::shared_ptr<CompiledDesign> Compile(const Document &document,const DesignRepository &repository=Designs());
struct Sample { uint64_t tick=0; std::vector<State> states; std::vector<double> volts; };
class Simulator {
  Netlist netlist;
  std::map<int,State> drives;
  std::vector<double> voltage;
  std::vector<State> states;
  std::vector<double> previous;
  bool defaults=true;
  double low=.8, high=2.;
  bool Solve(bool transient,double seconds);
public:
  uint64_t tick=0;
  bool converged=false;
  std::string status;
  std::vector<Sample> history;
  explicit Simulator(const Netlist &nets=Netlist(),const std::string &technology="bipolar",bool default_power=true);
  void Drive(const std::string &name,State state);
  void DriveItem(int id,State state);
  void Reset();
  bool Step(double seconds=1e-6);
  void ClearDrives();
  State Get(int node) const;
  double Voltage(int node) const;
  State PortState(const std::string &name) const;
  const Netlist &Nets() const { return netlist; }
  std::string Csv() const;
};
struct SimulationSession {
  std::shared_ptr<CompiledDesign> design;
  std::shared_ptr<Simulator> simulator;
  std::vector<int> node_map;
  uint64_t generation=0;
};
Document Example(const std::string &name);
std::vector<std::string> Examples();
void WriteFile(const std::string &path,const std::string &content);
std::string ReadFile(const std::string &path);
}
#endif
