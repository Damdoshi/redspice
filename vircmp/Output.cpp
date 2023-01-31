// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"Output.hpp"

const std::string	&hbs::Output::GetType(void) const
{
  return (type = "Out");
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::Output::Compute(size_t		pin)
{
  /// Check if it already computed (Maybe temporary...)
  hbs::Tristate		tri;

  if (AlreadyComputed(pin, tri))
    return (tri);

  /// Create an undefined state that will be set
  PresetOutput(pin);
  return (GetPin(pin));
}

bool			hbs::Output::Displayable(void) const
{
  return (true);
}

hbs::Output::Output(const hbs::Timer		&timer,
		    const std::string		&pos)
  : AComponent(timer, pos)
{}

hbs::Output::~Output(void)
{}

