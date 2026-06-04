// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"Circuit.hpp"

namespace		hbs
{
  void			Simulate(hbs::Circuit		&circuit,
				 hbs::Timer		&timer,
				 size_t			ticks)
  {
    size_t		i;

    while (ticks--)
      {
	timer.Tick();
	for (i = 1; i <= circuit.GetOutputNum(); ++i)
	  circuit.Compute(i);
      }
  }
}
