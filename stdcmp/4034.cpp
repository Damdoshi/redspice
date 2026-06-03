// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4034.hpp"

const std::string	&hbs::CMP_4034::GetType(void) const
{
  return (type = "4034");
}

static const size_t	io_pin[8] = {2, 3, 4, 5, 20, 21, 22, 23};

bool			hbs::CMP_4034::OutputEnabled(void)
{
  return (GetPin(1) == hbs::FALSE);
}

bool			hbs::CMP_4034::DrivesPin(size_t n) const
{
  if (n == 10 || n == 15)
    return (true);
  for (size_t i = 0; i < 8; ++i)
    if (n == io_pin[i])
      return (true);
  return (false);
}

void			hbs::CMP_4034::Tick(void)
{
  if (GetPin(14) == hbs::TRUE)
    {
      value.reset();
      return ;
    }
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(13) != hbs::TRUE || GetPrevious(13) != hbs::FALSE)
    return ;
  last_tick = timer.GetTime();
  if (GetPin(12) == hbs::TRUE)
    {
      for (size_t i = 0; i < 8; ++i)
	if (GetPin(io_pin[i]) == hbs::TRUE)
	  value.set(i);
	else
	  value.reset(i);
      return ;
    }
  if (GetPin(11) == hbs::TRUE)
    {
      value <<= 1;
      if (GetPin(9) == hbs::TRUE)
	value.set(0);
      else
	value.reset(0);
    }
  else
    {
      value >>= 1;
      if (GetPin(16) == hbs::TRUE)
	value.set(7);
      else
	value.reset(7);
    }
}

hbs::Tristate		hbs::CMP_4034::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      Tick();
      if (n == 10)
	return (p->second = value[0] ? hbs::TRUE : hbs::FALSE);
      if (n == 15)
	return (p->second = value[7] ? hbs::TRUE : hbs::FALSE);
      if (!OutputEnabled())
	return (p->second = hbs::HIGH_IMPEDANCE);
      for (size_t i = 0; i < 8; ++i)
	if (n == io_pin[i])
	  return (p->second = value[i] ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 8 || n == 24)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4034::CMP_4034(const hbs::Timer	&time,
				const std::string	&name,
				const std::string	&pos)
  : AComponent(time, name, pos),
    last_tick(0)
{}

hbs::CMP_4034::~CMP_4034(void)
{}

bool			hbs::CMP_4034::TypeMatches(const std::string &type)
{
  if (type == "4034" || type == "74HC4034" || type == "74HCT4034")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4034::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4034(timer, name, position));
}

std::string		hbs::CMP_4034::GetDisplayType(void) const
{
  return ("4034 8-bit bidirectional bus register");
}
