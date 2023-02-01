// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Circuit.hpp"
#include		"Screen.hpp"

bool			hbs::Screen::Draw(hbs::Circuit		&c)
{
  double		x;
  double		y;

  bunny_clear(&pic->buffer, BLACK);
  for (x = -win->buffer.width / pin_size / 2; x < win->buffer.width / pin_size / 2; ++x)
    for (y = -win->buffer.height / pin_size / 2; y < win->buffer.height / pin_size / 2; ++y)
      {
	Position	pos[2] =
	  {
	   {x + camera.x, y + camera.y},
	   {x + camera.x, y + camera.y}
	  };
	Line(pos[0], pos[1], GRAY(128));
      }
  c.Draw(*this);
  bunny_blit(&win->buffer, pic, NULL);
  bunny_blit(&win->buffer, controls, NULL);
  bunny_blit(&win->buffer, author, NULL);
  bunny_display(win);
  return (true);
}
