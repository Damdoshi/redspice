// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include	"Screen.hpp"
#include        <algorithm>
#include        <cmath>

void		hbs::Screen::Line(Position		a,
				  Position		b,
				  unsigned int		color)
{
  a.x -= camera.x;
  b.x -= camera.x;
  a.y -= camera.y;
  b.y -= camera.y;
  t_bunny_position pos[2] =
    {
     {(int)(a.x * pin_size), (int)(a.y * pin_size)},
     {(int)(b.x * pin_size), (int)(b.y * pin_size)}
    };
  unsigned int	col[2] = {color, color};

  pos[0].x += pic->buffer.width / 2;
  pos[0].y += pic->buffer.height / 2;
  pos[1].x += pic->buffer.width / 2;
  pos[1].y += pic->buffer.height / 2;
  bunny_set_line(&pic->buffer, pos, col);
}

void hbs::Screen::DashedLine(Position a, Position b, unsigned int color)
{
  double sx = (a.x - camera.x) * pin_size + pic->buffer.width / 2;
  double sy = (a.y - camera.y) * pin_size + pic->buffer.height / 2;
  double dx = (b.x - a.x) * pin_size, dy = (b.y - a.y) * pin_size;
  double length = std::hypot(dx, dy);
  if (!std::isfinite(sx) || !std::isfinite(sy) || !std::isfinite(length) || length < .5) return;
  dx /= length; dy /= length;
  double from = 0, to = length;
  const double origins[] = {sx, sy}, directions[] = {dx, dy};
  const double limits[] = {double(pic->buffer.width) + 1, double(pic->buffer.height) + 1};
  // Clip before splitting into dashes: distant off-screen endpoints must not
  // cause millions of draw calls or an overflowing screen coordinate.
  for (unsigned axis = 0; axis < 2; ++axis)
    {
      double origin = origins[axis], direction = directions[axis];
      if (std::abs(direction) < 1e-12)
        {
          if (origin < -1 || origin > limits[axis]) return;
          continue;
        }
      double low = (-1 - origin) / direction, high = (limits[axis] - origin) / direction;
      if (low > high) std::swap(low, high);
      from = std::max(from, low); to = std::min(to, high);
    }
  if (from >= to) return;
  sx += dx * from; sy += dy * from;
  unsigned int colors[] = {color, color};
  for (double dash = -std::fmod(from, 9.0); dash < to - from; dash += 9)
    {
      double start = std::max(0.0, dash), end = std::min(to - from, dash + 5);
      if (start >= end) continue;
      t_bunny_position points[] = {
        {int(sx + dx * start), int(sy + dy * start)},
        {int(sx + dx * end), int(sy + dy * end)}
      };
      bunny_set_line(&pic->buffer, points, colors);
    }
}
