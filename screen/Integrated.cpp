#include "Integrated.hpp"
#include "Shortcuts.hpp"
#include "Circuit.hpp"
#include "ComponentDockUI.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace {
bool Control(){const bool *k=bunny_get_keyboard();return k[BKS_LCONTROL] || k[BKS_RCONTROL];}
bool Shift(){const bool *k=bunny_get_keyboard();return k[BKS_LSHIFT] || k[BKS_RSHIFT];}
unsigned NativeColor(silicon::Color c){uint64_t r=RED^BLACK,g=GREEN^BLACK,b=BLUE^BLACK;return BLACK|unsigned(r*((c>>24)&255)/255)|unsigned(g*((c>>16)&255)/255)|unsigned(b*((c>>8)&255)/255);}
std::string KeyName(t_bunny_keysym key){
  switch(key){
    case BKS_ESCAPE:return "escape";case BKS_RETURN:return "enter";case BKS_BACKSPACE:return "backspace";
    case BKS_SPACE:return "space";case BKS_HOME:return "home";case BKS_PAGEUP:return "pageup";case BKS_PAGEDOWN:return "pagedown";
    case BKS_LEFT:return "left";case BKS_RIGHT:return "right";case BKS_UP:return "up";case BKS_DOWN:return "down";case BKS_DELETE:return "delete";
    case BKS_A:return "a";case BKS_D:return "d";case BKS_G:return "g";case BKS_I:return "i";case BKS_N:return "n";case BKS_O:return "o";case BKS_Q:return "q";case BKS_S:return "s";case BKS_Y:return "y";case BKS_Z:return "z";
    default:return "";
  }
}
bool AdvanceObservedSource(LoopData &ld,bool toggle){
  if(!ld.HasIntegratedCircuit() || !ld.CurrentIntegratedCircuit().read_only)return false;
  uint64_t id=ld.documents[ld.active_document]->parent_identity;
  for(LoopDocument *buffer:ld.documents)if(buffer->identity==id){
    if(buffer->integrated){if(toggle)buffer->integrated->running=!buffer->integrated->running;else buffer->integrated->Step();}
    else if(toggle)buffer->running=!buffer->running;
    else hbs::Simulate(*buffer->circuit,*buffer->timer,1);
    ld.CurrentIntegratedCircuit().SyncObservation();return true;
  }
  ld.CurrentIntegratedCircuit().message="Le buffer source a ete ferme ; la derniere observation est conservee";return true;
}
}
bool hbs::IsIntegratedCircuitFile(const std::string &file){
  if(file.empty())return false;
  std::ifstream in(file,std::ios::binary);char prefix[64]={};in.read(prefix,sizeof(prefix));std::string header(prefix,size_t(in.gcount()));
  size_t first=header.find_first_not_of(" \t\r\n");if(first!=std::string::npos && (header.compare(first,7,"BCAD_IC")==0 || header.compare(first,10,"REDSILICON")==0))return true;
  auto ext=std::filesystem::path(file).extension();return ext==".rsi" || ext==".rsi~" || ext==".ic";
}
void hbs::DrawIntegratedCircuit(LoopData &ld){
  auto &editor=ld.CurrentIntegratedCircuit();editor.SyncObservation();
  int offset=ComponentDockWidth(ld.screen),width=std::max(1,ld.screen.pic->buffer.width-2*offset),height=ld.screen.pic->buffer.height;editor.width=width;editor.height=height;
  if(ld.screen.search_panel)ld.integrated_pixels.clear();
  if(!ld.integrated_image || ld.integrated_image->width!=width || ld.integrated_image->height!=height){ld.integrated_image.reset(new silicon::Image(width,height));ld.integrated_pixels.clear();}
  editor.Render(*ld.integrated_image);
  const auto &pixels=ld.integrated_image->pixels;bool all=ld.integrated_pixels.size()!=pixels.size();
  for(int y=0;y<height;++y)for(int x=0;x<width;++x){size_t n=size_t(y)*width+x;if(all || pixels[n]!=ld.integrated_pixels[n])bunny_set_pixel(&ld.screen.pic->buffer,{x+offset,y},NativeColor(pixels[n]));}
  ld.integrated_pixels=pixels;
  auto oldcam=ld.screen.camera;int oldscale=ld.screen.pin_size;ld.screen.camera={0,0};ld.screen.pin_size=1;
  hbs::DrawComponentDocks(ld);hbs::DrawComponentMenu(ld);
  hbs::DrawShortcutBar(ld.screen,&ld);
  ld.screen.camera=oldcam;ld.screen.pin_size=oldscale;
  bunny_blit(&ld.screen.win->buffer,ld.screen.pic,nullptr);bunny_display(ld.screen.win);
}
bool hbs::IntegratedKey(t_bunny_event_state state,t_bunny_keysym key,LoopData &ld){
  if(!ld.HasIntegratedCircuit() || ld.file_browser)return false;
  auto &editor=ld.CurrentIntegratedCircuit();
  if(!editor.prompt.empty()){if(state==GO_DOWN)editor.Key(KeyName(key),Control(),Shift());ld.ProcessIntegratedRequests();return true;}
  if(Control() && key==BKS_W){if(state==GO_DOWN)ld.CloseCurrentDocument();return true;}
  if(Control() && key==BKS_TAB){if(state==GO_DOWN && !ld.documents.empty())ld.SelectDocument((ld.active_document+(Shift()?ld.documents.size()-1:1))%ld.documents.size());return true;}
  if(hbs::FunctionKeyIndex(key)>=0 || key==BKS_ESCAPE)return state==GO_DOWN;
  if(state==GO_DOWN){editor.Key(KeyName(key),Control(),Shift());ld.ProcessIntegratedRequests();}
  return true;
}
bool hbs::IntegratedClick(t_bunny_event_state state,t_bunny_mouse_button button,LoopData &ld){
  if(!ld.HasIntegratedCircuit() || ld.file_browser)return false;
  auto &editor=ld.CurrentIntegratedCircuit();auto p=*bunny_get_mouse_position();
  if(p.y<hbs::ShortcutBarHeight){hbs::HandleShortcutBarClick(state,button,p,ld);return true;}
  int key=button==BMB_LEFT?0:button==BMB_MIDDLE?1:2;
  if(state==GO_DOWN)editor.MouseDown(p.x-ComponentDockWidth(ld.screen),p.y,key,Shift());else editor.MouseUp(p.x-ComponentDockWidth(ld.screen),p.y,key);
  ld.ProcessIntegratedRequests();return true;
}
bool hbs::IntegratedMove(LoopData &ld){if(!ld.HasIntegratedCircuit() || ld.file_browser)return false;auto p=*bunny_get_mouse_position();ld.CurrentIntegratedCircuit().MouseMove(p.x-ComponentDockWidth(ld.screen),p.y);return true;}
bool hbs::IntegratedWheel(int wheel,int delta,LoopData &ld){if(!ld.HasIntegratedCircuit() || ld.file_browser)return false;auto p=*bunny_get_mouse_position();if(wheel==0)ld.CurrentIntegratedCircuit().Wheel(p.x-ComponentDockWidth(ld.screen),p.y,delta);return true;}
const char *hbs::IntegratedShortcutLabel(int index,bool control){
  static const char *normal[]={nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,"INSPECT","STEP","RUN/STOP"};
  static const char *alternate[]={nullptr,nullptr,nullptr,nullptr,"EXTRACT","RESET","NAME","PIN",nullptr,"SOURCE","PARENT","PACKAGE"};
  return index>=0 && index<12?(control?alternate[index]:normal[index]):nullptr;
}
bool hbs::IntegratedShortcut(int index,bool control,LoopData &ld){
  if(!ld.HasIntegratedCircuit())return false;
  if(!control){
    if(index<9)return false;
    if((index==10 || index==11) && AdvanceObservedSource(ld,index==11))return true;
    ld.CurrentIntegratedCircuit().Key(index==9?"inspect":index==10?"step":"space");
  }else{
    if(index==4)ld.CurrentIntegratedCircuit().Key("extract");
    else if(index==5)ld.CurrentIntegratedCircuit().Key("reset");
    else if(index==6)ld.CurrentIntegratedCircuit().Key("name");
    else if(index==7)ld.CurrentIntegratedCircuit().Key("pin");
    else if(index==9)ld.OpenSelectedDefinition();
    else if(index==10)ld.ParentInspection();
    else if(index==11)ld.CurrentIntegratedCircuit().Key("package");
    else return false;
  }
  ld.ProcessIntegratedRequests();return true;
}
