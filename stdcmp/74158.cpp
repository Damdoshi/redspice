// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74158.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74158::GetType(void) const
{
  return (type = "74158");
}

hbs::Tristate		hbs::CMP_74158::DisabledValue(void) const
{
  return (hbs::TRUE);
}

hbs::Tristate		hbs::CMP_74158::OutputValue(hbs::Tristate v) const
{
  return (hbs::LogicNot(v));
}

hbs::CMP_74158::CMP_74158(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : CMP_74157(time, name, pos)
{}

hbs::CMP_74158::~CMP_74158(void)
{}

bool			hbs::CMP_74158::TypeMatches(const std::string &type)
{
  if (type == "74158" || type == "74HC158" || type == "74HCT158" || type == "74LS158")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74158::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74158(timer, name, position));
}

std::string		hbs::CMP_74158::GetDisplayType(void) const
{
  return ("74158 Quad 2-to-1 multiplexer, inverted outputs");
}
