// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Circuit.hpp"
#include		"Screen.hpp"

extern "C"
t_bunny_response	screen_type(uint32_t		unicode,
				    LoopData		&ld)
{
  if (ld.search_panel)
    {
      // On tape dans le moteur de recherche

    }
  else
    {
      // Faire un "if", si on a une selection, on peut renommer
    }
  return (GO_ON);
}
