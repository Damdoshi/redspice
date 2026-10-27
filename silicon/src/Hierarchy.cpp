#include "Silicon.hpp"
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace silicon {
namespace {
Point RotateVector(Point p,int turns){switch((turns%4+4)%4){case 1:return {-p.y,p.x};case 2:return {-p.x,-p.y};case 3:return {p.y,-p.x};default:return p;}}
Point RotateCell(Point p,int turns){switch((turns%4+4)%4){case 1:return {-p.y-1,p.x};case 2:return {-p.x-1,-p.y-1};case 3:return {p.y,-p.x-1};default:return p;}}
bool Contact(Kind k){return k>=Kind::Input && k<=Kind::Ground;}
class Compiler {
  const DesignRepository &repository;
  std::map<std::string,Document> loaded;
  std::set<std::string> ancestry;
  std::shared_ptr<CompiledDesign> result;
  size_t instances=0;
  void Visit(const Document &document,const std::string &path,const std::string &parent,int item,Transform transform){
    document.Validate();
    if(++instances>4096 || ancestry.size()>256)throw std::runtime_error("Budget de hierarchie depasse");
    result->instances[path]={path,document.origin,parent,item,document,transform};
    if(!document.origin.empty())result->dependencies[document.origin]=document.ElectricalSnapshot();
    for(const Item &source:document.items){
      if(source.kind==Kind::Instance){
        std::string file=ResolveDesignPath(source.source,document.origin);
        if(!ancestry.insert(file).second)throw std::runtime_error("Reference recursive de cellule: "+file);
        if(!loaded.count(file))loaded[file]=repository.Read(file);
        std::string child=path.empty()?source.name:path+"/"+source.name;
        Visit(loaded.at(file),child,path,source.id,transform.Child({source.box.x,source.box.y},source.rotation));
        ancestry.erase(file);
        continue;
      }
      Item flat=source;
      if(!path.empty()){
        flat.id=result->flat.next_id++;
        flat.name=path+"/"+source.name;
        if(Contact(source.kind) || source.kind==Kind::Terminal)flat.kind=Kind::Probe;
        flat.pin=0;
      }
      flat.box=transform.Apply(source.box);
      flat.end=transform.Apply(source.end);
      result->item_ids[path][source.id]=flat.id;
      result->flat.items.push_back(flat);
      if(result->flat.items.size()>4096)throw std::runtime_error("Budget de geometrie instanciee depasse (4096 objets)");
    }
  }
public:
  explicit Compiler(const DesignRepository &r):repository(r),result(new CompiledDesign){}
  std::shared_ptr<CompiledDesign> Build(const Document &document){
    result->flat=document;result->flat.items.clear();
    if(!document.origin.empty())ancestry.insert(document.origin);
    Visit(document,"","",0,{});
    result->nets=ExtractFlat(result->flat);
    return result;
  }
};
}
Point Transform::Apply(Point p) const {p=RotateCell(p,rotation);return {p.x+offset.x,p.y+offset.y};}
Point Transform::Inverse(Point p) const {return RotateCell({p.x-offset.x,p.y-offset.y},-rotation);}
Rect Transform::Apply(Rect r) const {
  Point corners[]={Apply(Point{r.x,r.y}),Apply(Point{r.x+r.w-1,r.y}),Apply(Point{r.x,r.y+r.h-1}),Apply(Point{r.x+r.w-1,r.y+r.h-1})};
  int l=corners[0].x,t=corners[0].y,b=t,right=l;
  for(Point p:corners){l=std::min(l,p.x);right=std::max(right,p.x);t=std::min(t,p.y);b=std::max(b,p.y);}
  return {l,t,right-l+1,b-t+1};
}
Transform Transform::Child(Point delta,int turns) const {delta=RotateVector(delta,rotation);return {{offset.x+delta.x,offset.y+delta.y},(rotation+turns)%4};}
std::string ResolveDesignPath(const std::string &name,const std::string &parent){
  if(name.empty())throw std::runtime_error("Chemin de cellule vide");
  namespace fs=std::filesystem;fs::path p(name);
  if(p.is_relative() && !parent.empty())p=fs::path(parent).parent_path()/p;
  std::error_code ec;auto full=fs::weakly_canonical(fs::absolute(p),ec);
  return (ec?fs::absolute(p).lexically_normal():full).string();
}
Document DesignRepository::Read(const std::string &path) const {std::string file=ResolveDesignPath(path);auto it=opened.find(file);return it==opened.end()?Document::Load(file):it->second;}
void DesignRepository::Publish(const std::string &path,const Document &document){
  if(path.empty())return;Document copy=document;copy.Validate();
  for(Item &i:copy.items)if(i.kind==Kind::Instance)i.source=ResolveDesignPath(i.source,document.origin);
  for(auto &entry:copy.favorites.slots)if(entry.kind=="cell")entry.source=ResolveDesignPath(entry.source,document.origin);
  copy.origin=ResolveDesignPath(path);opened[copy.origin]=copy;
}
void DesignRepository::Forget(const std::string &path){if(!path.empty())opened.erase(ResolveDesignPath(path));}
DesignRepository &Designs(){static DesignRepository repository;return repository;}
std::shared_ptr<CompiledDesign> Compile(const Document &document,const DesignRepository &repository){return Compiler(repository).Build(document);}
Netlist Extract(const Document &document){return Compile(document)->nets;}
bool CompiledDesign::Current(const DesignRepository &repository) const {
  for(const auto &entry:dependencies)if(repository.Read(entry.first).ElectricalSnapshot()!=entry.second)return false;
  return true;
}
Netlist CompiledDesign::View(const std::string &path,const std::vector<int> &node_map) const {
  auto found=instances.find(path);if(found==instances.end())throw std::runtime_error("Instance inconnue: "+path);
  DesignRepository snapshot;
  for(const auto &entry:instances)if(!entry.second.source.empty())snapshot.Publish(entry.second.source,entry.second.document);
  auto local=Compile(found->second.document,snapshot);
  Netlist view=local->nets;
  std::vector<int> remap(view.nodes,-1);
  for(auto &entry:view.pixels){Point p=found->second.transform.Apply(Point{entry.first.x,entry.first.y});int n=nets.NodeAt(p,entry.first.layer);
    if(n>=0 && !node_map.empty())n=node_map.at(n);
    if(n>=0)remap[entry.second]=n;
    entry.second=n;
  }
  for(Device &d:view.devices){if(d.a>=0)d.a=remap.at(d.a);if(d.b>=0)d.b=remap.at(d.b);if(d.c>=0)d.c=remap.at(d.c);}
  for(Port &p:view.ports)p.node=remap.at(p.node);
  for(Port &p:view.observations)p.node=remap.at(p.node);
  view.nodes=node_map.empty()?nets.nodes:(node_map.empty()?0:*std::max_element(node_map.begin(),node_map.end())+1);
  return view;
}
}
