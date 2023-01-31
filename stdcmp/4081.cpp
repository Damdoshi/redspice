// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4081.hpp"

const std::string	&hbs::CMP_4081::GetType(void) const
{
  return (type = "4081");
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::CMP_4081::Logic(hbs::Tristate	a,
					     hbs::Tristate	b) const
{
  if (a == hbs::FALSE || b == hbs::FALSE)
    return (hbs::FALSE);
  if (a == hbs::UNDEFINED || b == hbs::UNDEFINED)
    return (hbs::UNDEFINED);
  return (hbs::TRUE);
}

hbs::CMP_4081::CMP_4081(const hbs::Timer	&timer,
			const std::string	&pos)
  : AGate(timer, pos)
{}
hbs::CMP_4081::~CMP_4081(void)
{}

