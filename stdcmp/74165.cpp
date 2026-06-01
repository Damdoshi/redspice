// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74165.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74165::GetType(void) const
{
  return (type = "74165");
}

static const size_t	parallel_pin[8] = {11, 12, 13, 14, 3, 4, 5, 6};

bool			hbs::CMP_74165::DrivesPin(size_t n) const
{
  return (n == 7 || n == 9);
}

void			hbs::CMP_74165::LoadParallel(void)
{
  for (size_t i = 0; i < 8; ++i)
    if (GetPin(parallel_pin[i]) == hbs::TRUE)
      value.set(i);
    else
      value.reset(i);
}

void			hbs::CMP_74165::Tick(void)
{
  if (GetPin(1) == hbs::FALSE)
    {
      LoadParallel();
      return ;
    }
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(15) == hbs::TRUE)
    return ;
  if (GetPin(2) != hbs::TRUE || GetPrevious(2) != hbs::FALSE)
    return ;
  last_tick = timer.GetTime();
  value <<= 1;
  if (GetPin(10) == hbs::TRUE)
    value.set(0);
  else
    value.reset(0);
}

hbs::Tristate		hbs::CMP_74165::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 7 || n == 9)
    {
      Tick();
      if (n == 7)
	return (p->second = value[7] ? hbs::TRUE : hbs::FALSE);
      return (p->second = value[7] ? hbs::FALSE : hbs::TRUE);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74165::CMP_74165(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : AComponent(time, name, pos),
    last_tick(0)
{}

hbs::CMP_74165::~CMP_74165(void)
{}

bool			hbs::CMP_74165::TypeMatches(const std::string &type)
{
  if (type == "74165" || type == "74HC165" || type == "74HCT165" || type == "74LS165")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74165::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74165(timer, name, position));
}

std::string		hbs::CMP_74165::GetDisplayType(void) const
{
  return ("74165 8-bit parallel-in serial-out shift register");
}
