// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Screen.hpp"

extern "C"
t_bunny_response	screen_wheel(int			id,
				     int			delta,
				     LoopData			&ld)
{
  return (GO_ON);
  (void)id;
  double		x = ld.screen.pic->buffer.width / ld.screen.PinSize();
  double		y = ld.screen.pic->buffer.height / ld.screen.PinSize();

  ld.screen.PinSize(ld.screen.PinSize() + delta);
  double		nx = ld.screen.pic->buffer.width / ld.screen.PinSize();
  double		ny = ld.screen.pic->buffer.height / ld.screen.PinSize();

  ld.screen.camera.x -= nx - x;
  ld.screen.camera.y -= ny - y;
  return (GO_ON);
}

