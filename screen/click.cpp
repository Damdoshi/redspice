// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Screen.hpp"

extern "C"
t_bunny_response	screen_click(t_bunny_event_state	state,
				     t_bunny_mouse_button	sym,
				     LoopData			&ld)
{
  (void)state;
  (void)sym;
  (void)ld;
  return (GO_ON);
}

