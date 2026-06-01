// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"Logic.hpp"
#include		"4503.hpp"

const std::string	&hbs::CMP_4503::GetType(void) const
{
  return (type = "4503");
}

bool			hbs::CMP_4503::TypeMatches(const std::string	&type)
{
  if (type == "4503")
    return (true);
  if (type == "74HC4503")
    return (true);
  if (type == "CD4503")
    return (true);
  if (type == "HEF4503")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4503::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4503(timer, name, position));
}

std::string		hbs::CMP_4503::GetDisplayType(void) const
{
  return ("4503 Hex non-inverting buffer, 3-state outputs");
}

bool			hbs::CMP_4503::DrivesPin(size_t		n) const
{
  return (n == 3 || n == 5 || n == 7 || n == 9 || n == 11 || n == 13);
}

static hbs::Tristate	Compute4503Buffer(hbs::Tristate input,
					  hbs::Tristate disable)
{
  if (disable == hbs::BROKEN || input == hbs::BROKEN)
    return (hbs::BROKEN);
  if (disable == hbs::TRUE)
    return (hbs::HIGH_IMPEDANCE);
  if (disable != hbs::FALSE)
    return (hbs::UNDEFINED);
  return (input);
}

hbs::Tristate		hbs::CMP_4503::Compute(size_t		n)
{
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);
  Preset		p = PresetOutput(n);

  if (n == 3)
    return (p->second = Compute4503Buffer(GetPin(2), GetPin(1)));
  if (n == 5)
    return (p->second = Compute4503Buffer(GetPin(4), GetPin(1)));
  if (n == 7)
    return (p->second = Compute4503Buffer(GetPin(6), GetPin(1)));
  if (n == 9)
    return (p->second = Compute4503Buffer(GetPin(10), GetPin(1)));
  if (n == 11)
    return (p->second = Compute4503Buffer(GetPin(12), GetPin(15)));
  if (n == 13)
    return (p->second = Compute4503Buffer(GetPin(14), GetPin(15)));
  if (n == 8 || n == 16)
    return (p->second = hbs::UNDEFINED);
  return (p->second = GetPin(n));
}

hbs::CMP_4503::CMP_4503(const hbs::Timer	&timer,
				 const std::string	&name,
				 const std::string	&pos)
  : AComponent(timer, name, pos)
{}

hbs::CMP_4503::~CMP_4503(void)
{}
