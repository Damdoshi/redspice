// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include	"Screen.hpp"

int		hbs::Screen::PinSize(int		p)
{
  if (p == 0)
    return (pin_size);
  return ((pin_size = p));
}

