// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4518.hpp"

const std::string	&hbs::CMP_4518::GetType(void) const
{
  return (type = "4518");
}

bool			hbs::CMP_4518::DrivesPin(size_t n) const
{
  return ((n >= 3 && n <= 6) || (n >= 11 && n <= 14));
}

void			hbs::CMP_4518::UpdateCounter(size_t index)
{
  size_t		clock;
  size_t		enable;
  size_t		reset;
  bool			increment;

  if (index > 1 || last_tick[index] == timer.GetTime())
    return ;
  if (index == 0)
    {
      clock = 1;
      enable = 2;
      reset = 7;
    }
  else
    {
      clock = 9;
      enable = 10;
      reset = 15;
    }
  if (GetPin(reset) == hbs::TRUE)
    {
      value[index] = 0;
      last_tick[index] = timer.GetTime();
      return ;
    }
  if (GetPin(reset) != hbs::FALSE)
    return ;
  increment = false;
  if (GetPin(enable) == hbs::TRUE && GetPin(clock) == hbs::TRUE && GetPrevious(clock) == hbs::FALSE)
    increment = true;
  if (GetPin(clock) == hbs::FALSE && GetPin(enable) == hbs::FALSE && GetPrevious(enable) == hbs::TRUE)
    increment = true;
  if (increment)
    {
      value[index] = (value[index] + 1) % 10;
      last_tick[index] = timer.GetTime();
    }
}

hbs::Tristate		hbs::CMP_4518::OutputBit(size_t n) const
{
  size_t		index;
  size_t		base;
  size_t		bit;

  if (n >= 3 && n <= 6)
    {
      index = 0;
      base = 3;
    }
  else if (n >= 11 && n <= 14)
    {
      index = 1;
      base = 11;
    }
  else
    return (hbs::UNDEFINED);
  bit = n - base;
  return (((value[index] >> bit) & 1) ? hbs::TRUE : hbs::FALSE);
}

hbs::Tristate		hbs::CMP_4518::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n >= 3 && n <= 6)
    {
      UpdateCounter(0);
      return (p->second = OutputBit(n));
    }
  if (n >= 11 && n <= 14)
    {
      UpdateCounter(1);
      return (p->second = OutputBit(n));
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4518::CMP_4518(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos)
{
  value[0] = 0;
  value[1] = 0;
  last_tick[0] = 0;
  last_tick[1] = 0;
}

hbs::CMP_4518::~CMP_4518(void)
{}

bool			hbs::CMP_4518::TypeMatches(const std::string &type)
{
  if (type == "4518")
    return (true);
  if (type == "74HC4518")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4518::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4518(timer, name, position));
}

std::string		hbs::CMP_4518::GetDisplayType(void) const
{
  return ("4518 Dual BCD up-counter");
}
