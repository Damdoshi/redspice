// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74164.hpp"

const std::string	&hbs::CMP_74164::GetType(void) const
{
  return (type = "74164");
}

static const size_t	output_pin[8] = {3, 4, 5, 6, 10, 11, 12, 13};

bool			hbs::CMP_74164::DrivesPin(size_t n) const
{
  return ((n >= 3 && n <= 6) || (n >= 10 && n <= 13));
}

void			hbs::CMP_74164::Tick(void)
{
  if (GetPin(9) == hbs::FALSE)
    {
      value.reset();
      return ;
    }
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(8) != hbs::TRUE || GetPrevious(8) != hbs::FALSE)
    return ;
  last_tick = timer.GetTime();
  value <<= 1;
  if (GetPin(1) == hbs::TRUE && GetPin(2) == hbs::TRUE)
    value.set(0);
  else
    value.reset(0);
}

hbs::Tristate		hbs::CMP_74164::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      Tick();
      for (size_t i = 0; i < 8; ++i)
	if (n == output_pin[i])
	  return (p->second = value[i] ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 7 || n == 14)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74164::CMP_74164(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : AComponent(time, name, pos),
    last_tick(0)
{}

hbs::CMP_74164::~CMP_74164(void)
{}

bool			hbs::CMP_74164::TypeMatches(const std::string &type)
{
  if (type == "74164" || type == "74HC164" || type == "74HCT164" || type == "74LS164")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74164::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74164(timer, name, position));
}

std::string		hbs::CMP_74164::GetDisplayType(void) const
{
  return ("74164 8-bit serial-in parallel-out shift register");
}
