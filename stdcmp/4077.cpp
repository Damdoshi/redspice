// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"Logic.hpp"
#include		"4077.hpp"

const std::string	&hbs::CMP_4077::GetType(void) const
{
  return (type = "4077");
}

bool			hbs::CMP_4077::TypeMatches(const std::string	&type)
{
  if (type == "4077")
    return (true);
  if (type == "74266")
    return (true);
  if (type == "74HC266")
    return (true);
  if (type == "74HCT266")
    return (true);
  if (type == "74LS266")
    return (true);
  if (type == "74HC4077")
    return (true);
  if (type == "CD4077")
    return (true);
  if (type == "HEF4077")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4077::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4077(timer, name, position));
}

std::string		hbs::CMP_4077::GetDisplayType(void) const
{
  return ("4077/74266 Quad 2-input XNOR");
}

bool			hbs::CMP_4077::DrivesPin(size_t		n) const
{
  return (n == 3 || n == 4 || n == 10 || n == 11);
}

hbs::Tristate		hbs::CMP_4077::Compute(size_t		n)
{
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);
  Preset		p = PresetOutput(n);

  if (n == 3)
    { hbs::Tristate values[] = {GetPin(1), GetPin(2)}; return (p->second = hbs::LogicXnor(values, 2)); }
  if (n == 4)
    { hbs::Tristate values[] = {GetPin(5), GetPin(6)}; return (p->second = hbs::LogicXnor(values, 2)); }
  if (n == 10)
    { hbs::Tristate values[] = {GetPin(8), GetPin(9)}; return (p->second = hbs::LogicXnor(values, 2)); }
  if (n == 11)
    { hbs::Tristate values[] = {GetPin(12), GetPin(13)}; return (p->second = hbs::LogicXnor(values, 2)); }
  if (n == 7 || n == 14)
    return (p->second = hbs::UNDEFINED);
  return (p->second = GetPin(n));
}

hbs::CMP_4077::CMP_4077(const hbs::Timer	&timer,
			 const std::string	&name,
			 const std::string	&pos)
  : AComponent(timer, name, pos)
{}

hbs::CMP_4077::~CMP_4077(void)
{}
