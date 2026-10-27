// Native LibLapin adapter. The model, extraction, solver and renderer have no
// dependency on BCAD's circuit engine or on LibLapin.
#include <lapin.h>
#include "Editor.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>

struct LoopData {
  silicon::Editor &editor;
  t_bunny_window *window;
  t_bunny_picture *&picture;
  silicon::Image image;
  std::vector<silicon::Color> previous;
  bool draw=true;
};
static bool Ctrl(){const bool *keys=bunny_get_keyboard();return keys[BKS_LCONTROL] || keys[BKS_RCONTROL];}
static bool Shift(){const bool *keys=bunny_get_keyboard();return keys[BKS_LSHIFT] || keys[BKS_RSHIFT];}
namespace {
t_bunny_response HandleKey(t_bunny_event_state event,t_bunny_keysym key,LoopData &ld){
  if(event!=GO_DOWN)return GO_ON;std::string name;
  switch(key){
    case BKS_ESCAPE:name="escape";break;case BKS_RETURN:name="enter";break;case BKS_BACKSPACE:name="backspace";break;
    case BKS_SPACE:name="space";break;case BKS_HOME:name="home";break;case BKS_PAGEUP:name="pageup";break;case BKS_PAGEDOWN:name="pagedown";break;
    case BKS_LEFT:name="left";break;case BKS_RIGHT:name="right";break;case BKS_UP:name="up";break;case BKS_DOWN:name="down";break;
    case BKS_DELETE:name="delete";break;case BKS_F1:name="help";break;case BKS_F2:name="name";break;case BKS_F3:name="value";break;
    case BKS_F4:name="tech";break;case BKS_F5:name="extract";break;case BKS_F6:name="step";break;case BKS_F7:name="reset";break;
    case BKS_F8:name="example";break;case BKS_F9:name="watch";break;case BKS_F10:name="break";break;
    case BKS_A:name="a";break;case BKS_D:name="d";break;case BKS_G:name="g";break;case BKS_N:name="n";break;
    case BKS_O:name="o";break;case BKS_Q:name="q";break;case BKS_S:name="s";break;case BKS_Y:name="y";break;case BKS_Z:name="z";break;
    default:break;
  }
  ld.editor.Key(name,Ctrl(),Shift());ld.draw=true;return ld.editor.quit?EXIT_ON_SUCCESS:GO_ON;
}
t_bunny_response HandleType(uint32_t code,LoopData &ld){if(!Ctrl())ld.editor.Type(code);ld.draw=true;return GO_ON;}
t_bunny_response HandleClick(t_bunny_event_state event,t_bunny_mouse_button button,LoopData &ld){const auto p=*bunny_get_mouse_position();int b=button==BMB_LEFT?0:button==BMB_MIDDLE?1:2;if(event==GO_DOWN)ld.editor.MouseDown(p.x,p.y,b,Shift());else ld.editor.MouseUp(p.x,p.y,b);ld.draw=true;return GO_ON;}
t_bunny_response HandleMove(const t_bunny_position *,LoopData &ld){const auto p=*bunny_get_mouse_position();ld.editor.MouseMove(p.x,p.y);ld.draw=true;return GO_ON;}
t_bunny_response HandleWheel(int wheel,int delta,LoopData &ld){if(wheel==0){const auto p=*bunny_get_mouse_position();ld.editor.Wheel(p.x,p.y,delta);}ld.draw=true;return GO_ON;}
t_bunny_response HandleResize(const t_bunny_window *,const t_bunny_position *size,LoopData &ld){
  int w=std::clamp(size->x,1100,2400),h=std::clamp(size->y,900,1600);if(w!=size->x || h!=size->y)bunny_resize_window(ld.window,{w,h});
  auto *next=bunny_new_picture(w,h);if(!next){ld.editor.message="Memoire graphique insuffisante";return GO_ON;}
  bunny_delete_clipable(ld.picture);ld.picture=next;ld.editor.width=w;ld.editor.height=h;ld.image=silicon::Image(w,h);ld.previous.clear();ld.draw=true;return GO_ON;
}
t_bunny_response HandleLoop(LoopData &ld){if(ld.editor.running){ld.editor.Step();ld.draw=true;}return ld.editor.quit?EXIT_ON_SUCCESS:GO_ON;}
static unsigned Native(silicon::Color c){
  // Use LibLapin's own channel masks, including platforms with a different
  // byte order. Alpha in the shared renderer has already been composited.
  uint64_t r=(RED ^ BLACK),g=(GREEN ^ BLACK),b=(BLUE ^ BLACK);
  return BLACK|unsigned(r*((c>>24)&255)/255)|unsigned(g*((c>>16)&255)/255)|unsigned(b*((c>>8)&255)/255);
}
t_bunny_response HandleDisplay(LoopData &ld){
  if(ld.draw){ld.editor.Render(ld.image);bool initial=ld.previous.size()!=ld.image.pixels.size();
    for(int y=0;y<ld.image.height;++y)for(int x=0;x<ld.image.width;++x){size_t i=size_t(y)*ld.image.width+x;auto color=ld.image.pixels[i];if(initial || color!=ld.previous[i])bunny_set_pixel(&ld.picture->buffer,{x,y},Native(color));}
    ld.previous=ld.image.pixels;ld.draw=false;
  }
  bunny_blit(&ld.window->buffer,ld.picture,nullptr);bunny_display(ld.window);return GO_ON;
}
}
// LibLapin's C API takes an opaque pointer. Only these relays cross that ABI;
// the C++ event handlers keep their typed reference without function casts.
extern "C" {
bunny_declare_context(silicon_window);
t_bunny_response silicon_window_key(t_bunny_event_state event,t_bunny_keysym key,void *data){return HandleKey(event,key,*static_cast<LoopData *>(data));}
t_bunny_response silicon_window_type(uint32_t code,void *data){return HandleType(code,*static_cast<LoopData *>(data));}
t_bunny_response silicon_window_click(t_bunny_event_state event,t_bunny_mouse_button button,void *data){return HandleClick(event,button,*static_cast<LoopData *>(data));}
t_bunny_response silicon_window_move(const t_bunny_position *relative,void *data){return HandleMove(relative,*static_cast<LoopData *>(data));}
t_bunny_response silicon_window_wheel(int wheel,int delta,void *data){return HandleWheel(wheel,delta,*static_cast<LoopData *>(data));}
t_bunny_response silicon_window_resize(const t_bunny_window *window,const t_bunny_position *size,void *data){return HandleResize(window,size,*static_cast<LoopData *>(data));}
t_bunny_response silicon_window_loop(void *data){return HandleLoop(*static_cast<LoopData *>(data));}
t_bunny_response silicon_window_display(void *data){return HandleDisplay(*static_cast<LoopData *>(data));}
}
static t_bunny_context MakeWindowContext(){
  // Zero all members, including optional callbacks in newer LibLapin versions.
  t_bunny_context context{};
  context.key=silicon_window_key;context.type=silicon_window_type;
  context.click=silicon_window_click;context.move=silicon_window_move;
  context.wheel=silicon_window_wheel;context.resize=silicon_window_resize;
  context.loop=silicon_window_loop;context.display=silicon_window_display;
  return context;
}
static t_bunny_context gl_silicon_window=MakeWindowContext();
int silicon::RunWindow(Editor &editor){
  auto *window=bunny_start_style(editor.width,editor.height,(t_bunny_window_style)(TITLEBAR|RESIZE_BUTTON|CLOSE_BUTTON),"RedSilicon - EFRITS");
  if(!window)throw std::runtime_error("Creation de la fenetre impossible");
  auto *picture=bunny_new_picture(editor.width,editor.height);if(!picture){bunny_stop(window);throw std::runtime_error("Creation de l'image impossible");}
  try {bunny_enable_full_blit(true);LoopData ld{editor,window,picture,Image(editor.width,editor.height),{},true};bunny_set_context(&gl_silicon_window);bunny_loop(window,20,&ld);picture=ld.picture;}
  catch(...){bunny_delete_clipable(picture);bunny_stop(window);throw;}
  bunny_delete_clipable(picture);bunny_stop(window);
  if(editor.Dirty()){
    std::string base=editor.path.empty()?"redsilicon-recovery":editor.path+".recovery";
    std::string recovery=base+".rsi";unsigned n=1;
    while(std::filesystem::exists(recovery))recovery=base+"-"+std::to_string(n++)+".rsi";
    editor.document.Save(recovery);std::cerr<<"Copie de recuperation: "<<recovery<<'\n';
  }
  return 0;
}
