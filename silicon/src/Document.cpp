#include "Silicon.hpp"
#include <algorithm>
#include <cerrno>
#include <cmath>
#include <fcntl.h>
#include <filesystem>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>

namespace silicon {
static const char *layers[]={"active","poly","metal1","metal2","collector-n","collector-p","base","emitter","contact","via"};
static const char *kinds[]={"shape","input","output","inout","supply","ground","probe","note","resistor","capacitor","current","terminal","instance"};
static const char *states[]={"0","1","U","Z","B"};
bool operator==(Point a,Point b) { return a.x==b.x && a.y==b.y; }
bool Rect::Contains(Point p) const { return p.x>=x && p.x<x+w && p.y>=y && p.y<y+h; }
const char *Name(Layer l) { return layers[static_cast<int>(l)]; }
const char *Name(Kind k) { return kinds[static_cast<int>(k)]; }
const char *Name(State s) { return states[static_cast<int>(s)]; }
Layer ParseLayer(const std::string &s) { for(int i=0;i<10;++i) if(s==layers[i]) return Layer(i); throw std::runtime_error("Couche inconnue: "+s); }
Kind ParseKind(const std::string &s) { for(int i=0;i<13;++i) if(s==kinds[i]) return Kind(i); throw std::runtime_error("Objet inconnu: "+s); }
State ParseState(const std::string &s) { for(int i=0;i<5;++i) if(s==states[i]) return State(i); throw std::runtime_error("Etat attendu: 0, 1, U, Z ou B"); }
bool PixelKey::operator<(const PixelKey &o) const {
  if(layer!=o.layer) return layer<o.layer;
  if(y!=o.y) return y<o.y;
  return x<o.x;
}
int Document::Add(Item item) { item.id=next_id++; items.push_back(item); return item.id; }
Item *Document::Find(int id) { for(Item &i:items) if(i.id==id) return &i; return nullptr; }
const Item *Document::Find(int id) const { for(const Item &i:items) if(i.id==id) return &i; return nullptr; }
void Document::Validate() const {
  favorites.Validate();
  if(technology!="nmos" && technology!="bipolar") throw std::runtime_error("Technologie attendue: nmos ou bipolar");
  if(items.size()>4096 || title.size()>256 || title.find_first_of("\r\n\0",0,3)!=std::string::npos) throw std::runtime_error("Titre invalide ou document trop grand (4096 objets maximum)");
  if(package.name.size()>128 || package.name.find_first_of("\r\n\0",0,3)!=std::string::npos || package.pins<0 || package.pins>64 || package.width<2 || package.width>64)throw std::runtime_error("Boitier invalide (jusqu'a 64 broches)");
  std::set<int> ids,pins; std::set<std::string> names,instances;
  size_t area=0;
  for(const Item &i:items) {
    if(i.id<=0 || i.id>=1000000000 || !ids.insert(i.id).second) throw std::runtime_error("Identifiant duplique ou invalide");
    if(int(i.layer)<0 || int(i.layer)>9 || int(i.kind)<0 || int(i.kind)>12 || int(i.initial)<0 || int(i.initial)>4) throw std::runtime_error("Type invalide");
    if(std::abs(double(i.box.x))>1000000 || std::abs(double(i.box.y))>1000000 || i.box.w<1 || i.box.h<1 || i.box.w>512 || i.box.h>512 || std::abs(double(i.end.x))>1000000 || std::abs(double(i.end.y))>1000000) throw std::runtime_error("Geometrie hors limites");
    if(i.name.size()>256 || i.group.size()>128 || i.name.find_first_of("\r\n\0",0,3)!=std::string::npos || i.group.find_first_of("\r\n\0",0,3)!=std::string::npos) throw std::runtime_error("Texte invalide");
    if(!std::isfinite(i.value) || i.value<0 || i.value>1e12) throw std::runtime_error("Valeur hors limites");
    if(i.kind==Kind::Resistor && i.value<1) throw std::runtime_error("Resistance minimale: 1 ohm");
    if(i.kind==Kind::Capacitor && i.value<1e-15) throw std::runtime_error("Capacite minimale: 1e-15 F");
    if(i.kind==Kind::Current && i.value>.1) throw std::runtime_error("Courant maximal: 0.1 A");
    if(i.kind>=Kind::Input && i.kind<=Kind::Ground) {
      if(i.name.empty() || !names.insert(i.name).second) throw std::runtime_error("Nom de broche vide ou duplique: "+i.name);
      for(unsigned char c:i.name) if(!std::isalnum(c) && c!='_') throw std::runtime_error("Broches: lettres ASCII, chiffres et underscore uniquement");
      if(i.pin<0 || i.pin>64 || (i.pin && !pins.insert(i.pin).second))throw std::runtime_error("Numero de broche invalide ou duplique");
      if(package.Enabled() && i.pin>package.pins)throw std::runtime_error("Numero de contact au-dela du boitier");
    }
    if(i.kind==Kind::Instance){
      if(i.source.empty() || i.source.size()>1024 || i.source.find_first_of("\r\n\0",0,3)!=std::string::npos || i.name.empty() || !instances.insert(i.name).second || i.rotation<0 || i.rotation>3)throw std::runtime_error("Instance invalide");
      for(unsigned char c:i.name)if(!std::isalnum(c) && c!='_' && c!='-')throw std::runtime_error("Nom d'instance invalide");
    }
    if(i.kind==Kind::Shape) area+=size_t(i.box.w)*i.box.h;
  }
  if(area>200000) throw std::runtime_error("Surface maximale: 200000 carres de grille");
}
std::string Document::Serialize() const {
  Validate(); std::ostringstream o; o<<"BCAD_IC 2\ntech "<<technology<<"\ntitle "<<std::quoted(title)<<"\npackage "<<std::quoted(package.name)<<' '<<package.pins<<' '<<package.width<<"\n"<<std::setprecision(17);
  for(const Item &i:items) o<<"item "<<i.id<<' '<<Name(i.kind)<<' '<<Name(i.layer)<<' '<<i.box.x<<' '<<i.box.y<<' '<<i.box.w<<' '<<i.box.h<<' '<<i.end.x<<' '<<i.end.y<<' '<<i.value<<' '<<Name(i.initial)<<' '<<i.watch<<' '<<i.breakpoint<<' '<<std::quoted(i.name)<<' '<<std::quoted(i.group)<<'\n';
  for(const Item &i:items){
    if(i.kind==Kind::Instance){std::filesystem::path source=i.source;if(source.is_absolute() && !origin.empty()){auto relative=source.lexically_relative(std::filesystem::path(origin).parent_path());if(!relative.empty())source=relative;}o<<"instance "<<i.id<<' '<<i.rotation<<' '<<std::quoted(source.generic_string())<<'\n';}
    if(i.pin)o<<"pin "<<i.id<<' '<<i.pin<<'\n';
  }
  bcad::ComponentDock dock=favorites;
  for(auto &entry:dock.slots)if(entry.kind=="cell" && !origin.empty()){
    std::filesystem::path source=entry.source;
    if(source.is_absolute()){auto relative=source.lexically_relative(std::filesystem::path(origin).parent_path());if(!relative.empty())entry.source=relative.generic_string();}
  }
  dock.Write(o);
  o<<"end "<<items.size()<<'\n';return o.str();
}
std::string Document::Snapshot() const {Document copy=*this;for(Item &i:copy.items)if(i.kind==Kind::Instance)i.source=ResolveDesignPath(i.source,origin);for(auto &entry:copy.favorites.slots)if(entry.kind=="cell")entry.source=ResolveDesignPath(entry.source,origin);copy.origin.clear();return copy.Serialize();}
std::string Document::ElectricalSnapshot() const {Document copy=*this;copy.favorites=bcad::ComponentDock();return copy.Snapshot();}
void Document::SetPackage(const std::string &name,int total){
  Document copy=*this;
  copy.package.name=name;
  if(name.empty()){copy.package.pins=0;*this=copy;return;}
  std::set<int> used;int maximum=0;
  for(const Item &i:copy.items)if(i.kind>=Kind::Input && i.kind<=Kind::Ground && i.pin){used.insert(i.pin);maximum=std::max(maximum,i.pin);}
  for(Item &i:copy.items)if(i.kind>=Kind::Input && i.kind<=Kind::Ground && !i.pin){int n=1;while(used.count(n))++n;i.pin=n;used.insert(n);maximum=std::max(maximum,n);}
  copy.package.pins=total?total:std::max(maximum,copy.package.pins);
  copy.Validate();*this=copy;
}
Document Document::Parse(const std::string &text) {
  if(text.size()>4*1024*1024) throw std::runtime_error("Fichier trop grand");
  std::istringstream in(text); std::string line; Document d; int line_number=0,version=0; bool header=false, tech=false,title=false,ended=false,package_seen=false;
  std::set<int> instance_metadata,pin_metadata;
  std::set<size_t> favorite_slots;
  while(std::getline(in,line)) {
    ++line_number; if(line.size()>4096) throw std::runtime_error("Ligne trop longue");
    std::istringstream row(line); std::string tag; if(!(row>>tag) || tag[0]=='#') continue;
    bool ok=true;
    if(ended)throw std::runtime_error("Donnees apres la fin du document");
    if(!header) { ok=bool(row>>version) && ((tag=="REDSILICON" && version==1) || (tag=="BCAD_IC" && version==2)); header=ok; }
    else if(tag=="package" && version==2){ok=!package_seen && bool(row>>std::quoted(d.package.name)>>d.package.pins>>d.package.width);package_seen=true;}
    else if(tag=="instance" && version==2){int id=0,rotation=0;std::string source;ok=bool(row>>id>>rotation>>std::quoted(source));Item *item=d.Find(id);ok=ok && item && item->kind==Kind::Instance && instance_metadata.insert(id).second;if(ok){item->rotation=rotation;item->source=source;}}
    else if(tag=="pin" && version==2){int id=0,n=0;ok=bool(row>>id>>n);Item *item=d.Find(id);ok=ok && item && item->kind>=Kind::Input && item->kind<=Kind::Ground && pin_metadata.insert(id).second;if(ok)item->pin=n;}
    else if(tag=="favorite" && version==2){d.favorites.Read(row,favorite_slots);row.clear();}
    else if(tag=="end") {size_t count=0;ok=bool(row>>count) && count==d.items.size();ended=ok;}
    else if(tag=="tech") { ok=!tech && bool(row>>d.technology); tech=true; }
    else if(tag=="title") { ok=!title && bool(row>>std::quoted(d.title)); title=true; }
    else if(tag=="item") {
      Item i; std::string k,l,s; int watch=-1,brk=-1;
      ok=bool(row>>i.id>>k>>l>>i.box.x>>i.box.y>>i.box.w>>i.box.h>>i.end.x>>i.end.y>>i.value>>s>>watch>>brk>>std::quoted(i.name)>>std::quoted(i.group));
      if(ok && (watch==0 || watch==1) && (brk==0 || brk==1)) {
        i.kind=ParseKind(k); i.layer=ParseLayer(l); i.initial=ParseState(s); i.watch=watch; i.breakpoint=brk;
        d.items.push_back(i); if(i.id>0 && i.id<1000000000) d.next_id=std::max(d.next_id,i.id+1);
      } else ok=false;
    } else ok=false;
    std::string extra; if(!ok || (row>>extra)) throw std::runtime_error("Syntaxe invalide, ligne "+std::to_string(line_number));
    if(d.items.size()>4096) throw std::runtime_error("Trop d'objets");
  }
  if(!header || !tech || !title || !ended) throw std::runtime_error("Document RedSilicon incomplet");
  if(version==2 && !package_seen)throw std::runtime_error("Description du boitier manquante");
  if(version==1){bool ports=false;for(const Item &i:d.items)ports=ports || (i.kind>=Kind::Input && i.kind<=Kind::Ground);if(ports)d.SetPackage("DIP");}
  d.Validate(); return d;
}
std::string ReadFile(const std::string &path) {
  int fd=open(path.c_str(),O_RDONLY|O_CLOEXEC|O_NONBLOCK); struct stat info{};
  if(fd<0) throw std::runtime_error("Ouverture impossible: "+path);
  if(fstat(fd,&info)<0 || !S_ISREG(info.st_mode) || info.st_size<0 || info.st_size>4*1024*1024) { close(fd); throw std::runtime_error("Fichier non regulier ou trop grand"); }
  std::string s(size_t(info.st_size),'\0'); size_t off=0;
  while(off<s.size()) { ssize_t n=read(fd,&s[off],s.size()-off); if(n<0 && errno==EINTR) continue; if(n<=0) { close(fd); throw std::runtime_error("Lecture incomplete"); } off+=size_t(n); }
  char c; ssize_t n; do n=read(fd,&c,1); while(n<0 && errno==EINTR); close(fd);
  if(n!=0) throw std::runtime_error("Fichier modifie pendant lecture");
  return s;
}
void WriteFile(const std::string &path,const std::string &s) {
  namespace fs=std::filesystem;
  fs::path target(path); struct stat info{}; bool exists=lstat(path.c_str(),&info)==0;
  if(path.empty() || (exists && !S_ISREG(info.st_mode))) throw std::runtime_error("Destination non reguliere: "+path);
  fs::path parent=target.parent_path(); if(parent.empty()) parent=".";
  std::string pattern=(parent/("."+target.filename().string()+".XXXXXX")).string();
  std::vector<char> temp(pattern.begin(),pattern.end()); temp.push_back(0);
  int fd=mkstemp(temp.data()); if(fd<0) throw std::runtime_error("Creation impossible: "+path);
  bool ok=true; if(exists && fchmod(fd,info.st_mode&0777)<0) ok=false;
  size_t off=0;
  while(ok && off<s.size()) { ssize_t n=write(fd,s.data()+off,s.size()-off); if(n<0 && errno==EINTR) continue; if(n<=0) ok=false; else off+=size_t(n); }
  int r; do r=fsync(fd); while(r<0 && errno==EINTR); if(r<0) ok=false;
  if(close(fd)<0) ok=false;
  if(ok && rename(temp.data(),path.c_str())<0) ok=false;
  unlink(temp.data());
  if(ok) { int dir=open(parent.c_str(),O_RDONLY|O_DIRECTORY|O_CLOEXEC); if(dir<0) ok=false; else { do r=fsync(dir); while(r<0 && errno==EINTR); if(r<0) ok=false; close(dir); } }
  if(!ok) throw std::runtime_error("Echec de sauvegarde: "+path);
}
Document Document::Load(const std::string &p) {Document d=Parse(ReadFile(p));d.origin=ResolveDesignPath(p);for(Item &i:d.items)if(i.kind==Kind::Instance)i.source=ResolveDesignPath(i.source,d.origin);for(auto &entry:d.favorites.slots)if(entry.kind=="cell")entry.source=ResolveDesignPath(entry.source,d.origin);return d;}
void Document::Save(const std::string &p) const {Document copy=*this;for(Item &i:copy.items)if(i.kind==Kind::Instance)i.source=ResolveDesignPath(i.source,origin);for(auto &entry:copy.favorites.slots)if(entry.kind=="cell")entry.source=ResolveDesignPath(entry.source,origin);copy.origin=ResolveDesignPath(p);WriteFile(p,copy.Serialize());}
}
