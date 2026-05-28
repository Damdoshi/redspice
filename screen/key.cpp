// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Circuit.hpp"
#include		"Screen.hpp"

extern "C"
t_bunny_response	screen_key(t_bunny_event_state	state,
				   t_bunny_keysym	sym,
				   LoopData		&ld)
{
  if (state == GO_DOWN)
    return (GO_ON);
  if (sym == BKS_ESCAPE)
    return (EXIT_ON_SUCCESS);
  if (sym == BKS_S && state == GO_UP)
    {
      if (bunny_get_keyboard()[BKS_LCONTROL] || bunny_get_keyboard()[BKS_RCONTROL])
	ld.circuit.Save(ld.screen.file_name);
    }
  if (state == GO_DOWN)
    {
      if ((sym == BKS_LALT || sym == BKS_RALT))
	ld.search_panel = !ld.search_panel;
      if (ld.search_panel) // Si le panneau de recherche est ouvert
	{
	  if (sym == BKS_PAGEUP)
	    ld.search_offset = bunny_rclamp(ld.search_offset - 10, 0, ld.library.size());
	  if (sym == BKS_PAGEDOWN)
	    ld.search_offset = bunny_rclamp(ld.search_offset + 10, 0, ld.library.size());
	  if (sym == BKS_UP)
	    ld.search_offset = bunny_rclamp(ld.search_offset -  1, 0, ld.library.size());
	  if (sym == BKS_DOWN)
	    ld.search_offset = bunny_rclamp(ld.search_offset +  1, 0, ld.library.size());
	}
    }
  return (GO_ON);
}

