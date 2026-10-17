// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"Link.hpp"
#include		"Parsing.hpp"
#include		"Exception.hpp"

hbs::Link::Link(const hbs::Position		&from,
		const hbs::Position		&to,
		IComponent			*icom,
		size_t				pin,
		const std::string		&str,
		bool				rev)
  : first(icom),
    second(pin)
{
  int i;
  hbs::Position		pos;
  Layer			layer = BOTTOM;

  i = 0;
  if (str != "")
    {
      ReadWhitespace(str, i);
      if (!ReadText(str, i, "["))
	throw hbs::SyntaxError(&str[i]);
    }
  while (str[i] != '\0' && str[i] != ']')
    {
      ReadWhitespace(str, i);
      if (str[i] == ']')
        break;
      if (ReadText(str, i, "t"))
	layer = TOP;
      else if (ReadText(str, i, "b"))
	layer = BOTTOM;
      if (!ReadNumber(str, i, pos.x))
        throw SyntaxError("Invalid link coordinate.");
      ReadWhitespace(str, i);
      if (!ReadText(str, i, ","))
	throw SyntaxError(&str[i]);
      if (!ReadNumber(str, i, pos.y))
        throw SyntaxError("Invalid link coordinate.");
      ReadWhitespace(str, i);

      if (!rev)
	third.push_back({pos, layer});
      else
	third.push_front({pos, layer});
    }
  if (!str.empty())
    {
      if (!ReadText(str, i, "]"))
        throw SyntaxError("Missing ']'.");
      ReadWhitespace(str, i);
      if ((size_t)i != str.size())
        throw SyntaxError("Unexpected link geometry suffix.");
    }
  (void)from;
  (void)to;
  /*
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
  */
}

