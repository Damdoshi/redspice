// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74153.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74153::GetType(void) const
{
  return (type = "74153");
}

static const size_t	data_pin[2][4] = {{6, 5, 4, 3}, {10, 11, 12, 13}};
static const size_t	enable_pin[2] = {1, 15};
static const size_t	output_pin[2] = {7, 9};

bool			hbs::CMP_74153::DrivesPin(size_t n) const
{
  return (n == 7 || n == 9);
}

static hbs::Tristate	mux_value(hbs::CMP_74153 &component, size_t part)
{
  hbs::Tristate		a;
  hbs::Tristate		b;
  unsigned int		addr;

  if (component.GetPin(enable_pin[part]) == hbs::TRUE)
    return (hbs::FALSE);
  if (!hbs::LogicKnown(component.GetPin(enable_pin[part])))
    return (hbs::UNDEFINED);
  a = component.GetPin(14);
  b = component.GetPin(2);
  if (!hbs::LogicKnown(a) || !hbs::LogicKnown(b))
    return (hbs::UNDEFINED);
  addr = (a == hbs::TRUE ? 1 : 0) | (b == hbs::TRUE ? 2 : 0);
  return (component.GetPin(data_pin[part][addr]));
}

hbs::Tristate		hbs::CMP_74153::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == output_pin[0])
    return (p->second = mux_value(*this, 0));
  if (n == output_pin[1])
    return (p->second = mux_value(*this, 1));
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74153::CMP_74153(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74153::~CMP_74153(void)
{}

bool			hbs::CMP_74153::TypeMatches(const std::string &type)
{
  if (type == "74153" || type == "74HC153" || type == "74HCT153" || type == "74LS153")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74153::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74153(timer, name, position));
}

std::string		hbs::CMP_74153::GetDisplayType(void) const
{
  return ("74153 Dual 4-to-1 multiplexer");
}
