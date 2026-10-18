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
    while (ticks--)
      {
	timer.Tick();
	circuit.Compute();
      }
  }
}
