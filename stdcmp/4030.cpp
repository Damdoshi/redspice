// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4030.hpp"

const std::string	&hbs::CMP_4030::GetType(void) const
{
  return (type = "4030");
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::CMP_4030::Logic(hbs::Tristate	a,
					     hbs::Tristate	b) const
{
  if (a == hbs::TRUE && b == hbs::FALSE)
    return (hbs::TRUE);
  if (b == hbs::TRUE && a == hbs::FALSE)
    return (hbs::TRUE);
  if (a == hbs::UNDEFINED || b == hbs::UNDEFINED)
    return (hbs::UNDEFINED);
  return (hbs::FALSE);
}

hbs::CMP_4030::CMP_4030(const hbs::Timer	&timer,
			const std::string	&name,
			const std::string	&pos)
  : AGate(timer, name, pos)
{}
hbs::CMP_4030::~CMP_4030(void)
{}


bool			hbs::CMP_4030::TypeMatches(const std::string	&type)
{
  if (type == "4030")
    return (true);
  if (type == "4070")
    return (true);
  if (type == "7486")
    return (true);
  if (type == "74HC86")
    return (true);
  if (type == "74HCT86")
    return (true);
  if (type == "74LS86")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4030::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4030(timer, name, position));
}

std::string		hbs::CMP_4030::GetDisplayType(void) const
{
  return ("4030/4070/7486 Quad 2-input XOR");
}
