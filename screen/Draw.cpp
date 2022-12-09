// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Circuit.hpp"
#include		"Screen.hpp"

bool			hbs::Screen::Draw(hbs::Circuit		&c)
{
  c.Draw(*this);
  bunny_blit(&win->buffer, pic, NULL);
  bunny_display(win);
  return (true);
}
