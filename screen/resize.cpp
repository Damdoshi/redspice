// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Screen.hpp"

extern "C"
t_bunny_response	screen_resize(const t_bunny_window	*win,
				      const t_bunny_position	*siz,
				      LoopData			&ld)
{
  ld.screen.Resize(*win, *siz);
  return (GO_ON);
}

