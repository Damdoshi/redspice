// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4011.hpp"

const std::string	&hbs::CMP_4011::GetType(void) const
{
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::CMP_4011::Logic(hbs::Tristate	a,
					     hbs::Tristate	b) const
{
  switch (CMP_4081::Logic(a, b))
    {
    case hbs::TRUE:
      return (hbs::FALSE);
    case hbs::FALSE:
      return (hbs::TRUE);
    default:
      return (hbs::UNDEFINED);
    }
  return (hbs::UNDEFINED);
}

hbs::CMP_4011::CMP_4011(const hbs::Timer	&timer,
			const std::string	&pos)
  : CMP_4081(timer, pos)
{}
hbs::CMP_4011::~CMP_4011(void)
{}

