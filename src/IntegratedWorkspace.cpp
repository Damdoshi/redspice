#include "Integrated.hpp"
#include "ComponentDockUI.hpp"
#include "Circuit.hpp"
#include "SiliconChip.hpp"
#include <cmath>
#include <memory>
#include <stdexcept>

void LoopData::PublishIntegratedDocument(){
  if(!HasIntegratedCircuit())return;
  LoopDocument &buffer=*documents[active_document];auto &editor=*buffer.integrated;
  if(editor.read_only)return;
  buffer.dirty=editor.Dirty();std::string snapshot=editor.document.Snapshot();
  if(snapshot!=buffer.published_snapshot){silicon::Designs().Publish(buffer.file_name,editor.document);buffer.published_snapshot=snapshot;}
}

void LoopData::ReloadDefinitions(){
  for(LoopDocument *buffer:documents){
    try{
      if(buffer->integrated){auto &editor=*buffer->integrated;
        if(editor.read_only)editor.SyncObservation();
        else if(!editor.design || !editor.design->Current(silicon::Designs()))editor.Rebuild();
      }else if(buffer->circuit){
        std::vector<hbs::IComponent*> parts;buffer->circuit->CollectComponents(parts);
        for(auto *part:parts)if(auto *chip=dynamic_cast<hbs::SiliconChip*>(part))chip->Reload();
      }
    }catch(const std::exception &error){
      buffer->running=false;if(buffer==documents[active_document] && !buffer->integrated)screen.loopsim=false;
      browser_error=std::string("Definition de CI: ")+error.what();
    }
  }
}

void LoopData::AdvanceBackgroundSimulations(){
  if(HasDocument() && !HasIntegratedCircuit())documents[active_document]->running=screen.loopsim;
  for(LoopDocument *buffer:documents){
    try{
      if(buffer->integrated){auto &editor=*buffer->integrated;if(!editor.read_only && editor.running)editor.Step();}
      else if(buffer->running && buffer->circuit)hbs::Simulate(*buffer->circuit,*buffer->timer,screen.loop_ticks_per_frame);
    }catch(const std::exception &error){buffer->running=false;if(buffer==documents[active_document])screen.loopsim=false;browser_error=error.what();}
  }
}

bool LoopData::OpenInspection(const std::shared_ptr<silicon::SimulationSession> &session,const std::string &instance,uint64_t parent){
  if(!session || !session->design || !session->design->instances.count(instance)){browser_error="Instance de CI indisponible";return false;}
  for(size_t n=0;n<documents.size();++n){auto &doc=*documents[n];if(doc.integrated && doc.integrated->read_only && doc.integrated->session==session && doc.integrated->instance_path==instance){SelectDocument(n);file_browser=false;return true;}}
  try{
    std::unique_ptr<LoopDocument> buffer(new LoopDocument("",BufferType::IntegratedCircuit,false));
    buffer->parent_identity=parent;buffer->integrated->Bind(session,instance);buffer->file_name=buffer->integrated->path;
    SaveActiveView();documents.push_back(buffer.release());active_document=documents.size()-1;ApplyActiveView();file_browser=false;browser_error.clear();return true;
  }catch(const std::exception &error){browser_error=error.what();return false;}
}

bool LoopData::InspectSelected(){
  if(!HasDocument())return false;
  if(HasIntegratedCircuit()){
    auto &editor=CurrentIntegratedCircuit();const auto *instance=editor.SelectedInstance();
    if(!instance){editor.message="Selectionner une instance de cellule";return false;}
    auto session=editor.session;std::string path=editor.read_only && !editor.instance_path.empty()?editor.instance_path+"/"+instance->name:instance->name;
    uint64_t parent=editor.read_only?documents[active_document]->parent_identity:documents[active_document]->identity;
    return OpenInspection(session,path,parent);
  }
  auto *chip=dynamic_cast<hbs::SiliconChip*>(screen.GetUniqueSelection());
  if(!chip){browser_error="Selectionner le boitier d'un CI personnalise";return false;}
  try{chip->Compute(1);return OpenInspection(chip->Session(),"",documents[active_document]->identity);}
  catch(const std::exception &error){browser_error=error.what();return false;}
}

bool LoopData::OpenSelectedDefinition(){
  if(!HasDocument())return false;std::string file;
  if(HasIntegratedCircuit()){
    const auto &editor=CurrentIntegratedCircuit();const auto *instance=editor.SelectedInstance();
    file=instance?silicon::ResolveDesignPath(instance->source,editor.document.origin):editor.path;
  }else if(auto *chip=dynamic_cast<hbs::SiliconChip*>(screen.GetUniqueSelection()))file=chip->SourcePath();
  if(file.empty()){browser_error="Aucune definition de CI selectionnee";return false;}
  return OpenFile(file);
}

bool LoopData::ParentInspection(){
  if(!HasIntegratedCircuit() || !CurrentIntegratedCircuit().read_only)return false;
  auto session=CurrentIntegratedCircuit().session;std::string path=CurrentIntegratedCircuit().instance_path;uint64_t parent=documents[active_document]->parent_identity;
  if(!path.empty()){size_t slash=path.rfind('/');return OpenInspection(session,slash==std::string::npos?"":path.substr(0,slash),parent);}
  for(size_t n=0;n<documents.size();++n)if(documents[n]->identity==parent)return SelectDocument(n);
  CurrentIntegratedCircuit().message="Le buffer source a ete ferme ; l'observation conserve son dernier etat";return false;
}

bool LoopData::InsertIntegratedCircuit(const std::string &file){
  if(!HasDocument())return false;
  try{
    if(!hbs::IsIntegratedCircuitFile(file))throw std::runtime_error("Choisir un document de circuit integre");
    if(HasIntegratedCircuit()){
      auto &editor=CurrentIntegratedCircuit();const auto *mouse=bunny_get_mouse_position();editor.InsertInstance(file,editor.World(mouse->x-hbs::ComponentDockWidth(screen),mouse->y));PublishIntegratedDocument();
    }else{
      hbs::Position position=screen.ScreenToWorld(*bunny_get_mouse_position());position.x=std::round(position.x);position.y=std::round(position.y);
      auto *chip=CurrentCircuit().CreateIntegratedCircuit(file,position);
      screen.selected_components.clear();screen.selected_tracks.clear();screen.selected_components.insert(chip);MarkDirty();
    }
    browser_error.clear();file_browser=false;integrated_pixels.clear();return true;
  }catch(const std::exception &error){browser_error=error.what();return false;}
}

void LoopData::ProcessIntegratedRequests(){
  if(!HasIntegratedCircuit())return;auto &editor=CurrentIntegratedCircuit();std::string action=editor.requested;editor.requested.clear();
  bool changed=!editor.read_only && editor.document.Snapshot()!=documents[active_document]->published_snapshot;
  PublishIntegratedDocument();
  if(changed)ReloadDefinitions();
  if(action=="save")SaveCurrentDocument();
  else if(action=="save_as")BeginSaveAs(false);
  else if(action=="open")BeginOpenFileBrowser();
  else if(action=="new")NewDocument(BufferType::IntegratedCircuit);
  else if(action=="quit")RequestQuit();
  else if(action=="inspect")InspectSelected();
  else if(action=="source")OpenSelectedDefinition();
  else if(action=="parent")ParentInspection();
  else if(action=="insert"){file_browser=true;file_browser_mode=FILE_BROWSER_INSERT;browser_target.clear();RefreshBrowser();}
}
