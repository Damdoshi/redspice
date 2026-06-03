// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74166.hpp"

const std::string	&hbs::CMP_74166::GetType(void) const
{
  return (type = "74166");
}

static const size_t	parallel_pin[8] = {2, 3, 4, 5, 10, 11, 12, 14};

bool			hbs::CMP_74166::DrivesPin(size_t n) const
{
  return (n == 13);
}

void			hbs::CMP_74166::LoadParallel(void)
{
  for (size_t i = 0; i < 8; ++i)
    if (GetPin(parallel_pin[i]) == hbs::TRUE)
      value.set(i);
    else
      value.reset(i);
}

void			hbs::CMP_74166::Tick(void)
{
  if (GetPin(9) == hbs::FALSE)
    {
      value.reset();
      return ;
    }
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(7) != hbs::TRUE || GetPrevious(7) != hbs::FALSE)
    return ;
  if (GetPin(6) == hbs::TRUE)
    return ;
  last_tick = timer.GetTime();
  if (GetPin(15) == hbs::FALSE)
    {
      LoadParallel();
      return ;
    }
  value <<= 1;
  if (GetPin(1) == hbs::TRUE)
    value.set(0);
  else
    value.reset(0);
}

hbs::Tristate		hbs::CMP_74166::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 13)
    {
      Tick();
      return (p->second = value[7] ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74166::CMP_74166(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : AComponent(time, name, pos),
    last_tick(0)
{}

hbs::CMP_74166::~CMP_74166(void)
{}

bool			hbs::CMP_74166::TypeMatches(const std::string &type)
{
  if (type == "74166" || type == "74HC166" || type == "74HCT166" || type == "74LS166")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74166::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74166(timer, name, position));
}

std::string		hbs::CMP_74166::GetDisplayType(void) const
{
  return ("74166 8-bit parallel-in serial-out shift register");
}
