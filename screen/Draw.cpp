// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Circuit.hpp"
#include		"Screen.hpp"

bool			hbs::Screen::Draw(hbs::Circuit		&c)
{
  double		x;
  double		y;

  bunny_clear(&pic->buffer, BLACK);
  for (x = -win->buffer.width / pin_size / 2; x < win->buffer.width / pin_size / 2; ++x)
    for (y = -win->buffer.height / pin_size / 2; y < win->buffer.height / pin_size / 2; ++y)
      {
	Position	pos[2] =
	  {
	    {x + camera.x, y + camera.y},
	    {x + camera.x + 1.0 / pin_size, y + camera.y + 1.0 / pin_size}
	  };
	Line(pos[0], pos[1], GRAY(128));
      }
  c.Draw(*this);

  if (grabbed)
    {
      t_bunny_accurate_position npos =
	{
	  (double)bunny_get_mouse_position()->x,
	  (double)bunny_get_mouse_position()->y
	};

      npos.x -= grab_pos.x;
      npos.y -= grab_pos.y;
      npos.x /= pin_size;
      npos.y /= pin_size;
      npos.x = round(npos.x);
      npos.y = round(npos.y);
      grabbed->Move(npos);
      grabbed->Draw(*this);
      npos.x *= -1;
      npos.y *= -1;
      grabbed->Move(npos);
    }
  
  char			buffer[32];

  snprintf(&buffer[0], sizeof(buffer), "TIME %04d", (int)c.GetTime());

  int ps = pin_size;
  pin_size = 1;
  Text(Position{0.5 * pic->buffer.width - TextSize({20, 20}, buffer).x - 0.5, 4.0 - pic->buffer.height / 2}, {20, 20}, RED, buffer);
  Text(Position{0.5 * pic->buffer.width - TextSize({20, 20}, buffer).x + 0.0, 5.0 - pic->buffer.height / 2}, {20, 20}, RED, buffer);
  Text(Position{0.5 * pic->buffer.width - TextSize({20, 20}, buffer).x + 0.5, 6.0 - pic->buffer.height / 2}, {20, 20}, RED, buffer);
  pin_size = ps;
  
  bunny_blit(&win->buffer, pic, NULL);
  bunny_blit(&win->buffer, controls, NULL);
  bunny_blit(&win->buffer, author, NULL);
  bunny_display(win);
  return (true);
}
