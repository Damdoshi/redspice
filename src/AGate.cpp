// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"AGate.hpp"

hbs::Tristate		hbs::AGate::Compute(size_t		n)
{
  /// Check if it already computed (Maybe temporary...)
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);

  /// Create an undefined state that will be set
  Preset		itx = PresetOutput(n);

  if (n == 3)
    return (itx->second = Logic(GetPin(1), GetPin(2)));
  if (n == 4)
    return (itx->second = Logic(GetPin(5), GetPin(6)));
  if (n == 10)
    return (itx->second = Logic(GetPin(8), GetPin(9)));
  else if (n == 11)
    return (itx->second = Logic(GetPin(12), GetPin(13)));
  else if (n == 7 || n == 14)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::AGate::AGate(const hbs::Timer	&_timer)
  : AComponent(_timer)
{}

hbs::AGate::~AGate(void)
{}

