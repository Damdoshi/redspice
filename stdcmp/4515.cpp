// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4515.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_4515::GetType(void) const
{
  return (type = "4515");
}

static const size_t pinstrenght[24] =
  {
    0, 0, 0, 7, 6, 5, 4, 3, 1, 2, 0, 0,
    13, 12, 15, 14, 9, 8, 11, 10, 0, 0, 0, 0
  };

static bool		read_address(hbs::CMP_4515 &component, unsigned char &value)
{
  hbs::Tristate		pins[4];

  pins[0] = component.GetPin(2);
  pins[1] = component.GetPin(3);
  pins[2] = component.GetPin(21);
  pins[3] = component.GetPin(22);
  value = 0;
  for (size_t i = 0; i < 4; ++i)
    {
      if (!hbs::LogicKnown(pins[i]))
	return (false);
      if (pins[i] == hbs::TRUE)
	value |= 1 << i;
    }
  return (true);
}

hbs::Tristate		hbs::CMP_4515::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  unsigned char		val;
  size_t		active;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if ((n >= 4 && n <= 11) || (n >= 13 && n <= 20))
    {
      if (GetPin(1) == hbs::FALSE)
	{
	  KeepOld(n);
	  if (GetPin(23) == hbs::TRUE)
	    return (p->second = hbs::TRUE);
	  return (GetPin(n));
	}
      if (GetPin(23) == hbs::TRUE)
	return (p->second = hbs::TRUE);
      if (!read_address(*this, val))
	return (p->second = hbs::UNDEFINED);
      if (val == 0)
	active = 10;
      else
	for (active = 0; pinstrenght[active] != val; ++active);
      return (p->second = active + 1 == n ? hbs::FALSE : hbs::TRUE);
    }
  if (n == 12 || n == 24)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4515::CMP_4515(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_4515::~CMP_4515(void)
{}

bool			hbs::CMP_4515::TypeMatches(const std::string &type)
{
  if (type == "4515")
    return (true);
  if (type == "74HC4515")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4515::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4515(timer, name, position));
}

std::string		hbs::CMP_4515::GetDisplayType(void) const
{
  return ("4515 4-to-16 decoder/demultiplexer with latch, active-low outputs");
}
