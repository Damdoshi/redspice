// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74195.hpp"

const std::string	&hbs::CMP_74195::GetType(void) const
{
  return (type = "74195");
}

static const size_t	parallel_pin[4] = {6, 7, 9, 10};
static const size_t	output_pin[4] = {15, 14, 13, 12};

bool			hbs::CMP_74195::DrivesPin(size_t n) const
{
  return ((n >= 12 && n <= 15) || n == 11);
}

void			hbs::CMP_74195::LoadParallel(void)
{
  for (size_t i = 0; i < 4; ++i)
    if (GetPin(parallel_pin[i]) == hbs::TRUE)
      value.set(i);
    else
      value.reset(i);
}

hbs::Tristate		hbs::CMP_74195::NextFirstStage(void)
{
  hbs::Tristate		j;
  hbs::Tristate		k;

  j = GetPin(1);
  k = GetPin(3);
  if (j == hbs::TRUE && k == hbs::TRUE)
    return (hbs::TRUE);
  if (j == hbs::FALSE && k == hbs::FALSE)
    return (hbs::FALSE);
  if (j == hbs::TRUE && k == hbs::FALSE)
    return (value[0] ? hbs::FALSE : hbs::TRUE);
  if (j == hbs::FALSE && k == hbs::TRUE)
    return (value[0] ? hbs::TRUE : hbs::FALSE);
  return (hbs::UNDEFINED);
}

void			hbs::CMP_74195::Tick(void)
{
  hbs::Tristate		first;

  if (GetPin(4) == hbs::FALSE)
    {
      value.reset();
      return ;
    }
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(2) != hbs::TRUE || GetPrevious(2) != hbs::FALSE)
    return ;
  last_tick = timer.GetTime();
  if (GetPin(5) == hbs::FALSE)
    {
      LoadParallel();
      return ;
    }
  first = NextFirstStage();
  value <<= 1;
  if (first == hbs::TRUE)
    value.set(0);
  else if (first == hbs::FALSE)
    value.reset(0);
  else
    value.reset(0);
}

hbs::Tristate		hbs::CMP_74195::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      Tick();
      if (n == 11)
	return (p->second = value[3] ? hbs::FALSE : hbs::TRUE);
      for (size_t i = 0; i < 4; ++i)
	if (n == output_pin[i])
	  return (p->second = value[i] ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74195::CMP_74195(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : AComponent(time, name, pos),
    last_tick(0)
{}

hbs::CMP_74195::~CMP_74195(void)
{}

bool			hbs::CMP_74195::TypeMatches(const std::string &type)
{
  if (type == "74195" || type == "74HC195" || type == "74HCT195" || type == "74LS195")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74195::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74195(timer, name, position));
}

std::string		hbs::CMP_74195::GetDisplayType(void) const
{
  return ("74195 4-bit parallel-access shift register");
}
