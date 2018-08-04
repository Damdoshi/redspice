// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4011.hpp"

const std::string	&nts::CMP_4011::GetType(void) const
{
  return (type = typeid(*this).name());
}

nts::Tristate		nts::CMP_4011::Logic(nts::Tristate	a,
					     nts::Tristate	b) const
{
  switch (CMP_4081::Logic(a, b))
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

nts::CMP_4011::CMP_4011(const nts::Timer	&timer)
  : CMP_4081(timer)
{}
nts::CMP_4011::~CMP_4011(void)
{}

