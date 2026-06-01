// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4015.hpp"

const std::string	&hbs::CMP_4015::GetType(void) const
{
  return (type = "4015");
}

bool			hbs::CMP_4015::DrivesPin(size_t n) const
{
  return ((n >= 2 && n <= 5) || (n >= 10 && n <= 13));
}

void			hbs::CMP_4015::Tick(size_t reg)
{
  size_t		clock_pin = reg == 0 ? 9 : 1;
  size_t		data_pin = reg == 0 ? 7 : 15;
  size_t		reset_pin = reg == 0 ? 6 : 14;

  if (GetPin(reset_pin) == hbs::TRUE)
    {
      value[reg].reset();
      return ;
    }
  if (last_tick[reg] == timer.GetTime())
    return ;
  if (GetPin(clock_pin) != hbs::TRUE || GetPrevious(clock_pin) != hbs::FALSE)
    return ;
  last_tick[reg] = timer.GetTime();
  value[reg] <<= 1;
  if (GetPin(data_pin) == hbs::TRUE)
    value[reg].set(0);
  else
    value[reg].reset(0);
}

hbs::Tristate		hbs::CMP_4015::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n >= 2 && n <= 5)
    {
      Tick(0);
      return (p->second = value[0][n - 2] ? hbs::TRUE : hbs::FALSE);
    }
  if (n >= 10 && n <= 13)
    {
      Tick(1);
      return (p->second = value[1][n - 10] ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4015::CMP_4015(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos)
{
  last_tick[0] = 0;
  last_tick[1] = 0;
}

hbs::CMP_4015::~CMP_4015(void)
{}

bool			hbs::CMP_4015::TypeMatches(const std::string &type)
{
  if (type == "4015")
    return (true);
  if (type == "74HC4015")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4015::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4015(timer, name, position));
}

std::string		hbs::CMP_4015::GetDisplayType(void) const
{
  return ("4015 Dual 4-stage serial-in parallel-out shift register");
}
