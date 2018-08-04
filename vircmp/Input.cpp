// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"Input.hpp"

const std::string	&nts::Input::GetType(void) const
{
  return (type = typeid(*this).name());
}

nts::Tristate		nts::Input::Compute(size_t			pin_num_this)
{
  if (pin_num_this != 1)
    throw nts::BadPin(GetType() + ":Bad pin.");
  return (value);
}

void			nts::Input::SetValue(nts::Tristate		tristate)
{
  value = tristate;
}

nts::Input::Input(nts::Timer		&timer,
		  const std::string	&val)
  : AComponent(timer),
    value(nts::UNDEFINED)
{
  if (val == "1")
    value = nts::TRUE;
  else if (val == "0")
    value = nts::FALSE;
}

nts::Input::~Input(void)
{}

