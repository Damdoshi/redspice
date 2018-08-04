// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4069.hpp"

const std::string	&nts::CMP_4069::GetType(void) const
{
  return (type = typeid(*this).name());
}

static nts::Tristate	Reverse(nts::Tristate			n)
{
  if (n == nts::TRUE)
    return (nts::FALSE);
  if (n == nts::FALSE)
    return (nts::TRUE);
  return (n);
}

nts::Tristate		nts::CMP_4069::Compute(size_t		n)
{
  /// Check if it already computed (Maybe temporary...)
  nts::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);

  /// Create an undefined state that will be set
  Preset		p = PresetOutput(n);

  if (n == 2)
    return (p->second = Reverse(GetPin(1)));
  if (n == 4)
    return (p->second = Reverse(GetPin(3)));
  if (n == 6)
    return (p->second = Reverse(GetPin(5)));
  if (n == 8)
    return (p->second = Reverse(GetPin(9)));
  if (n == 10)
    return (p->second = Reverse(GetPin(11)));
  if (n == 12)
    return (p->second = Reverse(GetPin(13)));
  if (n == 7 || n == 14)
    return (nts::UNDEFINED);
  return (GetPin(n));
}

nts::CMP_4069::CMP_4069(const nts::Timer	&tim)
  : AComponent(tim)
{}

nts::CMP_4069::~CMP_4069(void)
{}

