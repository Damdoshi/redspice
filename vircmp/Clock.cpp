// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"Clock.hpp"

const std::string	&nts::Clock::GetType(void) const
{
  return (type = typeid(*this).name());
}

nts::Tristate		nts::Clock::Compute(size_t			pin_num_this)
{
  std::map<size_t, PinState>::iterator	it;
  nts::Tristate		old;
  size_t		tim;

  if (pin_num_this != 1)
    throw nts::BadPin(GetType() + ":Bad pin.");
  tim = timer.GetTime();
  if ((it = timeline.find(tim)) == timeline.end())
    {
      old = value;
      if (value == nts::TRUE)
	value = nts::FALSE;
      else if (value == nts::FALSE)
	value = nts::TRUE;
      return (timeline[tim][1] = old);
    }
  return (timeline[tim][1]);
}

nts::Clock::Clock(nts::Timer		&timer,
		  const std::string	&value)
  : Input(timer, value)
{}

nts::Clock::~Clock(void)
{}

