// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74238.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74238::GetType(void) const
{
  return (type = "74238");
}

static const size_t	output_pin[8] = {15, 14, 13, 12, 11, 10, 9, 7};

bool			hbs::CMP_74238::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 8; ++i)
    if (n == output_pin[i])
      return (true);
  return (false);
}

hbs::Tristate		hbs::CMP_74238::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  hbs::Tristate		a;
  hbs::Tristate		b;
  hbs::Tristate		c;
  unsigned int		addr;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      if (GetPin(6) == hbs::FALSE || GetPin(4) == hbs::TRUE || GetPin(5) == hbs::TRUE)
	return (p->second = hbs::FALSE);
      if (!hbs::LogicKnown(GetPin(6)) || !hbs::LogicKnown(GetPin(4)) || !hbs::LogicKnown(GetPin(5)))
	return (p->second = hbs::UNDEFINED);
      a = GetPin(1);
      b = GetPin(2);
      c = GetPin(3);
      if (!hbs::LogicKnown(a) || !hbs::LogicKnown(b) || !hbs::LogicKnown(c))
	return (p->second = hbs::UNDEFINED);
      addr = (a == hbs::TRUE ? 1 : 0) | (b == hbs::TRUE ? 2 : 0) | (c == hbs::TRUE ? 4 : 0);
      return (p->second = n == output_pin[addr] ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74238::CMP_74238(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74238::~CMP_74238(void)
{}

bool			hbs::CMP_74238::TypeMatches(const std::string &type)
{
  if (type == "74238" || type == "74HC238" || type == "74HCT238" || type == "74LS238")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74238::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74238(timer, name, position));
}

std::string		hbs::CMP_74238::GetDisplayType(void) const
{
  return ("74238 3-to-8 decoder/demultiplexer, active-high outputs");
}
