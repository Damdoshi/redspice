// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"Timer.hpp"

size_t			hbs::Timer::GetTime(void) const
{
  return (cnt);
}

void			hbs::Timer::Tick(void)
{
  cnt += 1;
}

hbs::Timer::Timer(void)
  : cnt(0)
{}

hbs::Timer::~Timer(void)
{}

