// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"False.hpp"

const std::string	&hbs::False::GetType(void) const
{
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::False::Compute(size_t			pin_num_this)
{
  if (pin_num_this != 1)
    throw hbs::BadPin(GetType() + ":Bad pin.");
  return (hbs::FALSE);
}

hbs::False::False(hbs::Timer		&timer)
  : AComponent(timer)
{}

hbs::False::~False(void)
{}

