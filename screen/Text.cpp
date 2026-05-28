// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include	"Screen.hpp"

#define		SPACE_BETWEEN_LETTER			1.2

static void	single_letter(hbs::Screen		&scr,
			      hbs::Position		pos,
			      hbs::Size			siz,
			      unsigned int		color,
			      int			c,
			      int			ps,
			      hbs::Position		ori)
{
  const t_bunny_letter &l = gl_vector_font[c];

  for (int i = 0; i < l.nb_edge; ++i)
    {
      hbs::Position a, b;

      a.x = siz.x * l.edge[i].x0 / 20.0 + pos.x + ori.x * ps;
      a.y = siz.y * l.edge[i].y0 / 20.0 + pos.y + ori.y * ps;
      b.x = siz.x * l.edge[i].x1 / 20.0 + pos.x + ori.x * ps;
      b.y = siz.y * l.edge[i].y1 / 20.0 + pos.y + ori.y * ps;
      scr.Line(a, b, color);
    }
}

void		hbs::Screen::Text(Position		pos,
				  Size			siz,
				  unsigned int		color,
				  const std::string	&str)
{
  Position	ori = pos;
  int		ps = pin_size;
  char		c;

  pin_size = 1;
  pos.x = 0;
  pos.y = 0;
  for (size_t i = 0; str[i]; ++i)
    {
      if (isalpha(c = tolower(str[i])))
	c = c - 'a';
      else if (c >= '0' && c <= '9')
	c = c - '0' + BFT_0;
      else if (c == '!')
	c = BFT_EXCLAMATION;
      else if (c == '/')
	c = BFT_SLASH;
      else if (c == '.')
	c = BFT_DOT;
      else
	c = -1;

      if (c != -1)
	single_letter(*this, pos, siz, color, c, ps, ori);

      pos.x += siz.x * SPACE_BETWEEN_LETTER;
    }
  pin_size = ps;
}

hbs::Size hbs::Screen::TextSize(Size			s,
				const std::string	&str)
{
  return (Size{str.size() * s.x * SPACE_BETWEEN_LETTER - s.x * (SPACE_BETWEEN_LETTER - 1.0), s.y});
}

