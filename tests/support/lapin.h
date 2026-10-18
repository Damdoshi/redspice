// Test-only LibLapin surface. No real windowing or rasterization is exercised.
#ifndef BCAD_TEST_LAPIN_H
#define BCAD_TEST_LAPIN_H
#include <cstdint>
#include <cstddef>
#include <cmath>
#include <cstdlib>
#include <iostream>
struct t_bunny_position { int x, y; };
using t_bunny_size = t_bunny_position;
struct t_bunny_accurate_position { double x, y; };
using t_bunny_accurate_size = t_bunny_accurate_position;
inline t_bunny_accurate_position operator+(t_bunny_accurate_position a, t_bunny_accurate_position b) { return {a.x+b.x,a.y+b.y}; }
inline t_bunny_accurate_position operator-(t_bunny_accurate_position a, t_bunny_accurate_position b) { return {a.x-b.x,a.y-b.y}; }
struct t_bunny_buffer { int width, height; };
struct t_bunny_picture { t_bunny_buffer buffer; t_bunny_position position; };
struct t_bunny_window { t_bunny_buffer buffer; };
using t_bunny_window_style = int;
constexpr int TITLEBAR=1, RESIZE_BUTTON=2, CLOSE_BUTTON=4;
constexpr unsigned WHITE=0xffffffff, BLACK=0xff, RED=0xff0000ff, BLUE=0xffff, YELLOW=0xffff00ff, TEAL=0x00ffffff, GREEN=0x00ff00ff, PURPLE=0xff00ffff, PINK2=0xff80ffff, TRANSPARENT=0;
#define GRAY(v) ((unsigned)(v)*0x01010100u|0xffu)
#define ALPHA(a,c) (((unsigned)(c)&0xffffff00u)|(unsigned)(a))
enum t_bunny_keysym { BKS_A,BKS_ADD,BKS_BACKSPACE,BKS_C,BKS_D,BKS_DASH,BKS_DELETE,BKS_DOWN,BKS_EQUAL,BKS_ESCAPE,BKS_F1,BKS_F10,BKS_F11,BKS_F12,BKS_F2,BKS_F3,BKS_F4,BKS_F5,BKS_F6,BKS_F7,BKS_F8,BKS_F9,BKS_HOME,BKS_LCONTROL,BKS_LEFT,BKS_LSHIFT,BKS_N,BKS_P,BKS_PAGEDOWN,BKS_PAGEUP,BKS_Q,BKS_R,BKS_RCONTROL,BKS_RETURN,BKS_RIGHT,BKS_RSHIFT,BKS_S,BKS_SPACE,BKS_SUBTRACT,BKS_T,BKS_TAB,BKS_UP,BKS_V,BKS_W,BKS_Z,BKS_LALT,BKS_RALT, BKS_COUNT };
enum t_bunny_mouse_button { BMB_LEFT, BMB_MIDDLE, BMB_RIGHT };
enum t_bunny_event_state { GO_DOWN, GO_UP };
enum t_bunny_response { GO_ON, EXIT_ON_SUCCESS };
struct t_bunny_edge { double x0,y0,x1,y1; };
struct t_bunny_letter { int nb_edge; t_bunny_edge edge[1]; };
constexpr int BFT_0=26,BFT_EXCLAMATION=36,BFT_SLASH=37,BFT_DOT=38;
inline t_bunny_letter gl_vector_font[39] = {};
inline bool bunny_test_keys[BKS_COUNT] = {};
inline t_bunny_position bunny_test_mouse = {};
inline int bunny_test_allocations=0, bunny_test_fail_after=-1;
inline bool bunny_test_allocation_fails() { if (bunny_test_fail_after<0) return false; return bunny_test_fail_after--==0; }
inline t_bunny_window *bunny_start_style(int w,int h,t_bunny_window_style,const char*) { if(bunny_test_allocation_fails())return nullptr; ++bunny_test_allocations; return new t_bunny_window{{w,h}}; }
inline void bunny_stop(t_bunny_window*p) { if(p){--bunny_test_allocations;delete p;} }
inline t_bunny_picture *bunny_new_picture(int w,int h) { if(bunny_test_allocation_fails())return nullptr; ++bunny_test_allocations;return new t_bunny_picture{{w,h},{0,0}}; }
inline t_bunny_picture *bunny_load_picture(const char*) {return bunny_new_picture(32,32);}
inline void bunny_delete_clipable(t_bunny_picture*p) {if(p){--bunny_test_allocations;delete p;}}
inline const bool *bunny_get_keyboard() {return bunny_test_keys;}
inline const t_bunny_position *bunny_get_mouse_position() {return &bunny_test_mouse;}
inline void bunny_set_mouse_position_window(t_bunny_window*,int x,int y) {bunny_test_mouse={x,y};}
inline void bunny_resize_window(t_bunny_window*w,t_bunny_size s) {w->buffer={s.x,s.y};}
inline void bunny_enable_full_blit(bool) {}
inline void bunny_clear(t_bunny_buffer*,unsigned) {}
inline void (*bunny_test_draw)(const char*,t_bunny_position,t_bunny_position,unsigned) = nullptr;
inline void bunny_set_line(t_bunny_buffer*,const t_bunny_position *p,const unsigned *c)
{ if(bunny_test_draw)bunny_test_draw("line",p[0],p[1],c[0]); }
inline void bunny_set_polygon(t_bunny_buffer*,const t_bunny_position*,const unsigned*) {}
inline void bunny_set_circle(t_bunny_buffer*,t_bunny_position p,t_bunny_position r,unsigned c)
{ if(bunny_test_draw)bunny_test_draw("circle",p,r,c); }
inline void bunny_set_disk(t_bunny_buffer*,t_bunny_position p,t_bunny_position r,unsigned c,unsigned,int)
{ if(bunny_test_draw)bunny_test_draw("disk",p,r,c); }
inline void bunny_set_pixel(t_bunny_buffer*,t_bunny_position,unsigned) {}
inline void bunny_blit(t_bunny_buffer*,t_bunny_picture*,const t_bunny_position*) {}
inline void bunny_display(t_bunny_window*) {}
struct LoopData;
#define bunny_declare_context(prefix) \
t_bunny_response prefix##_key(t_bunny_event_state,t_bunny_keysym,LoopData&); \
t_bunny_response prefix##_type(uint32_t,LoopData&); \
t_bunny_response prefix##_click(t_bunny_event_state,t_bunny_mouse_button,LoopData&); \
t_bunny_response prefix##_move(const t_bunny_position*,LoopData&); \
t_bunny_response prefix##_wheel(int,int,LoopData&); \
t_bunny_response prefix##_resize(const t_bunny_window*,const t_bunny_position*,LoopData&); \
t_bunny_response prefix##_loop(LoopData&); \
t_bunny_response prefix##_display(LoopData&)
struct t_bunny_context {
 t_bunny_response (*key)(t_bunny_event_state,t_bunny_keysym,LoopData&);
 t_bunny_response (*type)(uint32_t,LoopData&);
 t_bunny_response (*click)(t_bunny_event_state,t_bunny_mouse_button,LoopData&);
 t_bunny_response (*move)(const t_bunny_position*,LoopData&);
 t_bunny_response (*wheel)(int,int,LoopData&);
 t_bunny_response (*resize)(const t_bunny_window*,const t_bunny_position*,LoopData&);
 t_bunny_response (*loop)(LoopData&);
 t_bunny_response (*display)(LoopData&);
};
inline void bunny_set_context(t_bunny_context*) {}
inline void bunny_loop(t_bunny_window*,int,LoopData*) {}
#endif
