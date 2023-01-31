// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Screen.hpp"

extern "C"
t_bunny_response	screen_move(const t_bunny_position	*rel,
				    LoopData			&ld)
{
  (void)rel;
  (void)ld;
  return (GO_ON);
}

