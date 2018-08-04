// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"True.hpp"

const std::string	&nts::True::GetType(void) const
{
  return (type = typeid(*this).name());
}

nts::Tristate		nts::True::Compute(size_t			pin_num_this)
{
  if (pin_num_this != 1)
    throw nts::BadPin(GetType() + ":Bad pin.");
  return (nts::TRUE);
}

nts::True::True(nts::Timer		&timer)
  : AComponent(timer)
{}

nts::True::~True(void)
{}

