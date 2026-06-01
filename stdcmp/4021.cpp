// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4021.hpp"

const std::string	&hbs::CMP_4021::GetType(void) const
{
  return (type = "4021");
}

static const size_t	parallel_pin[8] = {1, 4, 5, 6, 7, 13, 14, 15};

bool			hbs::CMP_4021::DrivesPin(size_t n) const
{
  return (n == 2 || n == 3 || n == 12);
}

void			hbs::CMP_4021::LoadParallel(void)
{
  for (size_t i = 0; i < 8; ++i)
    if (GetPin(parallel_pin[i]) == hbs::TRUE)
      value.set(i);
    else
      value.reset(i);
}

void			hbs::CMP_4021::Tick(void)
{
  if (GetPin(9) == hbs::TRUE)
    {
      LoadParallel();
      return ;
    }
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(10) != hbs::TRUE || GetPrevious(10) != hbs::FALSE)
    return ;
  last_tick = timer.GetTime();
  value <<= 1;
  if (GetPin(11) == hbs::TRUE)
    value.set(0);
  else
    value.reset(0);
}

hbs::Tristate		hbs::CMP_4021::Compute(size_t n)
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

hbs::CMP_4021::CMP_4021(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos),
    last_tick(0)
{}

hbs::CMP_4021::~CMP_4021(void)
{}

bool			hbs::CMP_4021::TypeMatches(const std::string &type)
{
  if (type == "4021")
    return (true);
  if (type == "74HC4021")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4021::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4021(timer, name, position));
}

std::string		hbs::CMP_4021::GetDisplayType(void) const
{
  return ("4021 8-stage asynchronous-load parallel/serial-input serial-output shift register");
}
