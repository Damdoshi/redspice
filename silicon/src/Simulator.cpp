#include "Silicon.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace silicon {
namespace {
bool Linear(std::vector<double> a,std::vector<double> rhs,std::vector<double> &answer,int n) {
  for(int k=0;k<n;++k) {
    int pivot=k;for(int r=k+1;r<n;++r)if(std::abs(a[r*n+k])>std::abs(a[pivot*n+k]))pivot=r;
    if(std::abs(a[pivot*n+k])<1e-20)return false;
    if(pivot!=k) {for(int c=k;c<n;++c)std::swap(a[k*n+c],a[pivot*n+c]);std::swap(rhs[k],rhs[pivot]);}
    for(int r=k+1;r<n;++r) {double f=a[r*n+k]/a[k*n+k];if(std::abs(f)<1e-30)continue;for(int c=k+1;c<n;++c)a[r*n+c]-=f*a[k*n+c];rhs[r]-=f*rhs[k];}
  }
  answer.assign(n,0);
  for(int r=n-1;r>=0;--r){double v=rhs[r];for(int c=r+1;c<n;++c)v-=a[r*n+c]*answer[c];answer[r]=v/a[r*n+r];if(!std::isfinite(answer[r]))return false;}
  return true;
}
struct Stamp {
  int n;
  const std::vector<double> &v;
  std::vector<double> j,f;
  Stamp(int size,const std::vector<double> &values):n(size),v(values),j(size*size,0),f(size,0) {}
  void Conductance(int a,int b,double g) {
    j[a*n+a]+=g;j[b*n+b]+=g;j[a*n+b]-=g;j[b*n+a]-=g;
    double i=(v[a]-v[b])*g;f[a]+=i;f[b]-=i;
  }
  void Current(int a,int b,double i){f[a]+=i;f[b]-=i;}
};
void Bipolar(Stamp &s,const Device &d) {
  const double vt=.02585,alphaF=100./101.,alphaR=.5;
  double sign=d.type==Device::PNP?-1:1;
  double vbe=sign*(s.v[d.b]-s.v[d.c]),vbc=sign*(s.v[d.b]-s.v[d.a]);
  double e=std::exp(std::clamp(vbe/vt,-40.,32.)),c=std::exp(std::clamp(vbc/vt,-40.,32.));
  double is=1e-15*d.value,ibe=is*(e-1),ibc=is*(c-1),gbe=is*e/vt,gbc=is*c/vt;
  double ic=sign*(alphaF*ibe-ibc),ie=sign*(-ibe+alphaR*ibc);
  s.f[d.a]+=ic;s.f[d.c]+=ie;s.f[d.b]-=ic+ie;
  int pins[]={d.a,d.b,d.c};
  double jc[]={gbc,alphaF*gbe-gbc,-alphaF*gbe};
  double je[]={-alphaR*gbc,-gbe+alphaR*gbc,gbe};
  for(int k=0;k<3;++k){s.j[d.a*s.n+pins[k]]+=jc[k];s.j[d.c*s.n+pins[k]]+=je[k];s.j[d.b*s.n+pins[k]]-=jc[k]+je[k];}
}
double MosCurrent(double vd,double vg,double vs,double ratio) {
  double sign=1;if(vd<vs){std::swap(vd,vs);sign=-1;}
  double over=vg-vs-1,ds=vd-vs;
  if(over<=0)return sign*ds*1e-12;
  double beta=.001*ratio;
  double current=ds<over?beta*(over*ds-.5*ds*ds):.5*beta*over*over;
  return sign*(current*(1+.01*ds)+ds*1e-12);
}
void Mos(Stamp &s,const Device &d) {
  double v[]={s.v[d.a],s.v[d.b],s.v[d.c]};
  double current=MosCurrent(v[0],v[1],v[2],d.value);
  s.Current(d.a,d.c,current);
  int pins[]={d.a,d.b,d.c};
  for(int k=0;k<3;++k){double old=v[k];v[k]+=1e-5;double derivative=(MosCurrent(v[0],v[1],v[2],d.value)-current)/1e-5;v[k]=old;s.j[d.a*s.n+pins[k]]+=derivative;s.j[d.c*s.n+pins[k]]-=derivative;}
}
}
Simulator::Simulator(const Netlist &nets,const std::string &technology,bool default_power):netlist(nets),defaults(default_power) {
  if(nets.nodes>MaxSimulationNodes || nets.nodes<0)throw std::runtime_error("Budget du solveur depasse");
  if(technology=="nmos"){low=1.5;high=3.5;}
  Reset();
}
void Simulator::Reset() {
  tick=0;history.clear();voltage.assign(netlist.nodes,0);previous=voltage;states.assign(netlist.nodes,State::Unknown);drives.clear();converged=false;status="Pret";
  for(const Port &p:netlist.ports) {
    if(p.kind==Kind::Input || p.kind==Kind::InOut || p.kind==Kind::Terminal)drives[p.item]=defaults && p.kind!=Kind::Terminal?p.initial:State::Floating;
    if(p.kind==Kind::Supply)drives[p.item]=defaults?State::High:State::Floating;
    if(p.kind==Kind::Ground)drives[p.item]=defaults?State::Low:State::Floating;
  }
}
void Simulator::ClearDrives(){drives.clear();}
void Simulator::Drive(const std::string &name,State value) {
  for(const Port &p:netlist.ports)if(p.name==name){DriveItem(p.item,value);return;}
  throw std::runtime_error("Broche inconnue: "+name);
}
void Simulator::DriveItem(int id,State value) {
  for(const Port &p:netlist.ports)if(p.item==id){if(p.kind==Kind::Output)throw std::runtime_error("Une sortie ne peut pas etre forcee");drives[id]=value;return;}
  throw std::runtime_error("Broche inconnue");
}
bool Simulator::Solve(bool transient,double seconds) {
  int n=netlist.nodes;
  if(!netlist.Valid()){status="Extraction invalide";states.assign(n,State::Broken);return false;}
  if(n==0){status="Aucun noeud";return true;}
  std::vector<int> anchors(n,0);std::vector<bool> uncertain(n,false),broken(n,false),reachable(n,false);
  std::vector<double> strength(n,0);
  for(const Port &p:netlist.ports){auto i=drives.find(p.item);if(i!=drives.end() && i->second!=State::Floating)strength[p.node]=std::max(strength[p.node],p.conductance);}
  for(const Port &p:netlist.ports){auto i=drives.find(p.item);if(i==drives.end())continue;
    if(p.conductance<strength[p.node])continue;
    if(i->second==State::Low)anchors[p.node]|=1;
    if(i->second==State::High)anchors[p.node]|=2;
    if(i->second==State::Unknown)uncertain[p.node]=true;
    if(i->second==State::Broken)broken[p.node]=true;
  }
  for(int k=0;k<n;++k)if(anchors[k]==3)broken[k]=true;
  bool settled=false;
  for(int iteration=0;iteration<100;++iteration){
    Stamp s(n,voltage);
    for(int k=0;k<n;++k){s.j[k*n+k]+=1e-12;s.f[k]+=voltage[k]*1e-12;}
    for(const Port &p:netlist.ports){auto i=drives.find(p.item);if(i==drives.end())continue;
      if(i->second==State::Low || i->second==State::High){double target=i->second==State::High?5:0;s.j[p.node*n+p.node]+=p.conductance;s.f[p.node]+=(voltage[p.node]-target)*p.conductance;}
    }
    for(const Device &d:netlist.devices) {
      if(d.type==Device::NPN || d.type==Device::PNP)Bipolar(s,d);
      else if(d.type==Device::NMOS)Mos(s,d);
      else if(d.type==Device::RESISTOR)s.Conductance(d.a,d.b,1/d.value);
      else if(d.type==Device::CURRENT)s.Current(d.a,d.b,d.value);
      else if(transient){double g=d.value/seconds;s.Conductance(d.a,d.b,g);s.Current(d.a,d.b,-g*(previous[d.a]-previous[d.b]));}
    }
    double residual=0;for(double f:s.f)residual=std::max(residual,std::abs(f));
    std::vector<double> rhs=s.f,delta;for(double &v:rhs)v=-v;
    if(!Linear(s.j,rhs,delta,n))break;
    double largest=0;for(double v:delta)largest=std::max(largest,std::abs(v));
    double factor=largest>1?1/largest:1;
    for(const Device &d:netlist.devices)if(d.type==Device::NPN || d.type==Device::PNP){
      double change=std::max(std::abs(delta[d.b]-delta[d.a]),std::abs(delta[d.b]-delta[d.c]));
      if(change>.1)factor=std::min(factor,.1/change);
    }
    for(int k=0;k<n;++k)voltage[k]=std::clamp(voltage[k]+factor*delta[k],-2.,8.);
    if(largest<1e-6 && residual<1e-7){settled=true;break;}
  }
  // Electrical reachability distinguishes floating metal from a computed 0 V
  // caused only by the numerical gmin. Sources never silently turn Z into 0.
  std::vector<std::vector<int>> edges(n);
  for(const Device &d:netlist.devices){
    bool connect=d.type==Device::RESISTOR;
    if(d.type==Device::NMOS)connect=voltage[d.b]-std::min(voltage[d.a],voltage[d.c])>1;
    if(d.type==Device::NPN || d.type==Device::PNP){double sign=d.type==Device::PNP?-1:1;connect=sign*(voltage[d.b]-voltage[d.c])>.45;if(connect){edges[d.b].push_back(d.c);edges[d.c].push_back(d.b);}}
    if(connect){int b=(d.type==Device::NMOS || d.type==Device::NPN || d.type==Device::PNP)?d.c:d.b;edges[d.a].push_back(b);edges[b].push_back(d.a);}
    if(transient && d.type==Device::CAPACITOR){if(states[d.a]==State::Low || states[d.a]==State::High)reachable[d.a]=true;if(states[d.b]==State::Low || states[d.b]==State::High)reachable[d.b]=true;}
  }
  std::vector<int> queue;for(int k=0;k<n;++k)if(anchors[k] || reachable[k]){reachable[k]=true;queue.push_back(k);}
  for(size_t q=0;q<queue.size();++q)for(int b:edges[queue[q]])if(!reachable[b]){reachable[b]=true;queue.push_back(b);}
  // Unknown control regions conservatively invalidate their controlled output;
  // driven supply rails stop this propagation.
  for(int pass=0;pass<n;++pass){bool changed=false;
    for(const Device &d:netlist.devices)if(d.type==Device::NPN || d.type==Device::PNP || d.type==Device::NMOS){
      bool bad=uncertain[d.b] || broken[d.b] || (!reachable[d.b] && !anchors[d.b]);
      if(bad)for(int p:{d.a,d.c})if(!anchors[p] && !uncertain[p]){uncertain[p]=true;changed=true;}
    }
    for(int a=0;a<n;++a)for(int b:edges[a])if(!anchors[b]){if(uncertain[a] && !uncertain[b]){uncertain[b]=true;changed=true;}if(broken[a] && !broken[b]){broken[b]=true;changed=true;}}
    if(!changed)break;
  }
  for(int k=0;k<n;++k){
    if(broken[k])states[k]=State::Broken;
    else if(!settled || uncertain[k])states[k]=State::Unknown;
    else if(!reachable[k])states[k]=State::Floating;
    else states[k]=voltage[k]<=low?State::Low:voltage[k]>=high?State::High:State::Unknown;
  }
  status=settled?"Convergence atteinte":"Non-convergence: etat U";
  return settled;
}
bool Simulator::Step(double seconds) {
  if(!std::isfinite(seconds) || seconds<1e-12 || seconds>1)throw std::runtime_error("Pas de temps attendu entre 1e-12 et 1 seconde");
  previous=voltage;converged=Solve(tick>0,seconds);++tick;
  if(history.size()>=4096)history.erase(history.begin(),history.begin()+1024);
  history.push_back({tick,states,voltage});return converged;
}
State Simulator::Get(int node) const {return node>=0 && node<int(states.size())?states[node]:State::Unknown;}
double Simulator::Voltage(int node) const {return node>=0 && node<int(voltage.size())?voltage[node]:std::numeric_limits<double>::quiet_NaN();}
State Simulator::PortState(const std::string &name) const {for(const Port &p:netlist.ports)if(p.name==name)return Get(p.node);throw std::runtime_error("Broche inconnue: "+name);}
std::string Simulator::Csv() const {
  std::vector<Port> series=netlist.ports;series.insert(series.end(),netlist.observations.begin(),netlist.observations.end());
  const auto quote=[](const std::string &name){std::string out="\"";for(char c:name){if(c=='\"')out+='\"';out+=c;}return out+'\"';};
  std::ostringstream s;s<<"tick";for(const Port &p:series)s<<','<<quote(p.name)<<','<<quote(p.name+"_V");s<<'\n'<<std::setprecision(9);
  for(const Sample &h:history){s<<h.tick;for(const Port &p:series)s<<','<<Name(h.states[p.node])<<','<<h.volts[p.node];s<<'\n';}return s.str();
}
}
