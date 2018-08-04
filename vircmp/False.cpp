// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"False.hpp"

const std::string	&nts::False::GetType(void) const
{
  return (type = typeid(*this).name());
}

nts::Tristate		nts::False::Compute(size_t			pin_num_this)
{
  if (pin_num_this != 1)
    throw nts::BadPin(GetType() + ":Bad pin.");
  return (nts::FALSE);
}

nts::False::False(nts::Timer		&timer)
  : AComponent(timer)
{}

nts::False::~False(void)
{}

