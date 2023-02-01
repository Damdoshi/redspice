// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"AComponent.hpp"

hbs::Link::Link(const hbs::Screen::Position	&from,
		const hbs::Screen::Position	&to,
		IComponent			*icom,
		size_t				pin,
		const std::string		&str,
		bool				rev)
  : first(icom),
    second(pin)
{
  int			i, j;
  hbs::Screen::Position	pos;
  size_t		tmp = 0;
  Layer			layer = BOTTOM;

  i = j = 0;
  if (str != "")
    {
      ReadWhitespace(str, i);
      if (!ReadText(str, i, "["))
	throw hbs::SyntaxError(&str[i]);
    }
  while (str[i] != '\0' && str[i] != ']')
    {
      ReadWhitespace(str, i);
      if (ReadText(str, i, "t"))
	layer = TOP;
      else if (ReadText(str, i, "b"))
	layer = BOTTOM;
      ReadWhitespace(str, i);
      j = i;
      ReadChar(str, i);
      ReadWhitespace(str, i);
      if (!ReadText(str, i, ","))
	throw SyntaxError(&str[i]);
      ReadWhitespace(str, i);
      pos.x = std::stof(&str[j], &tmp);
      if (tmp == 0)
	throw SyntaxError(&str[i]);
      j = i;
      ReadWhitespace(str, i);
      ReadChar(str, i);
      pos.y = std::stof(&str[j], &tmp);
      if (tmp == 0)
	throw SyntaxError(&str[i]);
      j = i;
      ReadWhitespace(str, i);

      if (!rev)
	third.push_back({pos, layer});
      else
	third.push_front({pos, layer});
    }
  if (!rev)
    {
      third.push_front({from, BOTTOM});
      third.push_back({to, BOTTOM});
    }
  else
    {
      third.push_front({to, BOTTOM});
      third.push_back({from, BOTTOM});
    }
}

