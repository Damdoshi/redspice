// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"Input.hpp"

const std::string	&hbs::Input::GetType(void) const
{
  return (type = "In");
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::Input::Compute(size_t			pin_num_this)
{
  if (pin_num_this != 1)
    throw hbs::BadPin(GetType() + ":Bad pin.");
  return (value);
}


std::string		hbs::Input::GetDefinitionValue(void) const
{
  if (value == hbs::TRUE)
    return ("1");
  if (value == hbs::FALSE)
    return ("0");
  return ("");
}

void			hbs::Input::SetValue(hbs::Tristate		tristate)
{
  if (timeline.rbegin() == timeline.rend())
    timeline[0][1] = tristate;
  else
    timeline.rbegin()->second[1] = tristate;
  value = tristate;
}

hbs::Input::Input(hbs::Timer		&timer,
		  const std::string	&name,
		  const std::string	&val,
		  const std::string	&pos)
  : AComponent(timer, name, pos),
    value(hbs::UNDEFINED)
{
  if (val == "1")
    value = hbs::TRUE;
  else if (val == "0")
    value = hbs::FALSE;
}

hbs::Input::~Input(void)
{}

