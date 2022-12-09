// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<lapin.h>
#include		"Screen.hpp"

hbs::Screen::Screen(void)
{
  if ((win = bunny_start(1440, 900, false, "RedSpice")) == NULL)
    throw 0;
  if ((pic = bunny_new_picture(win->buffer.width, win->buffer.height)) == NULL)
    throw 0;
}

hbs::Screen::~Screen(void)
{
  bunny_delete_clipable(pic);
  bunny_stop(win);
}

