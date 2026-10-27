#include "ComponentDockUI.hpp"
#include "ComponentMenu.hpp"
#include "Integrated.hpp"
#include "Editor.hpp"
#include "Circuit.hpp"
#include <cmath>
#include <filesystem>

namespace {
bool Control(){const bool *keys=bunny_get_keyboard();return keys[BKS_LCONTROL] || keys[BKS_RCONTROL];}
bool ReadOnly(const LoopData &ld){return ld.HasIntegratedCircuit() && ld.CurrentIntegratedCircuit().read_only;}
int RowHeight(int height){return std::clamp((height-100)/int(bcad::ComponentDock::SlotsPerSide),24,64);}
unsigned Native(silicon::Color c){uint64_t r=RED^BLACK,g=GREEN^BLACK,b=BLUE^BLACK;return BLACK|unsigned(r*((c>>24)&255)/255)|unsigned(g*((c>>16)&255)/255)|unsigned(b*((c>>8)&255)/255);}
std::string EntryLabel(const bcad::DockEntry &entry){return entry.kind=="cell"?std::filesystem::path(entry.source).stem().string():entry.type;}
void Message(LoopData &ld,const std::string &text){ld.component_status=text;if(ld.HasIntegratedCircuit())ld.CurrentIntegratedCircuit().message=text;}
void Changed(LoopData &ld){ld.MarkDirty();if(ld.HasIntegratedCircuit())ld.PublishIntegratedDocument();}
void SelectMenuEntry(LoopData &ld,const LoopData::MenuEntry &entry,bool pin){
  if(entry.browse){if(pin)return;hbs::CloseComponentMenu(ld);ld.file_browser=true;ld.file_browser_mode=FILE_BROWSER_INSERT;ld.RefreshBrowser();return;}
  if(pin){hbs::PinComponent(ld,entry.component);ld.screen.search_query.clear();ld.screen.search_offset=0;}
  else hbs::ActivateComponent(ld,entry.component);
}
std::vector<size_t> Matches(const LoopData &ld){
  std::vector<size_t> rows;
  for(size_t n=0;n<ld.component_menu.size();++n)if(hbs::ComponentMenuMatches(ld.component_menu[n],ld.screen.search_query))rows.push_back(n);
  return rows;
}
}

int hbs::ComponentDockWidth(const Screen &screen){return std::max(1,std::min(screen.pic->buffer.width/3,std::clamp(screen.pic->buffer.width/12,72,112)));}
int hbs::ComponentDockSlotAt(const Screen &screen,t_bunny_position p){
  int width=ComponentDockWidth(screen),side=p.x>=screen.pic->buffer.width-width?1:0;
  int x=p.x-(side?screen.pic->buffer.width-width:0),row=RowHeight(screen.pic->buffer.height);
  if(x<6 || x>=width-6 || p.y<80 || p.y>=screen.pic->buffer.height)return -1;
  int n=(p.y-80)/row;if(n>=int(bcad::ComponentDock::SlotsPerSide) || (p.y-80)%row>=row-4)return -1;
  return n+side*int(bcad::ComponentDock::SlotsPerSide);
}
bcad::ComponentDock &hbs::CurrentComponentDock(LoopData &ld){return ld.HasIntegratedCircuit()?ld.CurrentIntegratedCircuit().document.favorites:ld.CurrentCircuit().Favorites();}
const bcad::ComponentDock &hbs::CurrentComponentDock(const LoopData &ld){return ld.HasIntegratedCircuit()?ld.CurrentIntegratedCircuit().document.favorites:ld.CurrentCircuit().Favorites();}

