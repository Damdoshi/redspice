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
  t_bunny_size		size;

  if (win == NULL || siz == NULL)
    return (GO_ON);
  size.x = siz->x;
  size.y = siz->y;
  // When the window manager resizes the window, SFML's view must be reset too.
  // Otherwise the old 1440x900 view is stretched to the new window size.
  bunny_resize_window((t_bunny_window*)win, size);
  ld.screen.Resize(*win, *siz);
  return (GO_ON);
}

