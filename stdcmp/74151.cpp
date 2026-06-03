// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74151.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74151::GetType(void) const
{
  return (type = "74151");
}

static const size_t	data_pin[8] = {4, 3, 2, 1, 15, 14, 13, 12};

bool			hbs::CMP_74151::DrivesPin(size_t n) const
{
  return (n == 5 || n == 6);
}

static hbs::Tristate	selected_value(hbs::CMP_74151 &component)
{
  hbs::Tristate		a;
  hbs::Tristate		b;
  hbs::Tristate		c;
  unsigned int		addr;

  if (component.GetPin(7) == hbs::TRUE)
    return (hbs::FALSE);
  if (!hbs::LogicKnown(component.GetPin(7)))
    return (hbs::UNDEFINED);
  a = component.GetPin(11);
  b = component.GetPin(10);
  c = component.GetPin(9);
  if (!hbs::LogicKnown(a) || !hbs::LogicKnown(b) || !hbs::LogicKnown(c))
    return (hbs::UNDEFINED);
  addr = (a == hbs::TRUE ? 1 : 0) | (b == hbs::TRUE ? 2 : 0) | (c == hbs::TRUE ? 4 : 0);
  return (component.GetPin(data_pin[addr]));
}

hbs::Tristate		hbs::CMP_74151::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  hbs::Tristate		value;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 5 || n == 6)
    {
      if (GetPin(7) == hbs::TRUE)
	return (p->second = n == 5 ? hbs::FALSE : hbs::TRUE);
      value = selected_value(*this);
      return (p->second = n == 5 ? value : hbs::LogicNot(value));
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74151::CMP_74151(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74151::~CMP_74151(void)
{}

bool			hbs::CMP_74151::TypeMatches(const std::string &type)
{
  if (type == "74151" || type == "74HC151" || type == "74HCT151" || type == "74LS151")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74151::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74151(timer, name, position));
}

std::string		hbs::CMP_74151::GetDisplayType(void) const
{
  return ("74151 8-to-1 multiplexer");
}
