// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"True.hpp"

const std::string	&hbs::True::GetType(void) const
{
  return (type = "True");
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::True::Compute(size_t			pin_num_this)
{
  if (pin_num_this != 1)
    throw hbs::BadPin(GetType() + ":Bad pin.");
  return (hbs::TRUE);
}

hbs::True::True(hbs::Timer		&timer,
		const std::string	&name,
		const std::string	&pos)
  : AComponent(timer, name, pos)
{}

hbs::True::~True(void)
{}

