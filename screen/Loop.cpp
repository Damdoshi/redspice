// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Circuit.hpp"
#include		"Screen.hpp"

extern "C"
{
  bunny_declare_context(screen);
  t_bunny_context	gl_screen =
    {
     .key = screen_key,
     .type = screen_type,
     .click = screen_click,
     .move = screen_move,
     .wheel = screen_wheel,
     .resize = screen_resize,
     .loop = screen_loop,
     .display = screen_display
    };
}

bool			hbs::Screen::Loop(hbs::Circuit		&c,
					  hbs::Timer		&timer)
{
  LoopData		ld{c, timer, *this};

  bunny_set_context(&gl_screen);
  bunny_loop(win, 25, &ld);
  return (true);
}


