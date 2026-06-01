// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74595.hpp"

const std::string	&hbs::CMP_74595::GetType(void) const
{
  return (type = "74595");
}

static const size_t	output_pin[8] = {15, 1, 2, 3, 4, 5, 6, 7};

bool			hbs::CMP_74595::DrivesPin(size_t n) const
{
  return ((n >= 1 && n <= 7) || n == 9 || n == 15);
}

void			hbs::CMP_74595::Tick(void)
{
  if (GetPin(10) == hbs::FALSE)
    shift.reset();
  else if (last_shift_tick != timer.GetTime() && GetPin(11) == hbs::TRUE && GetPrevious(11) == hbs::FALSE)
    {
      last_shift_tick = timer.GetTime();
      shift <<= 1;
      if (GetPin(14) == hbs::TRUE)
	shift.set(0);
      else
	shift.reset(0);
    }
  if (last_latch_tick != timer.GetTime() && GetPin(12) == hbs::TRUE && GetPrevious(12) == hbs::FALSE)
    {
      last_latch_tick = timer.GetTime();
      storage = shift;
    }
}

hbs::Tristate		hbs::CMP_74595::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      Tick();
      if (n == 9)
	return (p->second = shift[7] ? hbs::TRUE : hbs::FALSE);
      if (GetPin(13) == hbs::TRUE)
	return (p->second = hbs::HIGH_IMPEDANCE);
      for (size_t i = 0; i < 8; ++i)
	if (n == output_pin[i])
	  return (p->second = storage[i] ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74595::CMP_74595(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : AComponent(time, name, pos),
    last_shift_tick(0),
    last_latch_tick(0)
{}

hbs::CMP_74595::~CMP_74595(void)
{}

bool			hbs::CMP_74595::TypeMatches(const std::string &type)
{
  if (type == "74595" || type == "74HC595" || type == "74HCT595" || type == "74LS595")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74595::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74595(timer, name, position));
}

std::string		hbs::CMP_74595::GetDisplayType(void) const
{
  return ("74595 8-bit serial-in shift register with storage latch and 3-state outputs");
}
