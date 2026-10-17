// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include	"Screen.hpp"
#include        <algorithm>

int		hbs::Screen::PinSize(int		p)
{
  if (p == 0)
    return (pin_size = std::clamp(pin_size, 1, PINSIZE_DEFAULT * 10));
  return (pin_size = std::clamp(p, 1, PINSIZE_DEFAULT * 10));
}



hbs::Position	hbs::Screen::ScreenToWorld(t_bunny_position mouse) const
{
  return (hbs::Position
	  {
    camera.x + (mouse.x - (pic ? pic->buffer.width / 2.0 : 0)) / std::max(1, pin_size),
    camera.y + (mouse.y - (pic ? pic->buffer.height / 2.0 : 0)) / std::max(1, pin_size)
	  });
}

void		hbs::Screen::ZoomAt(t_bunny_position	mouse,
				       int			delta)
{
  int		old_pin_size = PinSize();
  int		new_pin_size;

  if (delta == 0)
    return ;
  new_pin_size = (int)std::clamp((long long)pin_size + delta,
                               (long long)PINSIZE_DEFAULT / 4,
                               (long long)PINSIZE_DEFAULT * 10);
  if (new_pin_size < PINSIZE_DEFAULT / 4)
    new_pin_size = PINSIZE_DEFAULT / 4;
  if (new_pin_size > PINSIZE_DEFAULT * 10)
    new_pin_size = PINSIZE_DEFAULT * 10;
  if (new_pin_size == old_pin_size)
    return ;
  hbs::Position before =
    {
      camera.x + (mouse.x - pic->buffer.width / 2.0) / old_pin_size,
      camera.y + (mouse.y - pic->buffer.height / 2.0) / old_pin_size
    };

  pin_size = new_pin_size;
  camera.x = before.x - (mouse.x - pic->buffer.width / 2.0) / pin_size;
  camera.y = before.y - (mouse.y - pic->buffer.height / 2.0) / pin_size;
}

void		hbs::Screen::PanByScreenDelta(int	dx,
					 int	dy)
{
  camera.x = std::clamp(camera.x - (double)dx / PinSize(), -1000000.0, 1000000.0);
  camera.y = std::clamp(camera.y - (double)dy / PinSize(), -1000000.0, 1000000.0);
}

void		hbs::Screen::ResetZoom(t_bunny_position mouse)
{
  int		old_pin_size = PinSize();
  hbs::Position before =
    {
      camera.x + (mouse.x - pic->buffer.width / 2.0) / old_pin_size,
      camera.y + (mouse.y - pic->buffer.height / 2.0) / old_pin_size
    };

  pin_size = PINSIZE_DEFAULT;
  camera.x = before.x - (mouse.x - pic->buffer.width / 2.0) / pin_size;
  camera.y = before.y - (mouse.y - pic->buffer.height / 2.0) / pin_size;
}