bool hbs::ComponentMenuMatches(const LoopData::MenuEntry &entry,const std::string &query){
  if(entry.component.kind=="component")return ComponentMatchesQuery(entry.component.type,query);
  std::string text=LowerString(entry.label+" "+entry.description+" "+entry.component.type+" "+entry.component.source);
  for(const std::string &word:SearchWords(query))if(text.find(word)==std::string::npos)return false;
  return true;
}
void hbs::RefreshComponentMenu(LoopData &ld){
  ld.component_menu.clear();if(!ld.HasDocument())return;
  if(ld.HasIntegratedCircuit()){
    const char *types[]={"input","output","inout","supply","ground","terminal","probe","resistor","capacitor","current","note"};
    const char *descriptions[]={"Contact entree","Contact sortie","Contact bidirectionnel","Contact VCC / alimentation","Contact GND / masse","Borne interne sans contact exterieur","Sonde de tension","Resistance","Condensateur","Source de courant","Annotation"};
    for(size_t n=0;n<11;++n)ld.component_menu.push_back({{"tool",types[n],""},types[n],descriptions[n],false});
  }else for(const std::string &type:ld.CurrentCircuit().GetCreatableTypes())ld.component_menu.push_back({{"component",type,""},type,ComponentShortDescription(type),false});
  std::set<std::string> files;
  for(const LoopDocument *buffer:ld.documents)if(buffer->integrated && !buffer->temporary && !buffer->integrated->read_only && !buffer->file_name.empty())files.insert(buffer->file_name);
  for(const auto &entry:CurrentComponentDock(ld).slots)if(entry.kind=="cell")files.insert(entry.source);
  std::set<std::filesystem::path> folders{std::filesystem::path(ld.documents[ld.active_document]->file_name).parent_path(),ld.browser_directory};
  for(const auto &folder:folders){
    std::error_code ec;std::filesystem::directory_iterator it(folder,ec),end;size_t inspected=0;
    while(!ec && it!=end && inspected++<1024){auto ext=it->path().extension();if(ext==".rsi" || ext==".ic")files.insert(it->path().string());it.increment(ec);}
  }
  const std::string &name=ld.documents[ld.active_document]->file_name;
  std::string current=name.empty()?"":silicon::ResolveDesignPath(name);
  std::set<std::string> added;
  for(const std::string &candidate:files)try{
    std::string file=silicon::ResolveDesignPath(candidate);
    if(file==current || !added.insert(file).second)continue;
    silicon::Document document=silicon::Designs().Read(file);
    if(!ld.HasIntegratedCircuit() && !document.Packaged())continue;
    std::string description=(document.Packaged()?"CI / ":"Cellule / ")+document.title;
    ld.component_menu.push_back({{"cell","",file},std::filesystem::path(file).stem().string(),description,false});
  }catch(const std::exception &){}
  ld.component_menu.push_back({{},"Open chipset...","Choisir un autre fichier de cellule ou de puce",true});
}
void hbs::CloseComponentMenu(LoopData &ld){ld.screen.search_panel=false;ld.screen.search_query.clear();ld.screen.search_offset=0;ld.integrated_pixels.clear();}
void hbs::ToggleComponentMenu(LoopData &ld){
  if(!ld.HasDocument()){ld.BeginOpenFileBrowser();return;}
  if(ReadOnly(ld)){Message(ld,"Ouvrir la definition pour ajouter un composant");return;}
  if(ld.screen.search_panel){CloseComponentMenu(ld);return;}
  RefreshComponentMenu(ld);ld.screen.search_offset=0;ld.screen.search_panel=true;ld.pending_cell.clear();ld.integrated_pixels.clear();
}
bool hbs::PinComponent(LoopData &ld,const bcad::DockEntry &entry){
  if(ReadOnly(ld)){Message(ld,"Les favoris se modifient dans la definition du circuit");return false;}
  auto result=CurrentComponentDock(ld).Add(entry);
  if(result==bcad::ComponentDock::Added){Changed(ld);Message(ld,"Favori ajoute / Ctrl+clic retire / glisser deplace");return true;}
  Message(ld,result==bcad::ComponentDock::AlreadyPresent?"Ce composant figure deja dans les favoris":"Les vingt emplacements sont occupes");return false;
}
void hbs::ActivateComponent(LoopData &ld,const bcad::DockEntry &entry){
  entry.Validate();if(ReadOnly(ld)){Message(ld,"Ouvrir la definition pour ajouter un composant");return;}
  if(entry.kind=="cell"){
    auto doc=silicon::Designs().Read(entry.source);auto design=silicon::Compile(doc);
    if(!design->nets.Valid())throw std::runtime_error("Extraction de la cellule invalide");
    if(!ld.HasIntegratedCircuit() && !doc.Packaged())throw std::runtime_error("Ce CI doit avoir un boitier pour etre place sur une carte");
    ld.pending_cell=entry.source;CloseComponentMenu(ld);Message(ld,"Cliquer dans le dessin pour placer "+EntryLabel(entry)+" / Echap annule");return;
  }
  if(ld.HasIntegratedCircuit()){
    if(entry.kind!="tool")throw std::runtime_error("Ce composant appartient au mode circuit imprime");
    const char *types[]={"input","output","supply","ground","probe","note","resistor","capacitor","current","terminal","inout"};
    const silicon::Tool tools[]={silicon::Tool::Input,silicon::Tool::Output,silicon::Tool::Supply,silicon::Tool::Ground,silicon::Tool::Probe,silicon::Tool::Note,silicon::Tool::Resistor,silicon::Tool::Capacitor,silicon::Tool::Current,silicon::Tool::Terminal,silicon::Tool::InOut};
    for(size_t n=0;n<11;++n)if(entry.type==types[n]){ld.CurrentIntegratedCircuit().tool=tools[n];CloseComponentMenu(ld);Message(ld,"Outil / "+entry.type);return;}
    throw std::runtime_error("Outil CI inconnu");
  }
  if(entry.kind!="component")throw std::runtime_error("Cet outil appartient au mode circuit integre");
  auto p=ld.screen.ScreenToWorld(*bunny_get_mouse_position());p.x=std::round(p.x);p.y=std::round(p.y);
  auto *part=ld.CurrentCircuit().CreateUserComponent(entry.type,p);
  ld.screen.selected_components.clear();ld.screen.selected_tracks.clear();
  if(auto *track=dynamic_cast<hbs::Track*>(part))ld.screen.selected_tracks.insert(track);else ld.screen.selected_components.insert(part);
  ld.screen.placing_component=true;ld.screen.component_to_place=part;ld.screen.placing_type=entry.type;ld.screen.context_menu=false;
  CloseComponentMenu(ld);ld.MarkDirty();Message(ld,"Placement / "+entry.type);
}

