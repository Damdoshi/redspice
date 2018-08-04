// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"Timer.hpp"

size_t			nts::Timer::GetTime(void) const
{
  return (cnt);
}

void			nts::Timer::Tick(void)
{
  cnt += 1;
}

nts::Timer::Timer(void)
  : cnt(0)
{}

nts::Timer::~Timer(void)
{}

