// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4001.hpp"

const std::string	&nts::CMP_4001::GetType(void) const
{
  return (type = typeid(*this).name());
}

nts::Tristate		nts::CMP_4001::Logic(nts::Tristate	a,
					     nts::Tristate	b) const
{
  switch (CMP_4071::Logic(a, b))
    {
    case nts::TRUE:
      return (nts::FALSE);
    case nts::FALSE:
      return (nts::TRUE);
    default:
      return (nts::UNDEFINED);
    }
  return (nts::UNDEFINED);
}

nts::CMP_4001::CMP_4001(const nts::Timer	&timer)
  : CMP_4071(timer)
{}
nts::CMP_4001::~CMP_4001(void)
{}

