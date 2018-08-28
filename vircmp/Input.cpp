// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"Input.hpp"

const std::string	&hbs::Input::GetType(void) const
{
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::Input::Compute(size_t			pin_num_this)
{
  if (pin_num_this != 1)
    throw hbs::BadPin(GetType() + ":Bad pin.");
  return (value);
}

void			hbs::Input::SetValue(hbs::Tristate		tristate)
{
  value = tristate;
}

hbs::Input::Input(hbs::Timer		&timer,
		  const std::string	&val)
  : AComponent(timer),
    value(hbs::UNDEFINED)
{
  if (val == "1")
    value = hbs::TRUE;
  else if (val == "0")
    value = hbs::FALSE;
}

hbs::Input::~Input(void)
{}

