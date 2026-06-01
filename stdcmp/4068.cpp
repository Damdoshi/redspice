// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"Logic.hpp"
#include		"4068.hpp"

const std::string	&hbs::CMP_4068::GetType(void) const
{
  return (type = "4068");
}

bool			hbs::CMP_4068::TypeMatches(const std::string	&type)
{
  if (type == "4068")
    return (true);
  if (type == "74HC4068")
    return (true);
  if (type == "CD4068")
    return (true);
  if (type == "HEF4068")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4068::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4068(timer, name, position));
}

std::string		hbs::CMP_4068::GetDisplayType(void) const
{
  return ("4068 8-input NAND/AND");
}

bool			hbs::CMP_4068::DrivesPin(size_t		n) const
{
  return (n == 1 || n == 13);
}

hbs::Tristate		hbs::CMP_4068::Compute(size_t		n)
{
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);
  Preset		p = PresetOutput(n);

  if (n == 13)
    { hbs::Tristate values[] = {GetPin(2), GetPin(3), GetPin(4), GetPin(5), GetPin(9), GetPin(10), GetPin(11), GetPin(12)}; return (p->second = hbs::LogicNand(values, 8)); }
  if (n == 1)
    { hbs::Tristate values[] = {GetPin(2), GetPin(3), GetPin(4), GetPin(5), GetPin(9), GetPin(10), GetPin(11), GetPin(12)}; return (p->second = hbs::LogicAnd(values, 8)); }
  if (n == 6 || n == 7 || n == 8 || n == 14)
    return (p->second = hbs::UNDEFINED);
  return (p->second = GetPin(n));
}

hbs::CMP_4068::CMP_4068(const hbs::Timer	&timer,
			 const std::string	&name,
			 const std::string	&pos)
  : AComponent(timer, name, pos)
{}

hbs::CMP_4068::~CMP_4068(void)
{}
