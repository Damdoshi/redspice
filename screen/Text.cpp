// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include	"Screen.hpp"

static void	single_letter(hbs::Screen		&scr,
			      hbs::Screen::Position	pos,
			      hbs::Screen::Size		siz,
			      unsigned int		color,
			      int			c)
{
  const t_bunny_letter &l = gl_vector_font[c];

  for (int i = 0; i < l.nb_edge; ++i)
    {
      hbs::Screen::Position a, b;

      a.x = siz.x * l.edge[i].x0 / 20.0 + pos.x;
      a.y = siz.y * l.edge[i].y0 / 20.0 + pos.y;
      b.x = siz.x * l.edge[i].x1 / 20.0 + pos.x;
      b.y = siz.y * l.edge[i].y1 / 20.0 + pos.y;
      scr.Line(a, b, color);
    }
}

void		hbs::Screen::Text(Position		pos,
				  Size			siz,
				  unsigned int		color,
				  const std::string	&str)
{
  char		c;

  for (size_t i = 0; str[i]; ++i)
    {
      if (isalpha(c = tolower(str[i])))
	c = c - 'a';
      else if (isdigit(c))
	c = c - '0' + BFT_0;
      else if (c == '!')
	c = BFT_EXCLAMATION;
      else if (c == '/')
	c = BFT_SLASH;
      else if (c == '.')
	c = BFT_DOT;
      else
	c = ' ';

      single_letter(*this, pos, siz, color, c);

      pos.x += siz.x * 1.1;
    }
}

hbs::Screen::Size hbs::Screen::TextSize(Size			s,
					const std::string	&str)
{
  return (Size{str.size() * s.x * 1.1 - s.x * 0.1, s.y});
}

