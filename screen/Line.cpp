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
  a.x -= camera.x;
  b.x -= camera.x;
  a.y -= camera.y;
  b.y -= camera.y;
  t_bunny_position pos[2] =
    {
     {(int)(a.x * pin_size), (int)(a.y * pin_size)},
     {(int)(b.x * pin_size), (int)(b.y * pin_size)}
    };
  unsigned int	col[2] = {color, color};

  pos[0].x += pic->buffer.width / 2;
  pos[0].y += pic->buffer.height / 2;
  pos[1].x += pic->buffer.width / 2;
  pos[1].y += pic->buffer.height / 2;
  bunny_set_line(&pic->buffer, pos, col);
}

