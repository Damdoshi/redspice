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

