#include "Silicon.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>
#include <queue>
#include <sstream>
#include <stdexcept>

namespace silicon {
namespace {
struct Union {
  std::vector<int> p;
  int Add() { int n=int(p.size()); p.push_back(n); return n; }
  int Root(int n) { int r=n; while(p[r]!=r) r=p[r]; while(p[n]!=n) { int a=p[n]; p[n]=r; n=a; } return r; }
  void Join(int a,int b) { a=Root(a);b=Root(b);if(a!=b)p[b]=a; }
};
using Cells=std::set<PixelKey>;
bool Has(const Cells &s,Layer l,int x,int y) { return s.count({l,x,y})!=0; }
const int dx[]={1,-1,0,0},dy[]={0,0,1,-1};
std::vector<std::vector<Point>> Regions(const Cells &s,Layer l) {
  std::set<std::pair<int,int>> left;
  for(const PixelKey &p:s) if(p.layer==l) left.insert({p.x,p.y});
  std::vector<std::vector<Point>> result;
  while(!left.empty()) {
    std::vector<Point> region; std::queue<Point> q; auto start=*left.begin();left.erase(left.begin());q.push({start.first,start.second});
    while(!q.empty()) { Point p=q.front();q.pop();region.push_back(p); for(int k=0;k<4;++k) { auto i=left.find({p.x+dx[k],p.y+dy[k]}); if(i!=left.end()) { q.push({i->first,i->second});left.erase(i); } } }
    result.push_back(region);
  }
  return result;
}
}
int Netlist::NodeAt(Point p,Layer l) const { auto i=pixels.find({l,p.x,p.y}); return i==pixels.end()?-1:i->second; }
bool Netlist::Valid() const { for(const Diagnostic &d:diagnostics) if(d.error) return false; return true; }
Netlist ExtractFlat(const Document &doc) {
  doc.Validate(); Netlist out; Cells cells,channels;
  for(const Item &i:doc.items) if(i.kind==Kind::Shape && i.layer<Layer::Contact)
    for(int y=i.box.y;y<i.box.y+i.box.h;++y) for(int x=i.box.x;x<i.box.x+i.box.w;++x) cells.insert({i.layer,x,y});
  Cells physical=cells;
  // A MOS channel is not an ohmic short. Bipolar junctions likewise isolate
  // nested regions: an emitter contact reaches emitter, never base/collector.
  for(const PixelKey &p:physical) {
    if(p.layer==Layer::Active && Has(physical,Layer::Poly,p.x,p.y)) { cells.erase(p);channels.insert(p); }
    if((p.layer==Layer::CollectorN || p.layer==Layer::CollectorP) && Has(physical,Layer::Base,p.x,p.y)) cells.erase(p);
    if(p.layer==Layer::Base && Has(physical,Layer::Emitter,p.x,p.y)) cells.erase(p);
  }
  Union u;
  for(const PixelKey &p:cells) out.pixels[p]=u.Add();
  for(const auto &entry:out.pixels) { const PixelKey &p=entry.first; for(int k=0;k<2;++k) {
    auto next=out.pixels.find({p.layer,p.x+(k==0),p.y+(k==1)}); if(next!=out.pixels.end()) u.Join(entry.second,next->second);
  } }
  for(const Item &i:doc.items) if(i.kind==Kind::Shape && i.layer>=Layer::Contact) {
    bool good=true; int target=-1;
    for(int y=i.box.y;y<i.box.y+i.box.h;++y) for(int x=i.box.x;x<i.box.x+i.box.w;++x) {
      int metal=out.NodeAt({x,y},Layer::Metal1); int other=-1; unsigned count=0;
      if(i.layer==Layer::Via) { other=out.NodeAt({x,y},Layer::Metal2);count=other>=0; }
      else for(int l=0;l<8;++l) if(Layer(l)!=Layer::Metal1 && Layer(l)!=Layer::Metal2) {
        int n=out.NodeAt({x,y},Layer(l)); if(n>=0) { other=n;++count; }
      }
      if(metal<0 || count!=1) good=false;
      else { if(target>=0 && u.Root(target)!=u.Root(other)) good=false;target=other; }
    }
    if(!good) out.diagnostics.push_back({true,{i.box.x,i.box.y},"Contact: metal1 et une seule region requis (via: metal1 + metal2)",i.id});
    else for(int y=i.box.y;y<i.box.y+i.box.h;++y) for(int x=i.box.x;x<i.box.x+i.box.w;++x) u.Join(out.NodeAt({x,y},Layer::Metal1),target);
  }
  std::map<int,int> compact;
  for(auto &entry:out.pixels) { int r=u.Root(entry.second); if(!compact.count(r)) compact[r]=int(compact.size()); entry.second=compact[r]; }
  out.nodes=int(compact.size());
  // Islands with one base enclosure and one collector enclosure define BJTs.
  auto base_regions=Regions(physical,Layer::Base);
  std::map<std::pair<int,int>,std::pair<int,Device::Type>> base_owner;
  for(const auto &region:base_regions) {
    std::set<int> collectors; std::set<Layer> polarities; bool good=true;
    for(Point p:region) {
      bool n=Has(physical,Layer::CollectorN,p.x,p.y),pn=Has(physical,Layer::CollectorP,p.x,p.y);
      if(n==pn) good=false;
      Layer col=n?Layer::CollectorN:Layer::CollectorP; polarities.insert(col);
      for(int k=0;k<4;++k) {
        int x=p.x+dx[k],y=p.y+dy[k];
        if(!Has(physical,Layer::Base,x,y)) {
          int node=out.NodeAt({x,y},col); if(node<0) good=false; else collectors.insert(node);
        }
      }
    }
    if(!good || collectors.size()!=1 || polarities.size()!=1) out.diagnostics.push_back({true,region.front(),"Base: une enceinte collecteur, avec une marge d'un carre, est requise"});
    else for(Point p:region) base_owner[{p.x,p.y}]={*collectors.begin(),*polarities.begin()==Layer::CollectorN?Device::NPN:Device::PNP};
  }
  for(const auto &region:Regions(physical,Layer::Emitter)) {
    std::set<int> bases,emitters,collectors; std::set<Device::Type> types; bool good=true;
    for(Point p:region) {
      auto owner=base_owner.find({p.x,p.y});
      if(owner==base_owner.end()) good=false; else { collectors.insert(owner->second.first);types.insert(owner->second.second); }
      int n=out.NodeAt(p,Layer::Emitter);if(n>=0)emitters.insert(n);
      for(int k=0;k<4;++k) {
        int x=p.x+dx[k],y=p.y+dy[k];
        if(!Has(physical,Layer::Emitter,x,y)) { n=out.NodeAt({x,y},Layer::Base);if(n<0)good=false;else bases.insert(n); }
      }
    }
    if(!good || bases.size()!=1 || emitters.size()!=1 || collectors.size()!=1 || types.size()!=1) out.diagnostics.push_back({true,region.front(),"Emetteur: une base et un collecteur englobants sont requis"});
    else out.devices.push_back({*types.begin(),*collectors.begin(),*bases.begin(),*emitters.begin(),double(region.size()),region.front(),"Q"+std::to_string(out.devices.size()+1)});
  }
  for(const auto &region:Regions(channels,Layer::Active)) {
    std::set<int> terminals,gates;int minx=4096,miny=4096,maxx=-4096,maxy=-4096;
    for(Point p:region) { minx=std::min(minx,p.x);maxx=std::max(maxx,p.x);miny=std::min(miny,p.y);maxy=std::max(maxy,p.y);
      int gate=out.NodeAt(p,Layer::Poly); if(gate>=0)gates.insert(gate);
      for(int k=0;k<4;++k) { int n=out.NodeAt({p.x+dx[k],p.y+dy[k]},Layer::Active);if(n>=0)terminals.insert(n); }
    }
    bool lr=false,tb=false;
    for(int y=miny;y<=maxy;++y) lr=lr || (out.NodeAt({minx-1,y},Layer::Active)>=0 && out.NodeAt({maxx+1,y},Layer::Active)>=0);
    for(int x=minx;x<=maxx;++x) tb=tb || (out.NodeAt({x,miny-1},Layer::Active)>=0 && out.NodeAt({x,maxy+1},Layer::Active)>=0);
    bool rectangle=region.size()==size_t(maxx-minx+1)*(maxy-miny+1);
    if(terminals.size()!=2 || gates.size()!=1 || !rectangle || lr==tb) out.diagnostics.push_back({true,region.front(),"Canal NMOS: le poly doit traverser completement l'actif, avec deux regions distinctes"});
    else { auto t=terminals.begin();int a=*t++,b=*t;double ratio=lr?double(maxy-miny+1)/(maxx-minx+1):double(maxx-minx+1)/(maxy-miny+1);
      out.devices.push_back({Device::NMOS,a,*gates.begin(),b,ratio,region.front(),"M"+std::to_string(out.devices.size()+1)}); }
  }
  for(const PixelKey &p:physical) if(p.layer==Layer::CollectorN && Has(physical,Layer::CollectorP,p.x,p.y)) { out.diagnostics.push_back({true,{p.x,p.y},"Collecteurs N et P superposes"});break; }
  for(const Item &i:doc.items) if(i.kind!=Kind::Shape && i.kind!=Kind::Note) {
    Point p{i.box.x,i.box.y};int n=out.NodeAt(p,i.layer);
    if(n<0) { out.diagnostics.push_back({true,p,"Borne ou sonde sans conducteur sur la couche choisie",i.id});continue; }
    if((i.kind>=Kind::Input && i.kind<=Kind::Ground) || i.kind==Kind::Terminal) out.ports.push_back({i.id,n,i.name,i.kind,i.initial,i.pin});
    else if(i.kind==Kind::Probe)out.observations.push_back({i.id,n,i.name,i.kind,i.initial});
    else if(i.kind==Kind::Resistor || i.kind==Kind::Capacitor || i.kind==Kind::Current) {
      int b=out.NodeAt(i.end,i.layer);
      if(b<0) out.diagnostics.push_back({true,i.end,"Seconde borne sans conducteur",i.id});
      else if(n==b) out.diagnostics.push_back({true,p,"Deux bornes du meme element court-circuitees",i.id});
      else out.devices.push_back({i.kind==Kind::Resistor?Device::RESISTOR:i.kind==Kind::Capacitor?Device::CAPACITOR:Device::CURRENT,n,b,-1,i.value,p,i.name});
    }
  }
  if(out.nodes>MaxSimulationNodes) out.diagnostics.push_back({true,{0,0},"Budget du solveur depasse: "+std::to_string(MaxSimulationNodes)+" noeuds electriques"});
  if(out.devices.size()>4096) out.diagnostics.push_back({true,{0,0},"Budget depasse: 4096 dispositifs"});
  std::set<int> supplies,grounds;
  for(const Port &p:out.ports) {if(p.kind==Kind::Supply)supplies.insert(p.node);if(p.kind==Kind::Ground)grounds.insert(p.node);}
  for(int n:supplies) if(grounds.count(n)) out.diagnostics.push_back({true,{0,0},"Court-circuit direct entre alimentation et masse"});
  return out;
}
std::string Netlist::Spice(const std::string &title) const {
  if(!Valid()) throw std::runtime_error("Corriger les erreurs avant export");
  std::ostringstream s;s<<"* RedSilicon: "<<title<<"\n* Bornes dans l'ordre d'insertion, aucun rail implicite\n.subckt REDCHIP";
  for(const Port &p:ports)s<<" n"<<p.node;
  s<<"\n";int q=0,m=0,r=0,c=0,src=0;
  for(const Port &p:ports)s<<"* n"<<p.node<<" = "<<p.name<<" ("<<Name(p.kind)<<")\n";
  for(const Device &d:devices) {
    if(d.type==Device::NPN || d.type==Device::PNP) s<<"Q"<<++q<<" n"<<d.a<<" n"<<d.b<<" n"<<d.c<<' '<<(d.type==Device::NPN?"PEDN":"PEDP")<<" AREA="<<d.value;
    else if(d.type==Device::NMOS) s<<"M"<<++m<<" n"<<d.a<<" n"<<d.b<<" n"<<d.c<<" n"<<d.c<<" PEDMOS W="<<d.value<<"u L=1u";
    else s<<(d.type==Device::RESISTOR?"R":d.type==Device::CAPACITOR?"C":"I")<<(d.type==Device::RESISTOR?++r:d.type==Device::CAPACITOR?++c:++src)<<" n"<<d.a<<" n"<<d.b<<' '<<d.value;
    s<<'\n';
  }
  s<<".model PEDN NPN(IS=1e-15 BF=100 BR=1)\n.model PEDP PNP(IS=1e-15 BF=100 BR=1)\n.model PEDMOS NMOS(LEVEL=1 VTO=1 KP=0.001 LAMBDA=0.01)\n.ends REDCHIP\n";
  return s.str();
}
}
