#include "Editor.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace silicon {
namespace {
constexpr Color bg=0x141923ff,panel=0x202837ff,white=0xe4ecf7ff,muted=0x8393aaff,accent=0x7ac9f5ff;
const Color layer_colors[]={0x77ce69ff,0xef7b7bff,0x7dbeedff,0xb499edff,0xcea05bff,0xe07eafff,0x70cfc5ff,0xedce64ff,0xf1f3f7ff,0xc1cbdaff};
const char *tools[]={"Selection","Rectangle","Contact entree","Contact sortie","Contact VCC","Contact GND","Sonde","Annotation","Resistance","Capacite","Source I","Borne interne","Contact bidir"};
const Layer palette[]={Layer::Active,Layer::Poly,Layer::Metal1,Layer::Metal2,Layer::Contact,Layer::Via};
Color StateColor(State s){switch(s){case State::Low:return 0xed6672ff;case State::High:return 0x6dde9cff;case State::Floating:return 0xc292e6ff;case State::Broken:return 0xf5ce5fff;default:return 0x8994a6ff;}}
std::string Short(const std::string &s,size_t n){return s.size()<=n?s:s.substr(0,n-3)+"...";}
std::string Number(double n){std::ostringstream o;o<<std::setprecision(4)<<n;return o.str();}
bool TwoPins(Tool t){return t==Tool::Resistor || t==Tool::Capacitor || t==Tool::Current;}
bool TwoPins(Kind t){return t==Kind::Resistor || t==Kind::Capacitor || t==Kind::Current;}
bool InstanceBounds(const Document &drawing,const std::string &name,Rect &bounds){
  std::string prefix=name+"/";bool found=false;int l=0,t=0,r=0,b=0;
  for(const Item &i:drawing.items)if(i.kind!=Kind::Note && i.name.compare(0,prefix.size(),prefix)==0){
    int left=i.box.x,top=i.box.y,right=left+i.box.w,bottom=top+i.box.h;
    if(TwoPins(i.kind)){left=std::min(left,i.end.x);top=std::min(top,i.end.y);right=std::max(right,i.end.x+1);bottom=std::max(bottom,i.end.y+1);}
    if(!found){l=left;t=top;r=right;b=bottom;found=true;}else{l=std::min(l,left);t=std::min(t,top);r=std::max(r,right);b=std::max(b,bottom);}
  }
  if(found)bounds={l,t,r-l,b-t};return found;
}
Kind ToolKind(Tool t){switch(t){case Tool::Input:return Kind::Input;case Tool::Output:return Kind::Output;case Tool::Supply:return Kind::Supply;case Tool::Ground:return Kind::Ground;case Tool::Probe:return Kind::Probe;case Tool::Note:return Kind::Note;case Tool::Resistor:return Kind::Resistor;case Tool::Capacitor:return Kind::Capacitor;case Tool::Current:return Kind::Current;case Tool::Terminal:return Kind::Terminal;case Tool::InOut:return Kind::InOut;default:return Kind::Shape;}}
}
Editor::Editor(const Document &d,const std::string &file):document(d),path(file){if(!file.empty())document.origin=ResolveDesignPath(file);for(int k=4;k<8;++k)visible[k]=false;saved=document.Serialize();Refresh();Fit();}
bool Editor::Dirty() const{return !read_only && saved!=document.Serialize();}
Point Editor::World(int x,int y) const {return {int(std::floor((x-(180+width-320)/2.)/zoom+center_x)),int(std::floor((y-(100+height-190)/2.)/zoom+center_y))};}
Point Editor::Screen(Point p) const {return {int(std::lround((p.x-center_x)*zoom+(180+width-320)/2.)),int(std::lround((p.y-center_y)*zoom+(100+height-190)/2.))};}
void Editor::Fit(){
  if(document.items.empty()){center_x=0;center_y=0;zoom=16;return;}
  int l=4096,t=4096,r=-4096,b=-4096;
  const Document &geometry=design?design->flat:document;
  for(const Item &i:geometry.items){if(i.kind==Kind::Note)continue;l=std::min(l,i.box.x);t=std::min(t,i.box.y);r=std::max(r,i.box.x+i.box.w);b=std::max(b,i.box.y+i.box.h);if(TwoPins(i.kind)){l=std::min(l,i.end.x);t=std::min(t,i.end.y);r=std::max(r,i.end.x+1);b=std::max(b,i.end.y+1);}}
  if(l>r)return;center_x=(l+r)/2.;center_y=(t+b)/2.;zoom=std::clamp(std::min(double(width-540)/(r-l+8),double(height-340)/(b-t+8)),5.,28.);
}
void Editor::Refresh(){
  running=false;highlighted=-1;simulator.reset();design.reset();
  try{design=Compile(document);nets=design->nets;if(nets.nodes<=MaxSimulationNodes)simulator.reset(new Simulator(nets,document.technology));}
  catch(const std::exception &e){nets=Netlist();nets.diagnostics.push_back({true,{0,0},e.what()});message=e.what();}
  if(!session)session.reset(new SimulationSession);
  session->design=design;session->simulator=simulator;session->node_map.clear();for(int n=0;n<nets.nodes;++n)session->node_map.push_back(n);++session->generation;
}
void Editor::Rebuild(){if(read_only)SyncObservation();else Refresh();}
void Editor::Bind(const std::shared_ptr<SimulationSession> &s,const std::string &instance){session=s;instance_path=instance;read_only=true;running=false;observed_generation=0;SyncObservation();Fit();}
void Editor::SyncObservation(){
  if(!read_only || !session || !session->design || observed_generation==session->generation)return;
  auto found=session->design->instances.find(instance_path);if(found==session->design->instances.end()){message="Cette instance n'existe plus dans la definition";return;}
  document=found->second.document;path=document.origin;
  nets=session->design->View(instance_path,session->node_map);simulator=session->simulator;
  DesignRepository snapshot;for(const auto &entry:session->design->instances)if(!entry.second.source.empty())snapshot.Publish(entry.second.source,entry.second.document);
  design=Compile(document,snapshot);observed_generation=session->generation;
  message="Observation de l'instance / "+(instance_path.empty()?std::string("racine"):instance_path);
}
void Editor::Changed(const Document &before){document.Validate();undo.push_back(before.Snapshot());if(undo.size()>64)undo.erase(undo.begin());redo.clear();Refresh();}
void Editor::Ask(const std::string &action,const std::string &text){running=false;prompt=action;prompt_text=text;pressed=dragging=panning=boxing=false;}
void Editor::MarkSaved(const std::string &file){for(Item &i:document.items)if(i.kind==Kind::Instance)i.source=ResolveDesignPath(i.source,document.origin);for(auto &entry:document.favorites.slots)if(entry.kind=="cell")entry.source=ResolveDesignPath(entry.source,document.origin);document.origin=ResolveDesignPath(file);path=document.origin;saved=document.Serialize();message="Enregistre: "+path;}
void Editor::Save(const std::string &file){if(read_only)throw std::runtime_error("Ouvrir la definition pour modifier ce circuit");document.Save(file);MarkSaved(file);}
void Editor::Load(const std::string &file){Document next=Document::Load(file);document=next;path=file;saved=next.Serialize();undo.clear();redo.clear();selected.clear();Refresh();Fit();message="Ouvert: "+path;}
void Editor::InsertInstance(const std::string &source,Point position){
  if(read_only)throw std::runtime_error("Ouvrir la definition pour ajouter une cellule");
  std::string file=ResolveDesignPath(source,document.origin);Document cell=Designs().Read(file);Compile(cell);
  Document before=document;Item instance;instance.kind=Kind::Instance;instance.source=file;instance.box={position.x,position.y,1,1};
  int suffix=document.next_id;bool exists;
  do{instance.name="cell"+std::to_string(suffix++);exists=false;for(const Item &i:document.items)exists=exists || i.name==instance.name;}while(exists);
  selected={document.Add(instance)};Changed(before);
}
const Item *Editor::SelectedInstance() const {if(selected.size()!=1)return nullptr;const Item *i=document.Find(*selected.begin());return i && i->kind==Kind::Instance?i:nullptr;}
void Editor::FinishPrompt(){
  std::string action=prompt,text=prompt_text;prompt.clear();prompt_text.clear();
  Document before_prompt=document;
  try {
    if(action=="ouvrir")Load(text);
    else if(action=="enregistrer")Save(text);
    else if(action=="SPICE")WriteFile(text,nets.Spice(document.title));
    else if(action=="CSV") {if(!simulator)throw std::runtime_error("Simulation indisponible");WriteFile(text,simulator->Csv());}
    else if(action=="quitter - taper OUI"){if(text=="OUI"){quit=true;saved=document.Serialize();}}
    else if(action=="nouveau - taper OUI"){if(text=="OUI"){document=Document();path.clear();saved=document.Serialize();selected.clear();undo.clear();redo.clear();Refresh();}}
    else if(action=="exemple"){Document next=Example(text);Document before=document;document=next;Changed(before);Fit();}
    else if(action=="titre"){Document before=document;document.title=text;Changed(before);}
    else if(action=="groupe"){Document before=document;for(Item &i:document.items)if(selected.count(i.id))i.group=text;Changed(before);}
    else if(action=="cellule"){InsertInstance(text,mouse);}
    else if(action=="boitier"){
      Document before=document;std::string name=text;int count=0;size_t colon=text.find(':');
      if(colon!=std::string::npos){name=text.substr(0,colon);size_t used=0;count=std::stoi(text.substr(colon+1),&used);if(used!=text.size()-colon-1 || count<1)throw std::runtime_error("Boitier attendu sous la forme DIP:14");}
      document.SetPackage(name=="none"?"":name,count);Changed(before);
    }
    else if(action=="broche"){
      if(selected.size()!=1)throw std::runtime_error("Selectionner un contact externe");
      Document before=document;Item &i=*document.Find(*selected.begin());if(i.kind<Kind::Input || i.kind>Kind::Ground)throw std::runtime_error("Ce composant n'est pas un contact externe");
      size_t used=0;int pin=std::stoi(text,&used);if(used!=text.size())throw std::runtime_error("Numero de broche attendu");i.pin=pin;Changed(before);
    }
    else if(action=="nom" || action=="valeur"){
      if(selected.size()!=1)throw std::runtime_error("Selectionner un seul objet");
      Document before=document;Item &i=*document.Find(*selected.begin());
      if(action=="nom")i.name=text;else {size_t used=0;double v=std::stod(text,&used);if(used!=text.size())throw std::runtime_error("Nombre attendu");i.value=v;}
      try{Changed(before);}catch(...){document=before;throw;}
    }
  }catch(const std::exception &e){document=before_prompt;message=e.what();}
}
int Editor::Hit(Point p) const {
  for(auto it=document.items.rbegin();it!=document.items.rend();++it)if(it->kind!=Kind::Shape && it->kind!=Kind::Instance && (it->box.Contains(p) || (TwoPins(it->kind) && p==it->end)))return it->id;
  for(auto it=document.items.rbegin();it!=document.items.rend();++it)if(it->kind==Kind::Shape && it->layer==layer && visible[int(layer)] && it->box.Contains(p))return it->id;
  for(auto it=document.items.rbegin();it!=document.items.rend();++it)if(it->kind==Kind::Shape && visible[int(it->layer)] && it->box.Contains(p))return it->id;
  if(design)for(auto it=document.items.rbegin();it!=document.items.rend();++it)if(it->kind==Kind::Instance){Rect bounds;if(InstanceBounds(design->flat,it->name,bounds) && bounds.Contains(p))return it->id;}
  return 0;
}
bool Editor::Button(int x,int y){
  if(!embedded && y>=42 && y<80 && x>=12 && x<1012){int button=(x-12)/100;static const char *keys[]={"new","open","save","extract","space","step","fit","spice","csv","help"};Key(keys[button]);return true;}
  if(x<180){
    if(y>=132 && y<264){int n=int(palette[(y-132)/22]);if(x>=144){visible[n]=!visible[n];}else{layer=Layer(n);tool=Tool::Rectangle;}return true;}
    if(y>=320 && y<606){tool=Tool((y-320)/22);return true;}
    if(y>=615 && y<643){Key("cell");return true;}
    if(y>=650 && y<678){Key("package");return true;}
    return true;
  }
  if(x>=width-320){
    if(y>=235 && y<261){Key("name");return true;}
    if(y>=266 && y<292){Key("value");return true;}
    if(y>=297 && y<323){Key("watch");return true;}
    if(y>=328 && y<354){Key("break");return true;}
    if(y>=374 && y<405){Key(SelectedInstance()?"inspect":"toggle");return true;}
    if(y>=456){int line=(y-456)/38;if(line>=0 && line<int(nets.diagnostics.size())){const Diagnostic &d=nets.diagnostics[line];center_x=d.at.x;center_y=d.at.y;selected.clear();if(d.item)selected.insert(d.item);}}
    return true;
  }
  return y<100 || y>=height-190;
}
void Editor::MouseDown(int x,int y,int button,bool shift){
  mouse=World(x,y);last_pixel={x,y};if(!prompt.empty())return;
  if(button==2){if(dragging)document=drag_original;pressed=dragging=boxing=panning=false;tool=Tool::Select;return;}
  if(button==1){if(x<180 || x>=width-320 || y<100 || y>=height-190)return;int id=Hit(mouse);if(id){selected={id};Key("toggle");}else panning=true;return;}
  if(Button(x,y))return;
  pressed=true;down=mouse;shift_select=shift;drag_original=document;
  if(tool==Tool::Select){int id=Hit(mouse);if(id){if(shift){if(selected.count(id))selected.erase(id);else selected.insert(id);}else if(!selected.count(id))selected={id};dragging=!read_only && selected.count(id);highlighted=nets.NodeAt(mouse,document.Find(id)->layer);}else {if(!shift)selected.clear();boxing=true;}}
}
void Editor::MouseMove(int x,int y){
  if(panning){center_x-=(x-last_pixel.x)/zoom;center_y-=(y-last_pixel.y)/zoom;}
  last_pixel={x,y};mouse=World(x,y);
  if(dragging && pressed){document=drag_original;for(Item &i:document.items)if(selected.count(i.id)){i.box.x+=mouse.x-down.x;i.box.y+=mouse.y-down.y;i.end.x+=mouse.x-down.x;i.end.y+=mouse.y-down.y;}}
}
void Editor::MouseUp(int x,int y,int button){
  if(button==1){panning=false;return;}if(button!=0)return;
  mouse=World(x,y);if(!pressed)return;pressed=false;
  try {
    if(dragging){dragging=false;if(document.Serialize()!=drag_original.Serialize())Changed(drag_original);return;}
    if(boxing){boxing=false;Rect r{std::min(down.x,mouse.x),std::min(down.y,mouse.y),std::abs(mouse.x-down.x)+1,std::abs(mouse.y-down.y)+1};for(const Item &i:document.items)if(r.Contains({i.box.x,i.box.y}))selected.insert(i.id);return;}
    if(x<180 || x>=width-320 || y<100 || y>=height-190)return;
    if(read_only)return;
    Document before=document;Item i;i.layer=layer;i.kind=ToolKind(tool);i.box={mouse.x,mouse.y,1,1};
    if(tool==Tool::Rectangle){i.kind=Kind::Shape;if(layer<Layer::Contact)i.box={std::min(down.x,mouse.x),std::min(down.y,mouse.y),std::abs(down.x-mouse.x)+1,std::abs(down.y-mouse.y)+1};}
    else if(tool==Tool::Select)return;
    else if(TwoPins(tool)){i.box={down.x,down.y,1,1};i.end=mouse;i.value=tool==Tool::Capacitor?1e-9:tool==Tool::Current?.001:10000;i.name=std::string(Name(i.kind))+std::to_string(document.next_id);}
    else {i.name=std::string(Name(i.kind))+std::to_string(document.next_id);i.watch=i.kind!=Kind::Note;}
    selected={document.Add(i)};if(document.Packaged() && i.kind>=Kind::Input && i.kind<=Kind::Ground)document.SetPackage(document.package.name);Changed(before);if(tool==Tool::Note)Ask("nom","");
  }catch(const std::exception &e){document=drag_original;dragging=boxing=false;message=e.what();}
}
void Editor::Wheel(int x,int y,int delta){if(!prompt.empty())return;Point p=World(x,y);zoom=std::clamp(zoom*(delta>0?1.15:1/1.15),4.,48.);Point after=World(x,y);center_x+=p.x-after.x;center_y+=p.y-after.y;}
void Editor::Type(uint32_t c){
  if(prompt.empty() || prompt_text.size()>508 || c<32 || c==127 || c>0x10ffff || (c>=0xd800 && c<=0xdfff))return;
  if(c<128)prompt_text+=char(c);
  else if(c<0x800){prompt_text+=char(0xc0|(c>>6));prompt_text+=char(0x80|(c&63));}
  else if(c<0x10000){prompt_text+=char(0xe0|(c>>12));prompt_text+=char(0x80|((c>>6)&63));prompt_text+=char(0x80|(c&63));}
  else {prompt_text+=char(0xf0|(c>>18));prompt_text+=char(0x80|((c>>12)&63));prompt_text+=char(0x80|((c>>6)&63));prompt_text+=char(0x80|(c&63));}
}
void Editor::Step(){
  if(read_only){message="Avancer la simulation dans le buffer source";return;}
  if(!simulator || !nets.Valid()){message="Corriger les erreurs d'extraction avant simulation";running=false;return;}
  std::vector<State> prior;for(const Item &i:document.items)if(i.breakpoint)prior.push_back(simulator->Get(nets.NodeAt({i.box.x,i.box.y},i.layer)));
  simulator->Step();message=simulator->status;size_t b=0;
  for(const Item &i:document.items)if(i.breakpoint){State state=simulator->Get(nets.NodeAt({i.box.x,i.box.y},i.layer));if(prior[b++]!=state){running=false;message="Arret sur changement de "+i.name;}}
  if(!simulator->converged)running=false;
}
void Editor::Key(const std::string &key,bool ctrl,bool shift){
  try {
    if(!prompt.empty()){if(key=="escape"){prompt.clear();prompt_text.clear();}else if(key=="enter")FinishPrompt();else if(key=="backspace" && !prompt_text.empty()){size_t last=prompt_text.size()-1;while(last>0 && (static_cast<unsigned char>(prompt_text[last])&0xc0)==0x80)--last;prompt_text.erase(last);}return;}
    if(pressed || panning){if(key=="escape"){if(dragging)document=drag_original;pressed=dragging=panning=boxing=false;}return;}
    if(embedded && (key=="new" || key=="open" || key=="save" || (ctrl && (key=="n" || key=="o" || key=="s" || key=="q")))){requested=key=="n"?"new":key=="o"?"open":key=="s"?(shift?"save_as":"save"):key=="q"?"quit":key;return;}
    if(key=="inspect" || key=="source" || key=="parent"){requested=key;return;}
    if(key=="cell" || (ctrl && key=="i")){if(!read_only){if(embedded)requested="insert";else Ask("cellule");}return;}
    if(key=="package"){if(!read_only)Ask("boitier",document.package.name.empty()?"DIP":document.package.name+":"+std::to_string(document.package.pins));return;}
    if(key=="pin"){if(!read_only && selected.size()==1)Ask("broche",std::to_string(document.Find(*selected.begin())->pin));return;}
    if(key=="help" || key=="f1"){Ask("aide");return;}
    if(key=="open" || (ctrl && key=="o")){Ask("ouvrir",path);return;}
    if(key=="new" || (ctrl && key=="n")){if(Dirty())Ask("nouveau - taper OUI");else{document=Document();path.clear();saved=document.Serialize();selected.clear();undo.clear();redo.clear();Refresh();}return;}
    if(key=="save" || (ctrl && key=="s")){if(path.empty() || shift)Ask("enregistrer",path.empty()?"chip.rsi":path);else Save(path);return;}
    if(key=="spice"){Ask("SPICE",path.empty()?"chip.cir":path+".cir");return;}
    if(key=="csv"){Ask("CSV",path.empty()?"trace.csv":path+".csv");return;}
    if(key=="example"){Ask("exemple","nmos-inverter");return;}
    if(key=="escape" || (ctrl && key=="q")){if(Dirty())Ask("quitter - taper OUI");else quit=true;return;}
    if(key=="fit" || key=="home"){Fit();return;}
    if(read_only && key!="watch" && key!="help" && key!="f1" && key!="csv" && !(ctrl && key=="a"))return;
    if(key=="space"){running=!running;return;}
    if(key=="step" || key=="f6"){running=false;Step();return;}
    if(key=="extract" || key=="f5"){Refresh();message=std::to_string(nets.nodes)+" noeuds / "+std::to_string(nets.devices.size())+" dispositifs";return;}
    if(key=="reset" || key=="f7"){if(simulator)simulator->Reset();running=false;return;}
    if(key=="title"){Ask("titre",document.title);return;}
    if(key=="name" || key=="f2"){if(selected.size()==1)Ask("nom",document.Find(*selected.begin())->name);return;}
    if(key=="value" || key=="f3"){if(selected.size()==1)Ask("valeur",Number(document.Find(*selected.begin())->value));return;}
    if(ctrl && key=="g"){Ask("groupe");return;}
    if((ctrl && key=="z") || (ctrl && key=="y")){
      auto &from=key=="z"?undo:redo;auto &to=key=="z"?redo:undo;if(!from.empty()){to.push_back(document.Snapshot());std::string origin=document.origin;auto favorites=document.favorites;document=Document::Parse(from.back());document.origin=origin;document.favorites=favorites;from.pop_back();selected.clear();Refresh();}return;
    }
    Document before=document;
    if(key=="delete"){document.items.erase(std::remove_if(document.items.begin(),document.items.end(),[this](const Item &i){return selected.count(i.id);}),document.items.end());selected.clear();}
    else if(ctrl && key=="d"){std::vector<Item> copies;for(const Item &i:document.items)if(selected.count(i.id)){Item c=i;c.box.x+=2;c.box.y+=2;c.end.x+=2;c.end.y+=2;c.pin=0;if((c.kind>=Kind::Input && c.kind<=Kind::Ground) || c.kind==Kind::Instance)c.name+="_"+std::to_string(document.next_id+copies.size());copies.push_back(c);}selected.clear();for(Item &c:copies)selected.insert(document.Add(c));if(document.Packaged())document.SetPackage(document.package.name);}
    else if(ctrl && key=="a"){selected.clear();for(const Item &i:document.items)selected.insert(i.id);return;}
    else if(key=="tech"){return;}
    else if(key=="toggle"){
      if(selected.size()!=1)return;Item &i=*document.Find(*selected.begin());
      if(i.kind!=Kind::Input && i.kind!=Kind::InOut && i.kind!=Kind::Terminal){message="Choisir une entree ou une borne interne pour changer son etat";return;}
      State choices[]={State::Low,State::High,State::Floating,State::Unknown};int k=0;for(;k<4 && choices[k]!=i.initial;++k);i.initial=choices[(k+1)%4];
      // A stimulus is a document setting, while preserving current charge and
      // history in the running simulation for sequential circuits.
      document.Validate();undo.push_back(before.Snapshot());if(undo.size()>64)undo.erase(undo.begin());redo.clear();if(simulator){simulator->DriveItem(i.id,i.initial);Step();}return;
    }
    else if(key=="watch" || key=="break"){for(Item &i:document.items)if(selected.count(i.id)){if(key=="watch")i.watch=!i.watch;else i.breakpoint=!i.breakpoint;}if(!read_only){undo.push_back(before.Snapshot());if(undo.size()>64)undo.erase(undo.begin());redo.clear();}return;}
    else if(key=="pageup" || key=="pagedown"){
      if(selected.empty())return;int l=4096,t=4096,r=-4096,b=-4096;
      for(const Item &i:document.items)if(selected.count(i.id)){l=std::min(l,i.box.x);t=std::min(t,i.box.y);r=std::max(r,i.box.x+i.box.w);b=std::max(b,i.box.y+i.box.h);if(TwoPins(i.kind)){l=std::min(l,i.end.x);t=std::min(t,i.end.y);r=std::max(r,i.end.x+1);b=std::max(b,i.end.y+1);}}
      for(Item &i:document.items)if(selected.count(i.id)){Rect old=i.box;Point end=i.end;
        if(i.kind==Kind::Instance){Transform turn{{l,t},key=="pagedown"?1:3};Point origin{i.box.x-l,i.box.y-t};Point target=turn.Apply(origin);if(key=="pagedown")target.x+=b-t;else target.y+=r-l;i.box.x=target.x;i.box.y=target.y;i.rotation=(i.rotation+(key=="pagedown"?1:3))%4;continue;}
        if(key=="pagedown"){i.box={l+b-t-(old.y-t)-old.h,t+old.x-l,old.h,old.w};i.end={l+b-t-1-(end.y-t),t+end.x-l};}
        else{i.box={l+old.y-t,t+r-l-(old.x-l)-old.w,old.h,old.w};i.end={l+end.y-t,t+r-l-1-(end.x-l)};}
      }
    }
    else if(key=="left" || key=="right" || key=="up" || key=="down"){int dx=(key=="right")-(key=="left"),dy=(key=="down")-(key=="up");for(Item &i:document.items)if(selected.count(i.id)){i.box.x+=dx;i.box.y+=dy;i.end.x+=dx;i.end.y+=dy;}}
    else return;
    try{if(document.Serialize()!=before.Serialize())Changed(before);}catch(...){document=before;throw;}
  }catch(const std::exception &e){message=e.what();}
}
void Editor::Render(Image &im) const {
  im.Box(0,0,width,height,bg);int bottom=height-190;
  Document drawing=design?design->flat:document;
  if(!read_only && design){
    for(Item &i:drawing.items){const Item *current=document.Find(i.id);if(current && current->kind!=Kind::Instance)i=*current;}
    if(dragging)for(int id:selected){const Item *current=document.Find(id);const Item *old=drag_original.Find(id);if(current && old && current->kind==Kind::Instance){std::string prefix=current->name+"/";for(Item &i:drawing.items)if(i.name.compare(0,prefix.size(),prefix)==0){i.box.x+=current->box.x-old->box.x;i.box.y+=current->box.y-old->box.y;i.end.x+=current->box.x-old->box.x;i.end.y+=current->box.y-old->box.y;}}}
  }
  // Canvas first; opaque panels clip all mask geometry and annotation text.
  if(zoom>=6){Point p=World(180,100),q=World(width-320,bottom);for(int y=p.y;y<=q.y+1;++y)for(int x=p.x;x<=q.x+1;++x){Point s=Screen({x,y});im.Pixel(s.x,s.y,0x344151ff);}}
  for(int l=0;l<10;++l)if(visible[l])for(const Item &i:drawing.items)if(i.kind==Kind::Shape && int(i.layer)==l){Point s=Screen({i.box.x,i.box.y});int w=int(std::lround(i.box.w*zoom)),h=int(std::lround(i.box.h*zoom));Color c=layer_colors[l];im.Box(s.x,s.y,w,h,(c&0xffffff00)|48);im.Box(s.x,s.y,w,h,(c&0xffffff00)|190,false);
    if(i.layer>=Layer::Contact){im.Line(s.x+2,s.y+2,s.x+w-3,s.y+h-3,c);im.Line(s.x+w-3,s.y+2,s.x+2,s.y+h-3,c);}
  }
  if(highlighted>=0)for(const auto &entry:nets.pixels)if(entry.second==highlighted && visible[int(entry.first.layer)]){Point s=Screen({entry.first.x,entry.first.y});im.Box(s.x,s.y,int(zoom)+1,int(zoom)+1,0xffffff55);}
  for(const Device &d:nets.devices)if(d.type==Device::NPN || d.type==Device::PNP || d.type==Device::NMOS){Point p=Screen(d.at);im.Text(p.x+4,p.y-11,d.name+" "+(d.type==Device::NPN?"NPN":d.type==Device::PNP?"PNP":"NMOS"),accent);}
  for(const Item &i:drawing.items)if(i.kind!=Kind::Shape && i.kind!=Kind::Instance){Point p=Screen({i.box.x,i.box.y});p.x+=int(zoom/2);p.y+=int(zoom/2);int node=nets.NodeAt({i.box.x,i.box.y},i.layer);State state=simulator?simulator->Get(node):State::Unknown;
    if(i.kind==Kind::Note){if(document.Find(i.id))im.Text(p.x,p.y,i.name,0xcdd7a8ff);continue;}
    if(TwoPins(i.kind)){Point b=Screen(i.end);b.x+=int(zoom/2);b.y+=int(zoom/2);im.Line(p.x,p.y,b.x,b.y,0xd4c3a7ff);int mx=(p.x+b.x)/2,my=(p.y+b.y)/2;im.Box(mx-6,my-5,13,11,bg);im.Box(mx-6,my-5,13,11,0xd4c3a7ff,false);if(document.Find(i.id))im.Text(mx+10,my-9,i.name+" "+Number(i.value),0xd4c3a7ff);continue;}
    // Descendant terminals belong in their instance view. Showing every
    // nested name here obscures the parent wiring and repeats its contacts.
    if(!document.Find(i.id))continue;
    Color c=StateColor(state);
    if(i.kind==Kind::Terminal){im.Line(p.x,p.y-5,p.x+5,p.y,white);im.Line(p.x+5,p.y,p.x,p.y+5,white);im.Line(p.x,p.y+5,p.x-5,p.y,white);im.Line(p.x-5,p.y,p.x,p.y-5,white);im.Box(p.x-2,p.y-2,5,5,c);}
    else{im.Box(p.x-5,p.y-5,11,11,c);im.Box(p.x-6,p.y-6,13,13,white,false);}
    std::string label=i.name+(i.pin?" #"+std::to_string(i.pin):"")+" "+Name(state);if(simulator && node>=0 && state!=State::Floating)label+=" "+Number(simulator->Voltage(node))+"V";
    im.Box(p.x+9,p.y-12,int(label.size())*6+5,12,0x141923dd);im.Text(p.x+12,p.y-10,label,c);
    if(i.breakpoint)im.Box(p.x-10,p.y-10,21,21,0xef7575ff,false);
  }
  for(const Item &i:document.items)if(i.kind==Kind::Instance){Rect bounds;if(InstanceBounds(drawing,i.name,bounds)){Point p=Screen({bounds.x,bounds.y});im.Box(p.x-3,p.y-3,int(bounds.w*zoom)+6,int(bounds.h*zoom)+6,selected.count(i.id)?white:0x7ac9f577,false);im.Text(p.x,p.y-14,i.name,accent);}}
  for(int id:selected){const Item *i=document.Find(id);if(i && i->kind!=Kind::Instance){Point p=Screen({i->box.x,i->box.y});im.Box(p.x-2,p.y-2,int(i->box.w*zoom)+4,int(i->box.h*zoom)+4,white,false);}}
  if(pressed && !dragging){Point a=Screen({std::min(down.x,mouse.x),std::min(down.y,mouse.y)});int w=int((std::abs(mouse.x-down.x)+1)*zoom),h=int((std::abs(mouse.y-down.y)+1)*zoom);
    if(TwoPins(tool)){Point b=Screen(down),e=Screen(mouse);im.Line(b.x+int(zoom/2),b.y+int(zoom/2),e.x+int(zoom/2),e.y+int(zoom/2),accent);}else{im.Box(a.x,a.y,w,h,0xffffff22);im.Box(a.x,a.y,w,h,white,false);}
  }
  for(const Diagnostic &d:nets.diagnostics){Point p=Screen(d.at);im.Box(p.x-3,p.y-3,int(zoom)+6,int(zoom)+6,0xf3a168ff,false);}
  im.Box(0,0,width,100,panel);
  if(!embedded){im.Text(16,12,"BCAD / CI",accent,2);im.Text(178,15,Short(document.title,55)+(Dirty()?" *":""),white);im.Text(width-310,15,"DESSIN DE CIRCUITS INTEGRES",muted);}
  else{im.Text(16,64,Short(document.title,75)+(Dirty()?" *":""),white,2);im.Text(width-320,65,read_only?"INSTANCE / "+Short(instance_path,28):document.Packaged()?"BOITIER / "+document.package.name:"CELLULE SANS BOITIER",accent);}
  const char *buttons[]={"NOUVEAU","OUVRIR","SAUVER","EXTRAIRE","RUN/PAUSE","PAS","CADRER","SPICE","TRACES CSV","AIDE"};
  if(!embedded)for(int k=0;k<10;++k){int x=12+k*100;im.Box(x,42,92,32,k==4 && running?0x346853ff:0x303e52ff);im.Text(x+6,55,buttons[k],white);}
  im.Text(16,85,Short(path.empty()?"Nouveau dessin / .rsi":path,110),muted);
  im.Box(0,100,180,bottom-100,panel);im.Text(12,111,"COUCHES",white);
  for(int k=0;k<6;++k){int l=int(palette[k]),y=132+k*22;if(int(layer)==l)im.Box(7,y,166,20,0x344357ff);im.Box(13,y+5,10,10,layer_colors[l]);im.Text(29,y+6,Name(Layer(l)),white);im.Text(149,y+6,visible[l]?"O":"-",muted);}
  im.Text(12,298,"OUTILS",white);
  for(int k=0;k<13;++k){int y=320+k*22;if(int(tool)==k)im.Box(7,y,166,20,0x344357ff);im.Text(14,y+6,tools[k],int(tool)==k?accent:white);}
  im.Box(8,615,164,28,0x303e52ff);im.Text(15,626,"INSERER UNE CELLULE",accent);
  im.Box(8,650,164,28,0x303e52ff);im.Text(15,661,document.Packaged()?"BOITIER / "+document.package.name:"AJOUTER UN BOITIER",accent);
  int rx=width-320;im.Box(rx,100,320,bottom-100,panel);im.Text(rx+16,117,"INSPECTION",accent,2);
  im.Text(rx+16,145,std::to_string(selected.size())+" objets selectionnes",muted);
  if(selected.size()==1){const Item *i=document.Find(*selected.begin());if(i){im.Text(rx+16,170,Name(i->kind)+std::string(" / ")+Name(i->layer),white);im.Text(rx+16,188,Short(i->name,44),white);im.Text(rx+16,207,"Groupe / "+Short(i->group,35),muted);
      const char *labels[]={embedded?"CTRL F7 / RENOMMER":"F2 / RENOMMER","VALEUR / UNITES SI","AJOUTER/RETIRER TRACE","ARRET SUR CHANGEMENT"};for(int k=0;k<4;++k){im.Box(rx+12,235+k*31,296,26,0x303e52ff);im.Text(rx+21,244+k*31,labels[k],white);}
      im.Box(rx+12,374,296,30,0x303e52ff);im.Text(rx+21,386,i->kind==Kind::Instance?"F10 / OBSERVER CETTE INSTANCE":"CHANGER L'ETAT / 0 1 Z U",white);
  }}
  im.Text(rx+16,427,"EXTRACTION / "+std::to_string(nets.nodes)+"N / "+std::to_string(nets.devices.size())+"T",accent);
  if(nets.diagnostics.empty())im.Text(rx+16,460,"Aucune erreur detectee",0x6dde9cff);
  for(size_t n=0;n<nets.diagnostics.size() && 467+int(n)*38<bottom;++n){const Diagnostic &d=nets.diagnostics[n];std::string text=d.message;im.Text(rx+16,456+int(n)*38,Short(text,46),0xf3a168ff);if(text.size()>43)im.Text(rx+16,468+int(n)*38,Short(text.substr(43),46),muted);}
  im.Box(0,bottom,width,190,0x1b2230ff);im.Line(0,bottom,width,bottom,0x3d4b60ff);
  im.Text(16,bottom+13,"CHRONOGRAMMES / "+std::to_string(simulator?simulator->tick:0)+" US / PAS 1 US",accent);
  int row=0;for(const Item &i:document.items)if(i.watch && row<5){int y=bottom+35+row*22,node=nets.NodeAt({i.box.x,i.box.y},i.layer);im.Text(16,y+5,Short(i.name,22),white);im.Line(165,y+16,width-20,y+16,0x344151ff);
    if(simulator){const auto &history=simulator->history;size_t count=std::min(size_t(100),history.size()),start=history.size()-count;int prior_y=y+10;
      for(size_t t=0;t<count;++t){const Sample &h=history[start+t];State s=node>=0 && node<int(h.states.size())?h.states[node]:State::Unknown;int px=180+int(t)*(width-210)/100,py=s==State::High?y+2:s==State::Low?y+16:y+9;
        im.Line(px,prior_y,px,py,StateColor(s));im.Line(px,py,px+(width-210)/100,py,StateColor(s));prior_y=py;}
    }++row;
  }
  im.Text(16,height-23,Short(message,175),muted);
  if(!prompt.empty()){
    im.Box(0,0,width,height,0x00000099);int x=width/2-340,y=height/2-130;im.Box(x,y,680,260,panel);im.Box(x,y,680,260,accent,false);im.Text(x+24,y+24,prompt,accent,2);
    if(prompt=="aide")im.Text(x+24,y+65,"Le poly traverse l'actif : un transistor NMOS.\nRelier actif ou poly au metal1 avec un contact.\nMetal1 / metal2 : connexion seulement par un via.\nRectangle : glisser gauche. Contact/via : clic.\nR/C/source I : glisser entre deux conducteurs.\nF2 nom / F3 valeur en SI / F5 extraction / F6 pas / F7 reset\nCtrl O ouvrir / S sauver / Shift S sauver sous\nCtrl A tout / D dupliquer / G groupe / Z annuler / Y refaire\nEntree : valider. Echap : fermer. Exemples : F8.",white);
    else {im.Text(x+24,y+78,Short(prompt_text,100)+"_",white);im.Text(x+24,y+190,"ENTREE / VALIDER     ECHAP / ANNULER",muted);if(prompt=="ouvrir" && Dirty())im.Text(x+24,y+156,"Les modifications non enregistrees seront abandonnees.",0xf3a168ff);}
  }
}
}
