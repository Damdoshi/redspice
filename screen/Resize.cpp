// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Screen.hpp"

void			hbs::Screen::Resize(const t_bunny_window &_win,
					    t_bunny_position	pos)
{
  t_bunny_picture	*newpic;

  if (win != &_win)
    return ;
  if (pos.x < 1)
    pos.x = 1;
  if (pos.y < 1)
    pos.y = 1;
  if (pic != NULL && pic->buffer.width == pos.x && pic->buffer.height == pos.y)
    return ;
  if ((newpic = bunny_new_picture(pos.x, pos.y)) == NULL)
    return ;
  newpic->position.x = 0;
  newpic->position.y = 0;
  bunny_delete_clipable(pic);
  pic = newpic;
  if (author != NULL)
    author->position.y = pic->buffer.height - author->buffer.height;
}

