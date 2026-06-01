// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4510.hpp"

const std::string	&hbs::CMP_4510::GetType(void) const
{
  return (type = "4510");
}

static bool		known_low(hbs::Tristate t)
{
  return (t == hbs::FALSE);
}

static bool		known_high(hbs::Tristate t)
{
  return (t == hbs::TRUE);
}

bool			hbs::CMP_4510::DrivesPin(size_t n) const
{
  return (n == 2 || n == 6 || n == 7 || n == 11 || n == 14);
}

void			hbs::CMP_4510::UpdateCounter(void)
{
  bool			up;

  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(9) == hbs::TRUE)
    {
      value = 0;
      last_tick = timer.GetTime();
      return ;
    }
  if (GetPin(1) == hbs::TRUE)
    {
      value = 0;
      if (GetPin(4) == hbs::TRUE)
	value |= 1 << 0;
      if (GetPin(12) == hbs::TRUE)
	value |= 1 << 1;
      if (GetPin(13) == hbs::TRUE)
	value |= 1 << 2;
      if (GetPin(3) == hbs::TRUE)
	value |= 1 << 3;
      value %= 10;
      last_tick = timer.GetTime();
      return ;
    }
  if (!known_low(GetPin(5)))
    return ;
  if (!(GetPin(15) == hbs::TRUE && GetPrevious(15) == hbs::FALSE))
    return ;
  up = known_high(GetPin(10));
  if (up)
    value = (value >= 9) ? 0 : value + 1;
  else
    value = (value == 0) ? 9 : value - 1;
  last_tick = timer.GetTime();
}

hbs::Tristate		hbs::CMP_4510::OutputBit(size_t n) const
{
  unsigned int		bit;

  if (n == 6)
    bit = 0;
  else if (n == 11)
    bit = 1;
  else if (n == 14)
    bit = 2;
  else if (n == 2)
    bit = 3;
  else
    return (hbs::UNDEFINED);
  return (((value >> bit) & 1) ? hbs::TRUE : hbs::FALSE);
}

hbs::Tristate		hbs::CMP_4510::CarryOut(void)
{
  bool			up;

  up = known_high(GetPin(10));
  if (!known_low(GetPin(5)))
    return (hbs::TRUE);
  if (up && value == 9)
    return (hbs::FALSE);
  if (!up && value == 0)
    return (hbs::FALSE);
  return (hbs::TRUE);
}

hbs::Tristate		hbs::CMP_4510::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      UpdateCounter();
      if (n == 7)
	return (p->second = CarryOut());
      return (p->second = OutputBit(n));
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4510::CMP_4510(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos),
    value(0),
    last_tick(0)
{}

hbs::CMP_4510::~CMP_4510(void)
{}

bool			hbs::CMP_4510::TypeMatches(const std::string &type)
{
  if (type == "4510")
    return (true);
  if (type == "74HC4510")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4510::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4510(timer, name, position));
}

std::string		hbs::CMP_4510::GetDisplayType(void) const
{
  return ("4510 Presettable BCD up-down counter");
}
