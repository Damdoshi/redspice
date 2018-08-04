// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4071.hpp"

const std::string	&nts::CMP_4071::GetType(void) const
{
  return (type = typeid(*this).name());
}

nts::Tristate		nts::CMP_4071::Logic(nts::Tristate	a,
					     nts::Tristate	b) const
{
  if (a == nts::TRUE || b == nts::TRUE)
    return (nts::TRUE);
  if (a == nts::UNDEFINED || b == nts::UNDEFINED)
    return (nts::UNDEFINED);
  return (nts::FALSE);
}

nts::CMP_4071::CMP_4071(const nts::Timer	&timer)
  : AGate(timer)
{}
nts::CMP_4071::~CMP_4071(void)
{}