bool hbs::ComponentInterfaceKey(t_bunny_event_state state,t_bunny_keysym key,LoopData &ld){
  if(ld.file_browser || !ld.HasDocument())return false;
  if(key==BKS_ESCAPE && (ld.dock_pressed>=0 || !ld.pending_cell.empty())){
    if(state==GO_UP){ld.dock_pressed=-1;ld.dock_dragged=false;ld.pending_cell.clear();Message(ld,"Placement annule");}return true;
  }
  if(!ld.screen.search_panel || FunctionKeyIndex(key)>=0 || (Control() && (key==BKS_TAB || key==BKS_N || key==BKS_O || key==BKS_S || key==BKS_Q || key==BKS_W)))return false;
  if(ld.component_menu.empty())RefreshComponentMenu(ld);
  if(key==BKS_ESCAPE){if(state==GO_UP)CloseComponentMenu(ld);return true;}
  if(state!=GO_DOWN)return true;
  if(key==BKS_BACKSPACE && !ld.screen.search_query.empty()){ld.screen.search_query.pop_back();ld.screen.search_offset=0;}
  auto rows=Matches(ld);int maximum=std::max(0,int(rows.size())-1);
  if(key==BKS_PAGEUP)ld.screen.search_offset-=10;
  if(key==BKS_PAGEDOWN)ld.screen.search_offset+=10;
  if(key==BKS_UP)--ld.screen.search_offset;
  if(key==BKS_DOWN)++ld.screen.search_offset;
  ld.screen.search_offset=std::clamp(ld.screen.search_offset,0,maximum);
  if(key==BKS_RETURN && !rows.empty()){auto entry=ld.component_menu[rows[size_t(ld.screen.search_offset)]];SelectMenuEntry(ld,entry,Control());}
  return true;
}
bool hbs::ComponentInterfaceClick(t_bunny_event_state state,t_bunny_mouse_button button,LoopData &ld){
  if(ld.file_browser || !ld.HasDocument())return false;
  auto p=*bunny_get_mouse_position();int slot=ComponentDockSlotAt(ld.screen,p),width=ComponentDockWidth(ld.screen);
  if(ld.component_interface_press && button==BMB_LEFT && state==GO_UP){ld.component_interface_press=false;return true;}
  if(ld.dock_pressed>=0){
    if(button==BMB_LEFT && state==GO_UP){int from=ld.dock_pressed;bool dragged=ld.dock_dragged || std::abs(p.x-ld.dock_press_position.x)>5 || std::abs(p.y-ld.dock_press_position.y)>5;ld.dock_pressed=-1;ld.dock_dragged=false;
      if(dragged){if(slot>=0 && !ReadOnly(ld) && CurrentComponentDock(ld).Move(size_t(from),size_t(slot))){Changed(ld);Message(ld,"Favori deplace");}}
      else {auto entry=CurrentComponentDock(ld).slots[size_t(from)];if(!entry.Empty())ActivateComponent(ld,entry);}
    }
    return true;
  }
  // A release belongs to the widget that received the press, even when a
  // canvas drag or middle-button pan finishes over one of the side columns.
  if(state==GO_UP)return false;
  if(p.y>=ShortcutBarHeight && (p.x<width || p.x>=ld.screen.pic->buffer.width-width)){
    if(button==BMB_LEFT && state==GO_DOWN && slot>=0){
      if(Control()){if(!ReadOnly(ld) && CurrentComponentDock(ld).Remove(size_t(slot))){Changed(ld);Message(ld,"Favori retire");}ld.component_interface_press=true;}
      else if(!CurrentComponentDock(ld).slots[size_t(slot)].Empty()){ld.dock_pressed=slot;ld.dock_press_position=p;ld.dock_dragged=false;}
      else ld.component_interface_press=true;
    }
    return true;
  }
  if(p.y<ShortcutBarHeight)return false;
  if(ld.screen.search_panel){
    if(ld.component_menu.empty())RefreshComponentMenu(ld);
    if(button==BMB_LEFT && state==GO_DOWN){
      auto geometry=GetComponentMenuGeometry(ld.screen);auto rows=Matches(ld);int row=(p.y-geometry.first_y)/geometry.line+ld.screen.search_offset;
      if(p.x>=geometry.x+8 && p.x<geometry.x+geometry.list_w-8 && p.y>=geometry.first_y && p.y<geometry.first_y+geometry.rows*geometry.line && row>=0 && row<int(rows.size())){
        auto entry=ld.component_menu[rows[size_t(row)]];SelectMenuEntry(ld,entry,Control());
      }else if(!IsInsideComponentMenu(ld.screen,p))CloseComponentMenu(ld);
      ld.component_interface_press=true;
    }else if(button==BMB_RIGHT && state==GO_DOWN)CloseComponentMenu(ld);
    return true;
  }
  if(!ld.pending_cell.empty()){
    if(button==BMB_RIGHT && state==GO_DOWN){ld.pending_cell.clear();Message(ld,"Placement annule");return true;}
    if(button==BMB_LEFT && state==GO_DOWN){
      if(ld.HasIntegratedCircuit() && (p.x<width+180 || p.x>=ld.screen.pic->buffer.width-width-320 || p.y<100 || p.y>=ld.screen.pic->buffer.height-190))return false;
      std::string file=ld.pending_cell;if(ld.InsertIntegratedCircuit(file)){ld.pending_cell.clear();ld.component_interface_press=true;Message(ld,"Cellule placee");}return true;
    }
  }
  return false;
}
bool hbs::ComponentInterfaceMove(LoopData &ld){
  if(ld.dock_pressed<0)return false;
  auto p=*bunny_get_mouse_position();ld.dock_dragged=ld.dock_dragged || std::abs(p.x-ld.dock_press_position.x)>5 || std::abs(p.y-ld.dock_press_position.y)>5;return true;
}

