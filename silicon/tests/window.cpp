// Exercise the real C signatures, then dispatch actual editor events through
// the context. Including the adapter keeps its private LoopData test-local.
#include "../src/Window.cpp"
#include <type_traits>

namespace {
unsigned window_checks=0;
size_t pixels_drawn=0;
bool loop_dispatched=false;
void CheckWindow(bool condition,const char *message){
  ++window_checks;
  if(!condition)throw std::runtime_error(std::string("LibLapin adapter: ")+message);
}
void CountPixels(const char *kind,t_bunny_position,t_bunny_position,unsigned){
  if(std::string(kind)=="pixel")++pixels_drawn;
}
void DispatchLoop(void *data){
  auto &ld=*static_cast<LoopData *>(data);
  CheckWindow(bunny_test_context!=nullptr,"context registered");
  ld.editor.running=true;
  CheckWindow(bunny_test_context->loop(data)==GO_ON,"C loop relay");
  CheckWindow(ld.editor.simulator->tick==1,"loop receives the editor instance");
  loop_dispatched=true;
}
}

void RunWindowTests(){
  using KeyCallback=t_bunny_response (*)(t_bunny_event_state,t_bunny_keysym,void*);
  using LoopCallback=t_bunny_response (*)(void*);
  static_assert(std::is_same<decltype(&silicon_window_key),KeyCallback>::value,"C key callback signature");
  static_assert(std::is_same<decltype(&silicon_window_loop),LoopCallback>::value,"C loop callback signature");
  using ReferenceCallback=t_bunny_response (*)(LoopData&);
  static_assert(!std::is_convertible<ReferenceCallback,LoopCallback>::value,"a reference callback must not masquerade as the C ABI");

  CheckWindow(bunny_test_allocations==0,"initial resource count");
  silicon::Editor editor(silicon::Example("nmos-inverter"));
  auto *window=bunny_start_style(1440,900,TITLEBAR,"test");
  auto *picture=bunny_new_picture(1440,900);
  LoopData ld{editor,window,picture,silicon::Image(1440,900),{},true};
  auto context=MakeWindowContext();
  CheckWindow(context.get_focus==nullptr && context.lost_focus==nullptr && context.close==nullptr &&
              context.entering_context==nullptr && context.leaving_context==nullptr && context.event==nullptr,
              "unused context fields zero-initialized");
  void *data=&ld;
  CheckWindow(context.key(GO_UP,BKS_F6,data)==GO_ON,"key release accepted");
  CheckWindow(editor.simulator->tick==0,"key release does not step");
  CheckWindow(context.key(GO_DOWN,BKS_F6,data)==GO_ON,"C key relay");
  CheckWindow(editor.simulator->tick==1,"key press steps the same editor");

  CheckWindow(context.key(GO_DOWN,BKS_O,data)==GO_ON,"plain O key");
  CheckWindow(editor.prompt.empty(),"plain O does not open a file");
  bunny_test_keys[BKS_LCONTROL]=true;
  context.key(GO_DOWN,BKS_O,data);
  CheckWindow(editor.prompt=="ouvrir","Ctrl+O through C key relay");
  context.type('X',data);
  CheckWindow(editor.prompt_text.empty(),"Ctrl suppresses text input");
  bunny_test_keys[BKS_LCONTROL]=false;
  CheckWindow(context.type('a',data)==GO_ON,"C text relay");
  CheckWindow(editor.prompt_text=="a","text reaches prompt");
  context.key(GO_DOWN,BKS_ESCAPE,data);

  int input_id=0;
  for(const auto &item:editor.document.items)if(item.kind==silicon::Kind::Input)input_id=item.id;
  const auto *input=editor.document.Find(input_id);
  auto point=editor.Screen({input->box.x,input->box.y});
  bunny_test_mouse={point.x+int(editor.zoom/2),point.y+int(editor.zoom/2)};
  CheckWindow(context.click(GO_DOWN,BMB_MIDDLE,data)==GO_ON,"C click relay");
  CheckWindow(editor.document.Find(input_id)->initial==silicon::State::High,"click updates input");
  CheckWindow(editor.simulator->PortState("Y")==silicon::State::Low,"clicked input affects simulation");
  CheckWindow(context.click(GO_UP,BMB_MIDDLE,data)==GO_ON,"C release relay");

  double zoom=editor.zoom;
  CheckWindow(context.wheel(1,1,data)==GO_ON,"secondary wheel accepted");
  CheckWindow(editor.zoom==zoom,"secondary wheel leaves zoom alone");
  CheckWindow(context.wheel(0,1,data)==GO_ON,"C wheel relay");
  CheckWindow(editor.zoom>zoom,"main wheel zooms");
  t_bunny_position movement{5,2};
  CheckWindow(context.move(&movement,data)==GO_ON,"C move relay");

  auto *old=picture;
  bunny_test_fail_after=0;
  t_bunny_position size{1500,950};
  CheckWindow(context.resize(window,&size,data)==GO_ON,"failed resize accepted");
  CheckWindow(picture==old && bunny_test_allocations==2,"failed resize retains previous picture");
  bunny_test_fail_after=-1;
  CheckWindow(context.resize(window,&size,data)==GO_ON,"C resize relay");
  CheckWindow(editor.width==1500 && editor.height==950,"resize updates editor dimensions");
  CheckWindow(picture->buffer.width==1500 && ld.image.width==1500,"resize updates image buffers");
  CheckWindow(bunny_test_allocations==2,"resize releases old picture");

  bunny_test_draw=CountPixels;pixels_drawn=0;
  CheckWindow(context.display(data)==GO_ON,"C display relay");
  CheckWindow(pixels_drawn==size_t(1500)*950,"first display paints all pixels");
  size_t painted=pixels_drawn;
  context.display(data);
  CheckWindow(pixels_drawn==painted,"idle display reuses existing pixels");
  ld.draw=true;context.display(data);
  CheckWindow(pixels_drawn==painted,"unchanged redraw does not resend pixels");
  bunny_test_draw=nullptr;
  editor.quit=true;
  CheckWindow(context.loop(data)==EXIT_ON_SUCCESS,"quit response through C loop relay");
  bunny_delete_clipable(picture);bunny_stop(window);
  CheckWindow(bunny_test_allocations==0,"adapter test releases resources");

  silicon::Editor clean(silicon::Example("nmos-inverter"));
  bunny_test_loop_hook=DispatchLoop;
  CheckWindow(silicon::RunWindow(clean)==0,"native entry point with C callback dispatch");
  bunny_test_loop_hook=nullptr;
  CheckWindow(loop_dispatched,"loop receives opaque application data");
  CheckWindow(bunny_test_allocations==0,"native entry point releases resources");
  for(int fail_after:{0,1}){
    bunny_test_fail_after=fail_after;bool rejected=false;
    try{silicon::RunWindow(clean);}catch(const std::exception &){rejected=true;}
    bunny_test_fail_after=-1;
    CheckWindow(rejected,"window or picture allocation failure reported");
    CheckWindow(bunny_test_allocations==0,"allocation failure releases resources");
  }
  std::cout<<window_checks<<" verifications de l'adaptateur LibLapin reussies\n";
}
