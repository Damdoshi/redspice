// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4001.hpp"

const std::string	&hbs::CMP_4001::GetType(void) const
{
  return (type = "4001");
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::CMP_4001::Logic(hbs::Tristate	a,
					     hbs::Tristate	b) const
{
  switch (CMP_4071::Logic(a, b))
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

hbs::CMP_4001::CMP_4001(const hbs::Timer	&timer,
			const std::string	&pos)
  : CMP_4071(timer, pos)
{}
hbs::CMP_4001::~CMP_4001(void)
{}

