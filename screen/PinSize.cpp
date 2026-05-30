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



hbs::Position	hbs::Screen::ScreenToWorld(t_bunny_position mouse) const
{
  return (hbs::Position
	  {
	    camera.x + (mouse.x - pic->buffer.width / 2.0) / pin_size,
	    camera.y + (mouse.y - pic->buffer.height / 2.0) / pin_size
	  });
}

void		hbs::Screen::ZoomAt(t_bunny_position	mouse,
				       int			delta)
{
  int		old_pin_size = pin_size;
  int		new_pin_size;

  if (delta == 0)
    return ;
  new_pin_size = pin_size + delta;
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
  camera.x -= (double)dx / pin_size;
  camera.y -= (double)dy / pin_size;
}

void		hbs::Screen::ResetZoom(t_bunny_position mouse)
{
  int		old_pin_size = pin_size;
  hbs::Position before =
    {
      camera.x + (mouse.x - pic->buffer.width / 2.0) / old_pin_size,
      camera.y + (mouse.y - pic->buffer.height / 2.0) / old_pin_size
    };

  pin_size = PINSIZE_DEFAULT;
  camera.x = before.x - (mouse.x - pic->buffer.width / 2.0) / pin_size;
  camera.y = before.y - (mouse.y - pic->buffer.height / 2.0) / pin_size;
}
