// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"Logic.hpp"
#include		"4502.hpp"

const std::string	&hbs::CMP_4502::GetType(void) const
{
  return (type = "4502");
}

bool			hbs::CMP_4502::TypeMatches(const std::string	&type)
{
  if (type == "4502")
    return (true);
  if (type == "74HC4502")
    return (true);
  if (type == "CD4502")
    return (true);
  if (type == "HEF4502")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4502::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4502(timer, name, position));
}

std::string		hbs::CMP_4502::GetDisplayType(void) const
{
  return ("4502 Strobed hex inverter/buffer, 3-state outputs");
}

bool			hbs::CMP_4502::DrivesPin(size_t		n) const
{
  return (n == 2 || n == 5 || n == 7 || n == 9 || n == 11 || n == 14);
}

static hbs::Tristate	Compute4502Buffer(hbs::Tristate input,
					  hbs::Tristate disable,
					  hbs::Tristate inhibit)
{
  if (disable == hbs::BROKEN || inhibit == hbs::BROKEN || input == hbs::BROKEN)
    return (hbs::BROKEN);
  if (disable == hbs::TRUE)
    return (hbs::HIGH_IMPEDANCE);
  if (disable != hbs::FALSE)
    return (hbs::UNDEFINED);
  if (inhibit == hbs::TRUE)
    return (hbs::FALSE);
  if (inhibit != hbs::FALSE)
    return (hbs::UNDEFINED);
  return (hbs::LogicNot(input));
}

hbs::Tristate		hbs::CMP_4502::Compute(size_t		n)
{
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);
  Preset		p = PresetOutput(n);

  if (n == 5)
    return (p->second = Compute4502Buffer(GetPin(3), GetPin(4), GetPin(12)));
  if (n == 7)
    return (p->second = Compute4502Buffer(GetPin(6), GetPin(4), GetPin(12)));
  if (n == 2)
    return (p->second = Compute4502Buffer(GetPin(1), GetPin(4), GetPin(12)));
  if (n == 9)
    return (p->second = Compute4502Buffer(GetPin(10), GetPin(4), GetPin(12)));
  if (n == 11)
    return (p->second = Compute4502Buffer(GetPin(13), GetPin(4), GetPin(12)));
  if (n == 14)
    return (p->second = Compute4502Buffer(GetPin(15), GetPin(4), GetPin(12)));
  if (n == 8 || n == 16)
    return (p->second = hbs::UNDEFINED);
  return (p->second = GetPin(n));
}

hbs::CMP_4502::CMP_4502(const hbs::Timer	&timer,
				 const std::string	&name,
				 const std::string	&pos)
  : AComponent(timer, name, pos)
{}

hbs::CMP_4502::~CMP_4502(void)
{}
