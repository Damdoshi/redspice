// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74257.hpp"

const std::string	&hbs::CMP_74257::GetType(void) const
{
  return (type = "74257");
}

hbs::Tristate		hbs::CMP_74257::DisabledValue(void) const
{
  return (hbs::HIGH_IMPEDANCE);
}

hbs::CMP_74257::CMP_74257(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : CMP_74157(time, name, pos)
{}

hbs::CMP_74257::~CMP_74257(void)
{}

bool			hbs::CMP_74257::TypeMatches(const std::string &type)
{
  if (type == "74257" || type == "74HC257" || type == "74HCT257" || type == "74LS257")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74257::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74257(timer, name, position));
}

std::string		hbs::CMP_74257::GetDisplayType(void) const
{
  return ("74257 Quad 2-to-1 multiplexer with 3-state outputs");
}
