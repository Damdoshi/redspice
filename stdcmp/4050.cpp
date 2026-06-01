// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"Logic.hpp"
#include		"4050.hpp"

const std::string	&hbs::CMP_4050::GetType(void) const
{
  return (type = "4050");
}

bool			hbs::CMP_4050::TypeMatches(const std::string	&type)
{
  if (type == "4050")
    return (true);
  if (type == "74HC4050")
    return (true);
  if (type == "CD4050")
    return (true);
  if (type == "HEF4050")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4050::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4050(timer, name, position));
}

std::string		hbs::CMP_4050::GetDisplayType(void) const
{
  return ("4050 Hex non-inverting buffer/converter");
}

bool			hbs::CMP_4050::DrivesPin(size_t		n) const
{
  return (n == 2 || n == 4 || n == 6 || n == 8 || n == 10 || n == 12);
}

hbs::Tristate		hbs::CMP_4050::Compute(size_t		n)
{
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);
  Preset		p = PresetOutput(n);

  if (n == 2)
    return (p->second = GetPin(1));
  if (n == 4)
    return (p->second = GetPin(3));
  if (n == 6)
    return (p->second = GetPin(5));
  if (n == 8)
    return (p->second = GetPin(9));
  if (n == 10)
    return (p->second = GetPin(11));
  if (n == 12)
    return (p->second = GetPin(13));
  if (n == 7 || n == 14)
    return (p->second = hbs::UNDEFINED);
  return (p->second = GetPin(n));
}

hbs::CMP_4050::CMP_4050(const hbs::Timer	&timer,
			 const std::string	&name,
			 const std::string	&pos)
  : AComponent(timer, name, pos)
{}

hbs::CMP_4050::~CMP_4050(void)
{}
