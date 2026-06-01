// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"Logic.hpp"
#include		"4000.hpp"

const std::string	&hbs::CMP_4000::GetType(void) const
{
  return (type = "4000");
}

bool			hbs::CMP_4000::TypeMatches(const std::string	&type)
{
  if (type == "4000")
    return (true);
  if (type == "CD4000")
    return (true);
  if (type == "HEF4000")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4000::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4000(timer, name, position));
}

std::string		hbs::CMP_4000::GetDisplayType(void) const
{
  return ("4000 Dual 3-input NOR plus inverter");
}

bool			hbs::CMP_4000::DrivesPin(size_t		n) const
{
  return (n == 6 || n == 9 || n == 10);
}

hbs::Tristate		hbs::CMP_4000::Compute(size_t		n)
{
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);
  Preset		p = PresetOutput(n);

  if (n == 6)
    {
      hbs::Tristate values[] = {GetPin(3), GetPin(4), GetPin(5)};
      return (p->second = hbs::LogicNor(values, 3));
    }
  if (n == 10)
    {
      hbs::Tristate values[] = {GetPin(11), GetPin(12), GetPin(13)};
      return (p->second = hbs::LogicNor(values, 3));
    }
  if (n == 9)
    return (p->second = hbs::LogicNot(GetPin(8)));
  if (n == 1 || n == 2 || n == 7 || n == 14)
    return (p->second = hbs::UNDEFINED);
  return (p->second = GetPin(n));
}

hbs::CMP_4000::CMP_4000(const hbs::Timer	&timer,
			 const std::string	&name,
			 const std::string	&pos)
  : AComponent(timer, name, pos)
{}

hbs::CMP_4000::~CMP_4000(void)
{}
