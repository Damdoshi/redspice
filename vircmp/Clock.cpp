// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"Clock.hpp"

const std::string	&hbs::Clock::GetType(void) const
{
  return (type = "Clock");
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::Clock::Compute(size_t			pin_num_this)
{
  std::map<size_t, PinState>::iterator	it;
  hbs::Tristate		old;
  size_t		tim;

  if (pin_num_this != 1)
    throw hbs::BadPin(GetType() + ":Bad pin.");
  tim = timer.GetTime();
  if ((it = timeline.find(tim)) == timeline.end())
    {
      old = value;
      if (value == hbs::TRUE)
	value = hbs::FALSE;
      else if (value == hbs::FALSE)
	value = hbs::TRUE;
      return (timeline[tim][1] = old);
    }
  return (timeline[tim][1]);
}

hbs::Clock::Clock(hbs::Timer		&timer,
		  const std::string	&value,
		  const std::string	&pos)
  : Input(timer, value, pos)
{}

hbs::Clock::~Clock(void)
{}

