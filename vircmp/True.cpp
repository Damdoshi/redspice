// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"True.hpp"

const std::string	&hbs::True::GetType(void) const
{
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::True::Compute(size_t			pin_num_this)
{
  if (pin_num_this != 1)
    throw hbs::BadPin(GetType() + ":Bad pin.");
  return (hbs::TRUE);
}

hbs::True::True(hbs::Timer		&timer)
  : AComponent(timer)
{}

hbs::True::~True(void)
{}

