// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include	"Screen.hpp"

void		hbs::Screen::Circle(Position		pos,
				    Size		siz,
				    unsigned int	color,
				    bool		full)
{
  pos.x -= camera.x;
  pos.y -= camera.y;
  t_bunny_position	_pos = {(int)(pos.x * pin_size), (int)(pos.y * pin_size)};
  t_bunny_position	_siz = {(int)(siz.x * pin_size), (int)(siz.y * pin_size)};

  _pos.x += pic->buffer.width / 2;
  _pos.y += pic->buffer.height / 2;
  if (full)
    bunny_set_disk(&pic->buffer, _pos, _siz, color, TRANSPARENT, 0);
  else
    bunny_set_circle(&pic->buffer, _pos, _siz, color);
}

