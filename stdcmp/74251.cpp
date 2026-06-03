// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74251.hpp"

const std::string	&hbs::CMP_74251::GetType(void) const
{
  return (type = "74251");
}

hbs::Tristate		hbs::CMP_74251::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  if (n == 5 || n == 6)
    {
      p = PresetOutput(n);
      if (GetPin(7) == hbs::TRUE)
	return (p->second = hbs::HIGH_IMPEDANCE);
    }
  return (hbs::CMP_74151::Compute(n));
}

hbs::CMP_74251::CMP_74251(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : CMP_74151(time, name, pos)
{}

hbs::CMP_74251::~CMP_74251(void)
{}

bool			hbs::CMP_74251::TypeMatches(const std::string &type)
{
  if (type == "74251" || type == "74HC251" || type == "74HCT251" || type == "74LS251")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74251::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74251(timer, name, position));
}

std::string		hbs::CMP_74251::GetDisplayType(void) const
{
  return ("74251 8-to-1 multiplexer with 3-state outputs");
}
