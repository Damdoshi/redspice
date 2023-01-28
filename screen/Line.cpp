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
     {(int)a.x, (int)a.y},
     {(int)b.x, (int)b.y}
    };
  unsigned int	col[2] = {color, color};

  bunny_set_line(&pic->buffer, pos, col);
}

