// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74138.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74138::GetType(void) const
{
  return (type = "74138");
}

static const size_t	output_pin[8] = {15, 14, 13, 12, 11, 10, 9, 7};

bool			hbs::CMP_74138::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 8; ++i)
    if (n == output_pin[i])
      return (true);
  return (false);
}

static hbs::Tristate	decode(hbs::CMP_74138 &component, size_t n)
{
  hbs::Tristate		a;
  hbs::Tristate		b;
  hbs::Tristate		c;
  unsigned int		addr;

  if (component.GetPin(6) == hbs::FALSE || component.GetPin(4) == hbs::TRUE || component.GetPin(5) == hbs::TRUE)
    return (hbs::TRUE);
  if (!hbs::LogicKnown(component.GetPin(6)) || !hbs::LogicKnown(component.GetPin(4)) || !hbs::LogicKnown(component.GetPin(5)))
    return (hbs::UNDEFINED);
  a = component.GetPin(1);
  b = component.GetPin(2);
  c = component.GetPin(3);
  if (!hbs::LogicKnown(a) || !hbs::LogicKnown(b) || !hbs::LogicKnown(c))
    return (hbs::UNDEFINED);
  addr = (a == hbs::TRUE ? 1 : 0) | (b == hbs::TRUE ? 2 : 0) | (c == hbs::TRUE ? 4 : 0);
  return (n == output_pin[addr] ? hbs::FALSE : hbs::TRUE);
}

hbs::Tristate		hbs::CMP_74138::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    return (p->second = decode(*this, n));
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74138::CMP_74138(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74138::~CMP_74138(void)
{}

bool			hbs::CMP_74138::TypeMatches(const std::string &type)
{
  if (type == "74138" || type == "74HC138" || type == "74HCT138" || type == "74LS138")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74138::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74138(timer, name, position));
}

std::string		hbs::CMP_74138::GetDisplayType(void) const
{
  return ("74138 3-to-8 decoder/demultiplexer, active-low outputs");
}
