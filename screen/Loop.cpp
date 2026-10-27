// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Circuit.hpp"
#include		"Screen.hpp"

extern "C"
{
  // Typed C++ handlers remain callable by the editor's existing tests. The
  // separately named relays implement LibLapin's actual opaque-pointer ABI.
  t_bunny_response screen_key(t_bunny_event_state,t_bunny_keysym,LoopData&);
  t_bunny_response screen_type(uint32_t,LoopData&);
  t_bunny_response screen_click(t_bunny_event_state,t_bunny_mouse_button,LoopData&);
  t_bunny_response screen_move(const t_bunny_position*,LoopData&);
  t_bunny_response screen_wheel(int,int,LoopData&);
  t_bunny_response screen_resize(const t_bunny_window*,const t_bunny_position*,LoopData&);
  t_bunny_response screen_loop(LoopData&);
  t_bunny_response screen_display(LoopData&);
  t_bunny_response bcad_screen_key(t_bunny_event_state s,t_bunny_keysym k,void *p){return screen_key(s,k,*static_cast<LoopData*>(p));}
  t_bunny_response bcad_screen_type(uint32_t c,void *p){return screen_type(c,*static_cast<LoopData*>(p));}
  t_bunny_response bcad_screen_click(t_bunny_event_state s,t_bunny_mouse_button b,void *p){return screen_click(s,b,*static_cast<LoopData*>(p));}
  t_bunny_response bcad_screen_move(const t_bunny_position *r,void *p){return screen_move(r,*static_cast<LoopData*>(p));}
  t_bunny_response bcad_screen_wheel(int w,int d,void *p){return screen_wheel(w,d,*static_cast<LoopData*>(p));}
  t_bunny_response bcad_screen_resize(const t_bunny_window *w,const t_bunny_position *s,void *p){return screen_resize(w,s,*static_cast<LoopData*>(p));}
  t_bunny_response bcad_screen_loop(void *p){return screen_loop(*static_cast<LoopData*>(p));}
  t_bunny_response bcad_screen_display(void *p){return screen_display(*static_cast<LoopData*>(p));}
}
static t_bunny_context MakeContext(){
  t_bunny_context c{};c.key=bcad_screen_key;c.type=bcad_screen_type;c.click=bcad_screen_click;c.move=bcad_screen_move;
  c.wheel=bcad_screen_wheel;c.resize=bcad_screen_resize;c.loop=bcad_screen_loop;c.display=bcad_screen_display;return c;
}
static t_bunny_context gl_screen=MakeContext();

bool			hbs::Screen::Loop(hbs::Circuit		&c,
					  hbs::Timer		&timer)
{
  LoopData		ld{c, timer, *this};

  bunny_set_context(&gl_screen);
  bunny_loop(win, 50, &ld);
  return (true);
}


