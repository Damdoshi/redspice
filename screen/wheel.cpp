// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Screen.hpp"

extern "C"
t_bunny_response	screen_wheel(int			id,
				     int			delta,
				     LoopData			&ld)
{
  (void)id;
  (void)delta;
  (void)ld;
  return (GO_ON);
}

