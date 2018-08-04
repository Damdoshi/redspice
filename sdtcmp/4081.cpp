// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4081.hpp"

const std::string	&nts::CMP_4081::GetType(void) const
{
  return (type = typeid(*this).name());
}

nts::Tristate		nts::CMP_4081::Logic(nts::Tristate	a,
					     nts::Tristate	b) const
{
  if (a == nts::FALSE || b == nts::FALSE)
    return (nts::FALSE);
  if (a == nts::UNDEFINED || b == nts::UNDEFINED)
    return (nts::UNDEFINED);
  return (nts::TRUE);
}

nts::CMP_4081::CMP_4081(const nts::Timer	&timer)
  : AGate(timer)
{}
nts::CMP_4081::~CMP_4081(void)
{}

