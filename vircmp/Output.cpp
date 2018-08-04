// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"Output.hpp"

const std::string	&nts::Output::GetType(void) const
{
  return (type = typeid(*this).name());
}

nts::Tristate		nts::Output::Compute(size_t		pin)
{
  /// Check if it already computed (Maybe temporary...)
  nts::Tristate		tri;

  if (AlreadyComputed(pin, tri))
    return (tri);

  /// Create an undefined state that will be set
  PresetOutput(pin);
  return (GetPin(pin));
}

bool			nts::Output::Displayable(void) const
{
  return (true);
}

nts::Output::Output(const nts::Timer		&timer)
  : AComponent(timer)
{}

nts::Output::~Output(void)
{}

