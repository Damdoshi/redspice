// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Screen.hpp"
#include <algorithm>

extern "C"
t_bunny_response	screen_resize(const t_bunny_window	*win,
				      const t_bunny_position	*siz,
				      LoopData			&ld)
try
{
  t_bunny_size		size;

  if (win == NULL || siz == NULL)
    return (GO_ON);
  size.x = siz->x;
  size.y = siz->y;
  if(ld.HasIntegratedCircuit()){size.x=std::max(size.x,1100);size.y=std::max(size.y,900);}
  // When the window manager resizes the window, SFML's view must be reset too.
  // Otherwise the old 1440x900 view is stretched to the new window size.
  bunny_resize_window((t_bunny_window*)win, size);
  ld.screen.Resize(*win, size);
  ld.integrated_pixels.clear();
  return (GO_ON);
}
catch (const std::exception &error)
{
  hbs::ReportScreenError(ld, error);
  return (GO_ON);
}
