// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4014.hpp"

const std::string	&hbs::CMP_4014::GetType(void) const
{
  return (type = "4014");
}

static const size_t	parallel_pin[8] = {1, 4, 5, 6, 7, 13, 14, 15};

bool			hbs::CMP_4014::DrivesPin(size_t n) const
{
  return (n == 2 || n == 3 || n == 12);
}

void			hbs::CMP_4014::LoadParallel(void)
{
  for (size_t i = 0; i < 8; ++i)
    if (GetPin(parallel_pin[i]) == hbs::TRUE)
      value.set(i);
    else
      value.reset(i);
}

void			hbs::CMP_4014::Tick(void)
{
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(10) != hbs::TRUE || GetPrevious(10) != hbs::FALSE)
    return ;
  last_tick = timer.GetTime();
  if (GetPin(9) == hbs::TRUE)
    LoadParallel();
  else
    {
      value <<= 1;
      if (GetPin(11) == hbs::TRUE)
	value.set(0);
      else
	value.reset(0);
    }
}

hbs::Tristate		hbs::CMP_4014::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      Tick();
      if (n == 2)
	return (p->second = value[5] ? hbs::TRUE : hbs::FALSE);
      if (n == 3)
	return (p->second = value[6] ? hbs::TRUE : hbs::FALSE);
      return (p->second = value[7] ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4014::CMP_4014(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos),
    last_tick(0)
{}

hbs::CMP_4014::~CMP_4014(void)
{}

bool			hbs::CMP_4014::TypeMatches(const std::string &type)
{
  if (type == "4014")
    return (true);
  if (type == "74HC4014")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4014::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4014(timer, name, position));
}

std::string		hbs::CMP_4014::GetDisplayType(void) const
{
  return ("4014 8-stage parallel/serial-input serial-output shift register");
}
