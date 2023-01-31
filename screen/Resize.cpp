// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Screen.hpp"

void			hbs::Screen::Resize(const t_bunny_window &_win,
					    t_bunny_position	pos)
{
  if (win != &_win)
    return ;
  bunny_delete_clipable(pic);
  if ((pic = bunny_new_picture(pos.x, pos.y)) == NULL)
    throw 0;
}

