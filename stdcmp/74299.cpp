// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74299.hpp"

const std::string	&hbs::CMP_74299::GetType(void) const
{
  return (type = "74299");
}

static const size_t	io_pin[8] = {7, 13, 6, 14, 5, 15, 4, 16};

bool			hbs::CMP_74299::OutputEnabled(void)
{
  return (GetPin(2) == hbs::FALSE && GetPin(3) == hbs::FALSE);
}

bool			hbs::CMP_74299::DrivesPin(size_t n) const
{
  if (n == 8 || n == 17)
    return (true);
  for (size_t i = 0; i < 8; ++i)
    if (n == io_pin[i])
      return (true);
  return (false);
}

void			hbs::CMP_74299::Tick(void)
{
  hbs::Tristate		s0;
  hbs::Tristate		s1;

  if (GetPin(9) == hbs::FALSE)
    {
      value.reset();
      return ;
    }
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(12) != hbs::TRUE || GetPrevious(12) != hbs::FALSE)
    return ;
  last_tick = timer.GetTime();
  s0 = GetPin(1);
  s1 = GetPin(19);
  if (s0 == hbs::FALSE && s1 == hbs::FALSE)
    return ;
  if (s0 == hbs::TRUE && s1 == hbs::TRUE)
    {
      if (OutputEnabled())
	return ;
      for (size_t i = 0; i < 8; ++i)
	if (GetPin(io_pin[i]) == hbs::TRUE)
	  value.set(i);
	else
	  value.reset(i);
      return ;
    }
  if (s0 == hbs::TRUE && s1 == hbs::FALSE)
    {
      value <<= 1;
      if (GetPin(11) == hbs::TRUE)
	value.set(0);
      else
	value.reset(0);
      return ;
    }
  if (s0 == hbs::FALSE && s1 == hbs::TRUE)
    {
      value >>= 1;
      if (GetPin(18) == hbs::TRUE)
	value.set(7);
      else
	value.reset(7);
    }
}

hbs::Tristate		hbs::CMP_74299::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      Tick();
      if (n == 8)
	return (p->second = value[0] ? hbs::TRUE : hbs::FALSE);
      if (n == 17)
	return (p->second = value[7] ? hbs::TRUE : hbs::FALSE);
      if (!OutputEnabled())
	return (p->second = hbs::HIGH_IMPEDANCE);
      for (size_t i = 0; i < 8; ++i)
	if (n == io_pin[i])
	  return (p->second = value[i] ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 10 || n == 20)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74299::CMP_74299(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : AComponent(time, name, pos),
    last_tick(0)
{}

hbs::CMP_74299::~CMP_74299(void)
{}

bool			hbs::CMP_74299::TypeMatches(const std::string &type)
{
  if (type == "74299" || type == "74HC299" || type == "74HCT299" || type == "74LS299")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74299::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74299(timer, name, position));
}

std::string		hbs::CMP_74299::GetDisplayType(void) const
{
  return ("74299 8-bit universal shift/storage register with 3-state outputs");
}
