// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74258.hpp"

const std::string	&hbs::CMP_74258::GetType(void) const
{
  return (type = "74258");
}

hbs::Tristate		hbs::CMP_74258::DisabledValue(void) const
{
  return (hbs::HIGH_IMPEDANCE);
}

hbs::CMP_74258::CMP_74258(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : CMP_74158(time, name, pos)
{}

hbs::CMP_74258::~CMP_74258(void)
{}

bool			hbs::CMP_74258::TypeMatches(const std::string &type)
{
  if (type == "74258" || type == "74HC258" || type == "74HCT258" || type == "74LS258")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74258::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74258(timer, name, position));
}

std::string		hbs::CMP_74258::GetDisplayType(void) const
{
  return ("74258 Quad 2-to-1 multiplexer with 3-state inverted outputs");
}
