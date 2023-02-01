// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include	"Screen.hpp"

void		hbs::Screen::Line(Position		a,
				  Position		b,
				  unsigned int		color)
{
  t_bunny_position pos[2] =
    {
      {(int)(a.x * pin_size), (int)(a.y * pin_size)},
      {(int)(b.x * pin_size), (int)(b.y * pin_size)}
    };
  unsigned int	col[2] = {color, color};

  bunny_set_line(&pic->buffer, pos, col);
}

