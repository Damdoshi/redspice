// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"AComponent.hpp"

hbs::Link::Link(IComponent		*icom,
		size_t			pin,
		const std::string	&str)
  : first(icom),
    second(pin)
{
  int			i, j;
  hbs::Screen::Position	pos;
  size_t		tmp = 0;
  Layer			layer;

  i = j = 0;
  while (str[i] != '\0')
    {
      ReadWhitespace(str, i);
      if (!ReadText(str, i, "("))
	throw hbs::SyntaxError(str);
      ReadWhitespace(str, i);
      j = i;
      ReadChar(str, i);
      ReadWhitespace(str, i);
      if (!ReadText(str, i, ")"))
	throw SyntaxError(str);
      ReadWhitespace(str, i);
      pos.x = std::stof(&str[j], &tmp);
      if (tmp == 0)
	throw SyntaxError(str);
      j = i;
      ReadWhitespace(str, i);
      ReadChar(str, i);
      ReadWhitespace(str, i);
      pos.y = std::stof(&str[j], &tmp);
      ReadWhitespace(str, i);
      if (tmp == 0)
	throw SyntaxError(str);
      if (!ReadText(str, i, ","))
	throw SyntaxError(str);
      ReadWhitespace(str, i);
      if (ReadText(str, i, "t"))
	layer = TOP;
      else if (ReadText(str, i, "b"))
	layer = BOTTOM;
      else
	throw SyntaxError(str);
      ReadWhitespace(str, i);

      if (!ReadText(str, i, ")"))
	throw SyntaxError(str);
      ReadWhitespace(str, i);
      if (str[i] && !ReadText(str, i, ","))
	throw SyntaxError(str);
      ReadWhitespace(str, i);

      third.push_back({pos, layer});
    }
}

