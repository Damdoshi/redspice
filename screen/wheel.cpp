// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<algorithm>
#include		"Screen.hpp"

extern "C"
t_bunny_response	screen_wheel(int			id,
			     int			delta,
			     LoopData			&ld)
{
  (void)id;
  if (ld.file_browser)
    {
      hbs::FileBrowserWheel(delta, ld);
      return (GO_ON);
    }
  if (!ld.HasDocument())
    return (GO_ON);
  if (ld.screen.search_panel)
    {
      ld.screen.search_offset = std::max(0, ld.screen.search_offset - delta);
      return (GO_ON);
    }
  ld.screen.context_menu = false;
  ld.screen.ZoomAt(*bunny_get_mouse_position(), delta);
  ld.MarkDirty();
  return (GO_ON);
}
