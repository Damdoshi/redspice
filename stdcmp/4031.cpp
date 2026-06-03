// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4031.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_4031::GetType(void) const
{
  return (type = "4031");
}

bool			hbs::CMP_4031::DrivesPin(size_t n) const
{
  return (n == 13 || n == 14);
}

void			hbs::CMP_4031::Tick(void)
{
  if (GetPin(6) == hbs::TRUE)
    {
      value.reset();
      return ;
    }
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(9) != hbs::TRUE || GetPrevious(9) != hbs::FALSE)
    return ;
  last_tick = timer.GetTime();
  value <<= 1;
  if (GetPin(7) == hbs::TRUE)
    value.set(0);
  else
    value.reset(0);
}

hbs::Tristate		hbs::CMP_4031::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      Tick();
      if (n == 13)
	return (p->second = value[63] ? hbs::TRUE : hbs::FALSE);
      return (p->second = value[63] ? hbs::FALSE : hbs::TRUE);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4031::CMP_4031(const hbs::Timer	&time,
				const std::string	&name,
				const std::string	&pos)
  : AComponent(time, name, pos),
    last_tick(0)
{}

hbs::CMP_4031::~CMP_4031(void)
{}

bool			hbs::CMP_4031::TypeMatches(const std::string &type)
{
  if (type == "4031" || type == "74HC4031" || type == "74HCT4031")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4031::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4031(timer, name, position));
}

std::string		hbs::CMP_4031::GetDisplayType(void) const
{
  return ("4031 64-stage static shift register");
}
