// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74154.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74154::GetType(void) const
{
  return (type = "74154");
}

static const size_t	output_pin[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, 16, 17};

bool			hbs::CMP_74154::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 16; ++i)
    if (n == output_pin[i])
      return (true);
  return (false);
}

hbs::Tristate		hbs::CMP_74154::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  hbs::Tristate		a;
  hbs::Tristate		b;
  hbs::Tristate		c;
  hbs::Tristate		d;
  unsigned int		addr;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      if (GetPin(18) == hbs::TRUE || GetPin(19) == hbs::TRUE)
	return (p->second = hbs::TRUE);
      if (!hbs::LogicKnown(GetPin(18)) || !hbs::LogicKnown(GetPin(19)))
	return (p->second = hbs::UNDEFINED);
      a = GetPin(23);
      b = GetPin(22);
      c = GetPin(21);
      d = GetPin(20);
      if (!hbs::LogicKnown(a) || !hbs::LogicKnown(b) || !hbs::LogicKnown(c) || !hbs::LogicKnown(d))
	return (p->second = hbs::UNDEFINED);
      addr = (a == hbs::TRUE ? 1 : 0) | (b == hbs::TRUE ? 2 : 0) | (c == hbs::TRUE ? 4 : 0) | (d == hbs::TRUE ? 8 : 0);
      return (p->second = n == output_pin[addr] ? hbs::FALSE : hbs::TRUE);
    }
  if (n == 12 || n == 24)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74154::CMP_74154(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74154::~CMP_74154(void)
{}

bool			hbs::CMP_74154::TypeMatches(const std::string &type)
{
  if (type == "74154" || type == "74HC154" || type == "74HCT154" || type == "74LS154")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74154::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74154(timer, name, position));
}

std::string		hbs::CMP_74154::GetDisplayType(void) const
{
  return ("74154 4-to-16 decoder/demultiplexer, active-low outputs");
}