void hbs::RenderComponentDock(silicon::Image &image,const bcad::ComponentDock &dock,int side,int pressed,int hovered,bool dragging){
  image.Box(0,ShortcutBarHeight,image.width,image.height-ShortcutBarHeight,0x1b2432ff);
  image.Text(9,62,"FAVORITES",0x8393aaff);int row=RowHeight(image.height),limit=std::max(3,(image.width-24)/6);
  for(int n=0;n<int(bcad::ComponentDock::SlotsPerSide);++n){
    int slot=n+side*int(bcad::ComponentDock::SlotsPerSide),y=80+n*row;const auto &entry=dock.slots[size_t(slot)];
    bool active=pressed==slot || hovered==slot;auto outline=active?0x7ac9f5ff:0x3c4c62ff;
    image.Box(6,y,image.width-12,row-4,active?0x344b63ff:0x252f40ff);image.Box(6,y,image.width-12,row-4,outline,false);
    if(entry.Empty()){image.Text(image.width/2-3,y+(row-12)/2,"+",0x52647eff);continue;}
    std::string label=EntryLabel(entry);image.Text(12,y+9,label.substr(0,size_t(limit)),0xe4ecf7ff);
    if(row>=46 && label.size()>size_t(limit))image.Text(12,y+23,label.substr(size_t(limit),size_t(limit)),0xe4ecf7ff);
    if(row>=46)image.Text(12,y+row-17,entry.kind=="cell"?"CHIPSET":entry.kind=="tool"?"IC TOOL":"COMPONENT",0x8393aaff);
    if(dragging && hovered==slot)image.Box(8,y+2,image.width-16,row-8,0x7ac9f5ff,false);
  }
}
void hbs::DrawComponentDocks(LoopData &ld){
  if(!ld.HasDocument())return;int width=ComponentDockWidth(ld.screen),height=ld.screen.pic->buffer.height;
  auto mouse=*bunny_get_mouse_position();int hover=ComponentDockSlotAt(ld.screen,mouse);
  for(int side=0;side<2;++side){silicon::Image image(width,height);RenderComponentDock(image,CurrentComponentDock(ld),side,ld.dock_pressed,hover,ld.dock_dragged);int offset=side?ld.screen.pic->buffer.width-width:0;
    for(int y=ShortcutBarHeight;y<height;++y)for(int x=0;x<width;++x)bunny_set_pixel(&ld.screen.pic->buffer,{x+offset,y},Native(image.pixels[size_t(y)*width+x]));
  }
  if(!ld.component_status.empty() && ld.screen.pic->buffer.width>2*width){
    int middle=ld.screen.pic->buffer.width-2*width;silicon::Image status(middle,22);
    status.Box(0,0,middle,22,0x1b2432ff);status.Text(10,7,ld.component_status.substr(0,size_t(std::max(1,(middle-20)/6))),0x7ac9f5ff);
    for(int y=0;y<22 && y<height;++y)for(int x=0;x<middle;++x)bunny_set_pixel(&ld.screen.pic->buffer,{x+width,y+std::max(0,height-22)},Native(status.pixels[size_t(y)*middle+x]));
  }
}
