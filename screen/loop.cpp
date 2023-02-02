// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Circuit.hpp"
#include		"Screen.hpp"

extern "C"
t_bunny_response	screen_loop(LoopData			&ld)
{
  if (ld.screen.loopsim)
    hbs::Command("simulate", ld.circuit, ld.timer);
  return (GO_ON);
}

