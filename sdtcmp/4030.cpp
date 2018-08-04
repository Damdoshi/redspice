// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4030.hpp"

const std::string	&nts::CMP_4030::GetType(void) const
{
  return (type = typeid(*this).name());
}

nts::Tristate		nts::CMP_4030::Logic(nts::Tristate	a,
					     nts::Tristate	b) const
{
  if (a == nts::TRUE && b == nts::FALSE)
    return (nts::TRUE);
  if (b == nts::TRUE && a == nts::FALSE)
    return (nts::TRUE);
  if (a == nts::UNDEFINED || b == nts::UNDEFINED)
    return (nts::UNDEFINED);
  return (nts::FALSE);
}

nts::CMP_4030::CMP_4030(const nts::Timer	&timer)
  : AGate(timer)
{}
nts::CMP_4030::~CMP_4030(void)
{}

