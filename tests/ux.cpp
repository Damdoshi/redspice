#include "ComponentDockUI.hpp"
#include "ComponentMenu.hpp"
#include "Integrated.hpp"
#include "Circuit.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
extern "C" {
t_bunny_response bcad_screen_key(t_bunny_event_state,t_bunny_keysym,void*);
t_bunny_response bcad_screen_type(uint32_t,void*);
t_bunny_response bcad_screen_click(t_bunny_event_state,t_bunny_mouse_button,void*);
t_bunny_response bcad_screen_move(const t_bunny_position*,void*);
t_bunny_response bcad_screen_display(void*);
}
namespace {
size_t checks=0;
void Check(bool value,const char *message){++checks;if(!value)throw std::runtime_error(std::string("UX: ")+message);}
template<class Action> void Reject(Action action){bool rejected=false;try{action();}catch(const std::exception &){rejected=true;}Check(rejected,"invalid favorite metadata rejected");}
struct Board:hbs::Circuit {
  explicit Board(hbs::Timer &timer):Circuit(timer){}
  size_t Count() const {return circuit.size();}
};
void Put(const std::filesystem::path &path,const std::string &text){std::ofstream stream(path);stream<<text;stream.close();Check(bool(stream),"fixture written");}
void Key(LoopData &ld,t_bunny_keysym key){bcad_screen_key(GO_DOWN,key,&ld);bcad_screen_key(GO_UP,key,&ld);}
void Click(LoopData &ld,t_bunny_position point,bool control=false){bunny_test_mouse=point;bunny_test_keys[BKS_LCONTROL]=control;bcad_screen_click(GO_DOWN,BMB_LEFT,&ld);bcad_screen_click(GO_UP,BMB_LEFT,&ld);bunny_test_keys[BKS_LCONTROL]=false;}
t_bunny_position Slot(LoopData &ld,int slot){int width=hbs::ComponentDockWidth(ld.screen);int row=std::clamp((ld.screen.pic->buffer.height-100)/10,24,64);return {slot<10?width/2:ld.screen.pic->buffer.width-width/2,80+slot%10*row+row/2};}
void Drag(LoopData &ld,int from,int to){auto a=Slot(ld,from),b=Slot(ld,to);bunny_test_mouse=a;bcad_screen_click(GO_DOWN,BMB_LEFT,&ld);bunny_test_mouse=b;bcad_screen_move(nullptr,&ld);bcad_screen_click(GO_UP,BMB_LEFT,&ld);}
void Choose(LoopData &ld,const std::string &kind,const std::string &name,bool pin){
  if(!ld.screen.search_panel)Key(ld,BKS_F9);
  Check(ld.screen.search_panel,"component panel opened");ld.screen.search_query=kind=="cell"?std::filesystem::path(name).stem().string():name;
  int match=0,wanted=-1;
  for(const auto &entry:ld.component_menu)if(hbs::ComponentMenuMatches(entry,ld.screen.search_query)){
    if(entry.component.kind==kind && (kind=="cell"?entry.component.source==name:entry.component.type==name))wanted=match;
    ++match;
  }
  Check(wanted>=0,"component available in current mode");ld.screen.search_offset=wanted;
  auto geometry=hbs::GetComponentMenuGeometry(ld.screen);Click(ld,{geometry.x+25,geometry.first_y+10},pin);
}
}
int main(){namespace fs=std::filesystem;using namespace hbs;using bcad::DockEntry;
  fs::path folder=fs::temp_directory_path()/("bcad-ux-tests-"+std::to_string(getpid()));fs::create_directory(folder);
  try{
    const char *labels[]={"NEW CIRCUIT","NEW CHIPSET","SAVE","OPEN","CENTER","HOME","ZOOM IN","ZOOM OUT","COMPONENT","SELECT/PIN","STEP","RUN/STOP"};
    for(int n=0;n<12;++n)Check(std::string(GetShortcutBinding(SHORTCUT_NORMAL,n).label)==labels[n],"default function-key mapping");
    const std::string empty=".chipsets:\n\n.tracks:\n\n.links:\n";Put(folder/"board.rs",empty);
    auto chip=silicon::Example("nmos-inverter");chip.SetPackage("DIP");chip.Save((folder/"inverter.rsi").string());
    auto cell=chip;cell.SetPackage("");cell.Save((folder/"cell.rsi").string());
    {
      Timer timer;Board board(timer);board.Load((folder/"board.rs").string());Screen screen((folder/"board.rs").string());LoopData workspace(board,timer,screen);
      Key(workspace,BKS_F2);Check(workspace.HasIntegratedCircuit(),"F2 creates a chipset");
      Key(workspace,BKS_F1);Check(!workspace.HasIntegratedCircuit() && workspace.documents.size()==3,"F1 creates a printed circuit");
      Check(workspace.SelectDocument(0),"return to original board");
      Choose(workspace,"component","CAPACITOR",true);
      Check(board.Count()==0 && !screen.placing_component,"Ctrl+click pins without inserting a component");
      Check(screen.search_panel && screen.search_query.empty() && screen.search_offset==0,"choosing a favorite clears the search");
      Check(board.Favorites().slots[0].type=="CAPACITOR","first free favorite button used");
      Choose(workspace,"component","CAPACITOR",true);int occupied=0;for(const auto &entry:board.Favorites().slots)occupied+=!entry.Empty();Check(occupied==1,"favorites stay unique");
      Choose(workspace,"component","CAPACITOR",false);
      Check(screen.placing_component && board.Count()==1 && !screen.search_panel && screen.search_query.empty(),"normal selection inserts and clears search");
      Click(workspace,{screen.pic->buffer.width/2,screen.pic->buffer.height/2});Check(!screen.placing_component,"placement completed");
      Drag(workspace,0,10);Check(board.Favorites().slots[0].Empty() && board.Favorites().slots[10].type=="CAPACITOR","favorite moves across screen sides");
      Choose(workspace,"component","RESISTOR",true);CloseComponentMenu(workspace);Drag(workspace,0,10);
      Check(board.Favorites().slots[0].type=="CAPACITOR" && board.Favorites().slots[10].type=="RESISTOR","dropping on an occupied button exchanges favorites");
      Click(workspace,Slot(workspace,10),true);Check(board.Favorites().slots[10].Empty() && board.Count()==1,"Ctrl+click removes favorite only");
      Click(workspace,Slot(workspace,0));Check(screen.placing_component && board.Count()==2,"favorite button starts normal component placement");
      Key(workspace,BKS_DELETE);Check(!screen.placing_component && board.Count()==1,"favorite placement can be cancelled");
      bunny_test_mouse=Slot(workspace,0);bcad_screen_click(GO_DOWN,BMB_LEFT,&workspace);bunny_test_mouse={screen.pic->buffer.width/2,300};bcad_screen_move(nullptr,&workspace);bcad_screen_click(GO_UP,BMB_LEFT,&workspace);
      Check(board.Count()==1 && board.Favorites().slots[0].type=="CAPACITOR","drag outside docks cancels without placing");
      screen.panning=true;screen.right_panning=false;bunny_test_mouse=Slot(workspace,0);bcad_screen_click(GO_UP,BMB_LEFT,&workspace);Check(!screen.panning,"canvas pan releases over a dock");
      bcad_screen_click(GO_UP,BMB_MIDDLE,&workspace);Check(workspace.documents.size()==3,"middle release handles mixed PCB and IC buffers");
      Check(bcad_screen_display(&workspace)==GO_ON,"PCB renders favorites through native callback");
      Key(workspace,BKS_F3);Check(!workspace.file_browser && !workspace.documents[0]->Modified(),"F3 saves the current board");
      Timer copied_time;Board copied(copied_time);copied.Load((folder/"board.rs").string());Check(copied.Favorites()==board.Favorites(),"PCB favorites persist in the circuit file");
      Key(workspace,BKS_F4);Check(workspace.file_browser && workspace.file_browser_mode==FILE_BROWSER_OPEN,"F4 opens shared file and buffer selector");Key(workspace,BKS_ESCAPE);Check(!workspace.file_browser && !workspace.quit_requested,"Escape returns from Open");
      int before_zoom=screen.pin_size;Key(workspace,BKS_F7);Check(screen.pin_size>before_zoom,"F7 zooms in");Key(workspace,BKS_F8);Check(screen.pin_size==before_zoom,"F8 zooms out");
      Choose(workspace,"cell",(folder/"inverter.rsi").string(),true);CloseComponentMenu(workspace);
      Choose(workspace,"cell",(folder/"inverter.rsi").string(),false);Check(!workspace.pending_cell.empty(),"custom chipset selection starts placement");
      Key(workspace,BKS_ESCAPE);Check(workspace.pending_cell.empty() && !workspace.quit_requested,"Escape cancels chipset placement without quitting");
      Check(workspace.OpenFile((folder/"inverter.rsi").string()),"open IC definition");
      auto &editor=workspace.CurrentIntegratedCircuit();editor.simulator->Drive("A",silicon::State::High);editor.Step();auto simulation=editor.simulator;auto tick=simulation->tick;
      Choose(workspace,"tool","resistor",true);
      Check(editor.document.favorites.slots[0].kind=="tool","IC tool can be pinned from Component");
      Check(editor.simulator==simulation && editor.simulator->tick==tick && editor.design->Current(silicon::Designs()),"favorites do not reset electrical simulation");
      CloseComponentMenu(workspace);Click(workspace,Slot(workspace,0));Check(editor.tool==silicon::Tool::Resistor,"IC favorite selects its drawing tool");
      Choose(workspace,"cell",(folder/"cell.rsi").string(),true);CloseComponentMenu(workspace);
      Check(editor.document.favorites.slots[1].source==(folder/"cell.rsi").string(),"bare reusable cell can be pinned");
      Check(bcad_screen_display(&workspace)==GO_ON,"IC renders docks with native event routing");
      fs::create_directory(folder/"moved");Check(workspace.SaveCurrentDocumentAs((folder/"moved/owner.rsi").string()),"IC Save As succeeds");
      auto saved=silicon::Document::Load((folder/"moved/owner.rsi").string());Check(saved.favorites==editor.document.favorites,"IC favorites survive Save As");
      Check(silicon::ReadFile((folder/"moved/owner.rsi").string()).find("../cell.rsi")!=std::string::npos,"cell favorites use portable relative paths");
      Check(workspace.OpenInspection(editor.session,"",workspace.documents[workspace.active_document]->identity),"open read-only observation");
      auto observed=CurrentComponentDock(workspace);Check(!PinComponent(workspace,{"tool","capacitor",""}) && CurrentComponentDock(workspace)==observed,"observation favorites are read-only");
      Check(workspace.SelectDocument(0),"return to PCB for metadata tests");
      auto unusual=folder/"special # \"cell\".rsi";chip.Save(unusual.string());board.Favorites().slots[5]={"cell","",unusual.string()};
      Check(board.Save((folder/"moved/favorites.rs").string()),"save favorites with quoted paths");copied.Load((folder/"moved/favorites.rs").string());Check(copied.Favorites()==board.Favorites(),"spaces, quotes and hash survive PCB favorite serialization");
      auto previous=board.Favorites();Put(folder/"invalid.rs",empty+".misc:\nfavorite left 0 \"component\" \"AND\" \"\"\nfavorite left 0 \"component\" \"OR\" \"\"\n");
      Reject([&]{board.Load((folder/"invalid.rs").string());});Check(board.Favorites()==previous,"rejected favorite load is transactional");
      auto text=chip.Serialize();text.insert(text.find("end "),"favorite left 0 \"tool\" \"resistor\" \"\"\nfavorite right 0 \"tool\" \"resistor\" \"\"\n");Reject([&]{silicon::Document::Parse(text);});
      text=chip.Serialize();text.insert(text.find("end "),"favorite right 10 \"tool\" \"resistor\" \"\"\n");Reject([&]{silicon::Document::Parse(text);});
      Check(IsIntegratedCircuitFile((folder/"inverter.rsi").string()) && !IsIntegratedCircuitFile((folder/"board.rs").string()),"PCB and IC retain distinct file types");
      for(auto *document:workspace.documents)if(document->temporary)fs::remove(document->file_name);
    }
    Check(bunny_test_allocations==0,"graphics resources released");fs::remove_all(folder);
    std::cout<<"PASS unified toolbar, component search and persistent favorites: "<<checks<<" checks\n";return 0;
  }catch(const std::exception &error){fs::remove_all(folder);std::cerr<<error.what()<<'\n';return 1;}
}
