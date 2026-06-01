// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"Logic.hpp"
#include		"4584.hpp"

const std::string	&hbs::CMP_4584::GetType(void) const
{
  return (type = "4584");
}

bool			hbs::CMP_4584::TypeMatches(const std::string	&type)
{
  if (type == "4584")
    return (true);
  if (type == "7414")
    return (true);
  if (type == "74HC14")
    return (true);
  if (type == "74HCT14")
    return (true);
  if (type == "74LS14")
    return (true);
  if (type == "74HC4584")
    return (true);
  if (type == "CD4584")
    return (true);
  if (type == "HEF4584")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4584::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4584(timer, name, position));
}

std::string		hbs::CMP_4584::GetDisplayType(void) const
{
  return ("4584/7414 Hex Schmitt trigger inverter");
}

bool			hbs::CMP_4584::DrivesPin(size_t		n) const
{
  return (n == 2 || n == 4 || n == 6 || n == 8 || n == 10 || n == 12);
}

hbs::Tristate		hbs::CMP_4584::Compute(size_t		n)
{
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);
  Preset		p = PresetOutput(n);

  if (n == 2)
    return (p->second = hbs::LogicNot(GetPin(1)));
  if (n == 4)
    return (p->second = hbs::LogicNot(GetPin(3)));
  if (n == 6)
    return (p->second = hbs::LogicNot(GetPin(5)));
  if (n == 8)
    return (p->second = hbs::LogicNot(GetPin(9)));
  if (n == 10)
    return (p->second = hbs::LogicNot(GetPin(11)));
  if (n == 12)
    return (p->second = hbs::LogicNot(GetPin(13)));
  if (n == 7 || n == 14)
    return (p->second = hbs::UNDEFINED);
  return (p->second = GetPin(n));
}

hbs::CMP_4584::CMP_4584(const hbs::Timer	&timer,
			 const std::string	&name,
			 const std::string	&pos)
  : AComponent(timer, name, pos)
{}

hbs::CMP_4584::~CMP_4584(void)
{}
