// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include	"Screen.hpp"

void		hbs::Screen::Square(Position		pos,
				    Size		siz,
				    unsigned int	color,
				    bool		full)
{
  pos.x -= camera.x;
  pos.y -= camera.y;
  pos.x *= pin_size;
  pos.y *= pin_size;
  siz.x *= pin_size;
  siz.y *= pin_size;
  if (full)
    {
      t_bunny_position	poss[2][3] =
	{
	 {
	  {(int)pos.x, (int)pos.y},
	  {(int)pos.x + (int)siz.x, (int)pos.y},
	  {(int)pos.x, (int)pos.y + (int)siz.y}
	 },
	 {
	  {(int)pos.x + (int)siz.x, (int)pos.y},
	  {(int)pos.x, (int)pos.y + (int)siz.y},
	  {(int)pos.x + (int)siz.x, (int)pos.y + (int)siz.y
	  }
	 },
	};
      unsigned int col[3] = {color, color, color};

      for (int i = 0; i < 3; ++i)
	{
	  poss[0][i].x += pic->buffer.width / 2;
	  poss[0][i].y += pic->buffer.height / 2;
	  poss[1][i].x += pic->buffer.width / 2;
	  poss[1][i].y += pic->buffer.height / 2;
	}
      bunny_set_polygon(&pic->buffer, &poss[0][0], col);
      bunny_set_polygon(&pic->buffer, &poss[1][0], col);
    }
  else
    {
      t_bunny_position	poss[4][2] =
	{
	 {{(int)pos.x, (int)pos.y}, {(int)pos.x + (int)siz.x, (int)pos.y}},
	 {{(int)pos.x, (int)pos.y + (int)siz.y}, {(int)pos.x + (int)siz.x, (int)pos.y + (int)siz.y}},
	 {{(int)pos.x, (int)pos.y}, {(int)pos.x, (int)pos.y + (int)siz.y}},
	 {{(int)pos.x + (int)siz.x, (int)pos.y}, {(int)pos.x + (int)siz.x, (int)pos.y + (int)siz.y}},
	};
      unsigned int col[2] = {color, color};

      for (int i = 0; i < 4; ++i)
	{
	  poss[i][0].x += pic->buffer.width / 2;
	  poss[i][0].y += pic->buffer.height / 2;
	  poss[i][1].x += pic->buffer.width / 2;
	  poss[i][1].y += pic->buffer.height / 2;
	}
      bunny_set_line(&pic->buffer, &poss[0][0], col);
      bunny_set_line(&pic->buffer, &poss[1][0], col);
      bunny_set_line(&pic->buffer, &poss[2][0], col);
      bunny_set_line(&pic->buffer, &poss[3][0], col);
    }
}
