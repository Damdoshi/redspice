#include "Integrated.hpp"
#include "Circuit.hpp"
#include "SiliconChip.hpp"
#include "Shortcuts.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
extern "C" {
t_bunny_response bcad_screen_key(t_bunny_event_state,t_bunny_keysym,void*);
t_bunny_response bcad_screen_type(uint32_t,void*);
t_bunny_response bcad_screen_click(t_bunny_event_state,t_bunny_mouse_button,void*);
t_bunny_response bcad_screen_loop(void*);
t_bunny_response bcad_screen_display(void*);
}
namespace {
unsigned checks=0;
void Check(bool value,const char *message){++checks;if(!value)throw std::runtime_error(std::string("Integrated BCAD: ")+message);}
template<class Action> void Reject(Action action){bool failed=false;try{action();}catch(const std::exception &){failed=true;}Check(failed,"rejected invalid action");}
struct Board:hbs::Circuit {
  explicit Board(hbs::Timer &timer):Circuit(timer){}
  hbs::IComponent &At(const std::string &name){return *circuit.at(name);}
  hbs::SiliconChip &Chip(const std::string &name){return dynamic_cast<hbs::SiliconChip&>(At(name));}
};
void Put(const std::filesystem::path &path,const std::string &text){std::ofstream out(path);out<<text;out.close();Check(bool(out),"fixture written");}
void Step(Board &board,hbs::Timer &timer){timer.Tick();board.Compute();}
void Expect(Board &board,const std::string &name,hbs::Tristate state){auto actual=board.GetOutputValue(name);if(actual!=state)std::cerr<<name<<": got "<<actual<<", expected "<<state<<'\n';Check(actual==state,"output state");}
std::string One(const std::string &file){return ".chipsets:\ninput a(0) [-10,0]\ntrue power [-10,-3]\nfalse ground [-10,3]\nsilicon u("+file+") [0,0]\noutput y [15,0]\n.links:\na:1 u:1\npower:1 u:2\nu:3 y:1\nground:1 u:4\n";}
void TypePrompt(LoopData &ld,const std::string &text){for(unsigned char c:text)bcad_screen_type(c,&ld);bcad_screen_key(GO_DOWN,BKS_RETURN,&ld);}
}
int main(){namespace fs=std::filesystem;using namespace silicon;
  fs::path folder=fs::temp_directory_path()/("bcad-integrated-tests-"+std::to_string(getpid()));fs::create_directory(folder);
  try{
    Document inverter=Example("nmos-inverter");inverter.SetPackage("DIP");inverter.Save((folder/"inverter.rsi").string());
    Document cell=inverter;cell.SetPackage("");for(Item &i:cell.items)if(i.kind>=Kind::Input && i.kind<=Kind::Ground){i.kind=Kind::Terminal;i.pin=0;}cell.Save((folder/"cell.rsi").string());
    Put(folder/"one.rs",One("inverter.rsi"));
    {
      hbs::Timer time;Board board(time);board.Load((folder/"one.rs").string());
      Step(board,time);Expect(board,"y",hbs::TRUE);board.SetValue("a",hbs::TRUE);Step(board,time);Expect(board,"y",hbs::FALSE);
      auto &chip=board.Chip("u");Check(chip.GetPinCount()==4,"package contact count");
      silicon::Editor observer;observer.Bind(chip.Session(),"");int y=observer.nets.NodeAt({0,1},Layer::Metal1);Check(observer.simulator->Get(y)==State::Low,"board instance exposes internal simulation");
      board.SetValue("a",hbs::FALSE);Step(board,time);Check(observer.simulator->Get(y)==State::High,"inspection follows board execution");
      fs::create_directory(folder/"moved");Check(board.Save((folder/"moved/one.rs").string()),"board save-as");
      hbs::Timer time2;Board reloaded(time2);reloaded.Load((folder/"moved/one.rs").string());Step(reloaded,time2);Expect(reloaded,"y",hbs::TRUE);
      Check(silicon::ReadFile((folder/"moved/one.rs").string()).find("../inverter.rsi")!=std::string::npos,"board stores portable IC references");
      Put(folder/"bare.rs",One("cell.rsi"));Reject([&]{board.Load((folder/"bare.rs").string());});board.SetValue("a",hbs::TRUE);Step(board,time);Expect(board,"y",hbs::FALSE);
    }
    {
      std::string cascade=".chipsets:\ninput a(0) [-10,0]\ntrue p [-10,-3]\nfalse g [-10,3]\nsilicon first(inverter.rsi) [0,0]\nsilicon second(inverter.rsi) [15,0]\noutput y [30,0]\n.links:\na:1 first:1\np:1 first:2\np:1 second:2\ng:1 first:4\ng:1 second:4\nfirst:3 second:1\nsecond:3 y:1\n";
      Put(folder/"cascade.rs",cascade);hbs::Timer time;Board board(time);board.Load((folder/"cascade.rs").string());
      for(int a=0;a<2;++a){board.SetValue("a",a?hbs::TRUE:hbs::FALSE);Step(board,time);Expect(board,"y",a?hbs::TRUE:hbs::FALSE);}
      auto left=board.Chip("first").Session(),right=board.Chip("second").Session();
      Check(left->simulator==right->simulator,"all custom ICs on the board share an electrical solve");
      Check(left->simulator->Nets().devices.size()==4,"both internal transistor networks are simulated");
      Check(left->simulator->Nets().nodes==5,"wired package pins merge their electrical nodes");
    }
    {
      Document pass=Example("nmos-sample-hold");for(Item &i:pass.items)if(i.name=="D" || i.name=="Q")i.kind=Kind::InOut;pass.SetPackage("DIP");pass.Save((folder/"pass.rsi").string());
      Put(folder/"pass.rs",".chipsets:\ninput clk(1) [-10,0]\ninput d(Z) [-10,3]\ninput q(1) [-10,6]\nfalse g [-10,9]\nsilicon u(pass.rsi) [0,0]\noutput observed [20,0]\n.links:\nclk:1 u:1\nd:1 u:2\nq:1 u:3\ng:1 u:4\nu:2 observed:1\n");
      hbs::Timer time;Board board(time);board.Load((folder/"pass.rs").string());Step(board,time);Expect(board,"observed",hbs::TRUE);
      board.SetValue("q",hbs::HIGH_IMPEDANCE);board.SetValue("d",hbs::FALSE);Step(board,time);
      Check(board.Chip("u").Compute(3)==hbs::FALSE,"bidirectional pad conducts in the other direction");
      board.SetValue("q",hbs::TRUE);board.SetValue("d",hbs::HIGH_IMPEDANCE);for(int k=0;k<30;++k)Step(board,time);
      board.SetValue("clk",hbs::FALSE);board.SetValue("q",hbs::HIGH_IMPEDANCE);Step(board,time);
      Expect(board,"observed",hbs::HIGH_IMPEDANCE);Check(board.Chip("u").Compute(3)==hbs::TRUE,"internal capacitance retains its charge when the pad is released");
    }
    {
      Document numbered=inverter;for(Item &i:numbered.items){if(i.name=="VCC")i.pin=14;if(i.name=="GND")i.pin=7;if(i.name=="Y")i.pin=4;}numbered.package.pins=14;numbered.Validate();numbered.Save((folder/"numbered.rsi").string());
      Put(folder/"numbered.rs",".chipsets:\ninput a(1) [-10,0]\ntrue p [-10,-3]\nfalse g [-10,3]\nsilicon u(numbered.rsi) [0,0]\noutput y [20,0]\n.links:\na:1 u:1\np:1 u:14\ng:1 u:7\nu:4 y:1\n");
      hbs::Timer time;Board board(time);board.Load((folder/"numbered.rs").string());Step(board,time);Expect(board,"y",hbs::FALSE);Check(board.Chip("u").GetPinCount()==14,"explicit package numbering including NC pins");Check(board.Chip("u").Compute(3)==hbs::HIGH_IMPEDANCE,"unused package contacts stay floating");
    }
    {
      hbs::Timer time;Board board(time);board.Load((folder/"one.rs").string());hbs::Screen screen((folder/"one.rs").string());LoopData workspace(board,time,screen);
      Check(!workspace.HasIntegratedCircuit(),"PCB buffer starts in PCB mode");
      Check(workspace.NewDocument(BufferType::IntegratedCircuit),"create an IC buffer");
      Check(workspace.documents.size()==2 && workspace.HasIntegratedCircuit(),"mixed buffer workspace");Reject([&]{workspace.CurrentCircuit();});
      Check(workspace.CurrentIntegratedCircuit().document.items.empty() && !workspace.CurrentIntegratedCircuit().document.Packaged(),"new IC is an unboxed cell");
      Check(workspace.InsertIntegratedCircuit((folder/"cell.rsi").string()),"insert cell from the shared file selector");
      Check(bcad_screen_display(&workspace)==GO_ON && !workspace.file_browser,"integrated mode renders through the native callback");
      Check(workspace.CurrentIntegratedCircuit().SelectedInstance()!=nullptr,"inserted object is a reference");
      Check(workspace.SaveCurrentDocumentAs((folder/"module.rsi").string()),"save typed IC document");
      Check(hbs::IsIntegratedCircuitFile((folder/"module.rsi").string()),"type is persisted in the document");
      auto definition_id=workspace.documents[workspace.active_document]->identity;
      Check(workspace.InspectSelected(),"enter a subcell as a live inspection buffer");
      Check(workspace.CurrentIntegratedCircuit().read_only,"inspection is distinct from a definition buffer");
      Check(bcad_screen_display(&workspace)==GO_ON && !workspace.file_browser,"instance inspection renders through the native callback");
      Check(workspace.CurrentIntegratedCircuit().instance_path.find("cell")==0,"inspection has an instance path");
      Check(workspace.ParentInspection(),"return to parent IC inspection");Check(workspace.CurrentIntegratedCircuit().instance_path.empty(),"parent path is the root instance");
      Check(workspace.ParentInspection(),"return from inspection to the source buffer");Check(workspace.documents[workspace.active_document]->identity==definition_id,"parent buffer identity survives opening more buffers");
      Check(workspace.OpenFile((folder/"cell.rsi").string()),"open the actual cell definition");
      Check(!workspace.CurrentIntegratedCircuit().read_only,"open-file chooses an editable definition");
      workspace.CurrentIntegratedCircuit().document.title="Modification non enregistree";
      Check(!workspace.CloseCurrentDocument(),"dirty IC asks to save");Check(workspace.file_browser_mode==FILE_BROWSER_SAVE_THEN_CLOSE,"shared close/save prompt");
      Check(workspace.CompleteSavePrompt(true),"explicit discard closes the dirty definition");
      Check(workspace.SelectDocument(0),"return to PCB buffer");screen.selected_components={&board.Chip("u")};Step(board,time);
      Check(workspace.InspectSelected(),"enter a packaged IC from the PCB");auto session=workspace.CurrentIntegratedCircuit().session;
      auto before_step=time.GetTime();bcad_screen_key(GO_UP,BKS_F11,&workspace);Check(time.GetTime()==before_step+1,"stepping an inspection advances its PCB source");
      Check(session==board.Chip("u").Session(),"PCB inspection binds the selected component instance");
      Check(workspace.OpenSelectedDefinition(),"open source while an observation of the same file exists");Check(!workspace.CurrentIntegratedCircuit().read_only,"source and observation do not alias as buffers");
      auto source_index=workspace.active_document;
      Check(workspace.SelectDocument(0),"select PCB before source update");board.SetValue("a",hbs::TRUE);Step(board,time);Expect(board,"y",hbs::FALSE);
      Check(workspace.SelectDocument(source_index),"select source definition");
      for(Item &i:workspace.CurrentIntegratedCircuit().document.items)if(i.kind==Kind::Resistor)i.value=15000;
      workspace.ProcessIntegratedRequests();
      Check(workspace.SelectDocument(0),"updated definition reloads on the board");
      bool updated=false;for(const Device &device:board.Chip("u").Session()->design->nets.devices)updated=updated || (device.type==Device::RESISTOR && device.value==15000);
      Check(updated,"an unsaved source update reaches existing instances");
      Step(board,time);Expect(board,"y",hbs::FALSE);
      size_t before=workspace.documents.size();bunny_test_keys[BKS_LCONTROL]=true;bcad_screen_key(GO_UP,BKS_F1,&workspace);bunny_test_keys[BKS_LCONTROL]=false;
      Check(workspace.documents.size()==before+1 && workspace.HasIntegratedCircuit(),"Ctrl+F1 creates a typed IC buffer through the C ABI");
      auto current=workspace.active_document;bunny_test_keys[BKS_LCONTROL]=true;bcad_screen_key(GO_DOWN,BKS_TAB,&workspace);bunny_test_keys[BKS_LCONTROL]=false;Check(workspace.active_document!=current,"Ctrl+Tab switches buffer types");
      for(LoopDocument *doc:workspace.documents)if(doc->temporary)std::filesystem::remove(doc->file_name);
    }
    Check(bunny_test_allocations==0,"all workspace graphic resources released");
    fs::remove_all(folder);std::cout<<"PASS unified PCB/IC buffers, hierarchy and board simulation: "<<checks<<" checks\n";return 0;
  }catch(const std::exception &error){fs::remove_all(folder);std::cerr<<error.what()<<'\n';return 1;}
}
