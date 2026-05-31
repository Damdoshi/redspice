// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4071.hpp"

const std::string	&hbs::CMP_4071::GetType(void) const
{
  return (type = "4071");
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::CMP_4071::Logic(hbs::Tristate	a,
					     hbs::Tristate	b) const
{
  if (a == hbs::TRUE || b == hbs::TRUE)
    return (hbs::TRUE);
  if (a == hbs::UNDEFINED || b == hbs::UNDEFINED)
    return (hbs::UNDEFINED);
  return (hbs::FALSE);
}

hbs::CMP_4071::CMP_4071(const hbs::Timer	&timer,
			const std::string	&name,
			const std::string	&pos)
  : AGate(timer, name, pos)
{}
hbs::CMP_4071::~CMP_4071(void)
{}


bool			hbs::CMP_4071::TypeMatches(const std::string	&type)
{
  if (type == "4071")
    return (true);
  if (type == "7432")
    return (true);
  if (type == "74HC32")
    return (true);
  if (type == "74HCT32")
    return (true);
  if (type == "74LS32")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4071::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4071(timer, name, position));
}

std::string		hbs::CMP_4071::GetDisplayType(void) const
{
  return ("4071/7432 Quad 2-input OR");
}
