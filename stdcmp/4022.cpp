// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4022.hpp"
#define			PIN					-1 +
#define			XXX					255

const std::string	&hbs::CMP_4022::GetType(void) const
{
  return (type = "4022");
}

static const size_t	pin_output_index[16] =
  {
    [PIN  1] = 1,
    [PIN  2] = 0,
    [PIN  3] = 2,
    [PIN  4] = 5,
    [PIN  5] = 6,
    [PIN  6] = XXX,
    [PIN  7] = 3,
    [PIN  8] = XXX,
    [PIN  9] = XXX,
    [PIN 10] = 7,
    [PIN 11] = 4,
    [PIN 12] = XXX,
    [PIN 13] = XXX,
    [PIN 14] = XXX,
    [PIN 15] = XXX,
    [PIN 16] = XXX
  };

bool			hbs::CMP_4022::DrivesPin(size_t n) const
{
  return ((n >= 1 && n <= 5) || n == 7 || n == 10 || n == 11 || n == 12);
}

hbs::Tristate		hbs::CMP_4022::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  bool			tick;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  if (DrivesPin(n))
    {
      if (GetPin(15) == hbs::TRUE)
	{
	  value.reset();
	  value.set(0);
	  return (p->second = (pin_output_index[n - 1] == 0 ? hbs::TRUE : hbs::FALSE));
	}
      tick = false;
      if (GetPin(14) == hbs::TRUE && GetPrevious(14) == hbs::FALSE)
	tick = true;
      else if (GetPin(13) == hbs::FALSE && GetPrevious(13) == hbs::TRUE)
	tick = true;
      if (tick && last_tick != timer.GetTime())
	{
	  last_tick = timer.GetTime();
	  if (!(value <<= 1).any())
	    value.set(0);
	}
      if (n == 12)
	return (p->second = ((value >> 4).any() ? hbs::FALSE : hbs::TRUE));
      return (p->second = (value[pin_output_index[n - 1]] ? hbs::TRUE : hbs::FALSE));
    }
  return (GetPin(n));
}

hbs::CMP_4022::CMP_4022(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos),
    value(1),
    last_tick(0)
{}

hbs::CMP_4022::~CMP_4022(void)
{}

bool			hbs::CMP_4022::TypeMatches(const std::string &type)
{
  if (type == "4022")
    return (true);
  if (type == "74HC4022")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4022::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4022(timer, name, position));
}

std::string		hbs::CMP_4022::GetDisplayType(void) const
{
  return ("4022 Divide-by-8 Johnson counter");
}
