// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Screen.hpp"

extern "C"
t_bunny_response	screen_key(t_bunny_event_state	state,
				   t_bunny_keysym	sym,
				   LoopData		&ld)
{
  if (state == GO_DOWN)
    return (GO_ON);
  if (sym == BKS_ESCAPE)
    return (EXIT_ON_SUCCESS);
  (void)ld;
  return (GO_ON);
}

