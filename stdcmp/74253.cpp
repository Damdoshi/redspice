// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74253.hpp"

const std::string	&hbs::CMP_74253::GetType(void) const
{
  return (type = "74253");
}

hbs::Tristate		hbs::CMP_74253::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  if ((n == 7 && GetPin(1) == hbs::TRUE) || (n == 9 && GetPin(15) == hbs::TRUE))
    {
      p = PresetOutput(n);
      return (p->second = hbs::HIGH_IMPEDANCE);
    }
  return (hbs::CMP_74153::Compute(n));
}

hbs::CMP_74253::CMP_74253(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : CMP_74153(time, name, pos)
{}

hbs::CMP_74253::~CMP_74253(void)
{}

bool			hbs::CMP_74253::TypeMatches(const std::string &type)
{
  if (type == "74253" || type == "74HC253" || type == "74HCT253" || type == "74LS253")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74253::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74253(timer, name, position));
}

std::string		hbs::CMP_74253::GetDisplayType(void) const
{
  return ("74253 Dual 4-to-1 multiplexer with 3-state outputs");
}
