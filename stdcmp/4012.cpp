// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"Logic.hpp"
#include		"4012.hpp"

const std::string	&hbs::CMP_4012::GetType(void) const
{
  return (type = "4012");
}

bool			hbs::CMP_4012::TypeMatches(const std::string	&type)
{
  if (type == "4012")
    return (true);
  if (type == "7420")
    return (true);
  if (type == "74HC20")
    return (true);
  if (type == "74HCT20")
    return (true);
  if (type == "74LS20")
    return (true);
  if (type == "74HC4012")
    return (true);
  if (type == "CD4012")
    return (true);
  if (type == "HEF4012")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4012::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4012(timer, name, position));
}

std::string		hbs::CMP_4012::GetDisplayType(void) const
{
  return ("4012/7420 Dual 4-input NAND");
}

bool			hbs::CMP_4012::DrivesPin(size_t		n) const
{
  return (n == 1 || n == 13);
}

hbs::Tristate		hbs::CMP_4012::Compute(size_t		n)
{
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);
  Preset		p = PresetOutput(n);

  if (n == 1)
    {
      hbs::Tristate values[] = {GetPin(2), GetPin(3), GetPin(4), GetPin(5)};
      return (p->second = hbs::LogicNand(values, 4));
    }
  if (n == 13)
    {
      hbs::Tristate values[] = {GetPin(9), GetPin(10), GetPin(11), GetPin(12)};
      return (p->second = hbs::LogicNand(values, 4));
    }
  if (n == 7 || n == 14)
    return (p->second = hbs::UNDEFINED);
  return (p->second = GetPin(n));
}

hbs::CMP_4012::CMP_4012(const hbs::Timer	&timer,
			 const std::string	&name,
			 const std::string	&pos)
  : AComponent(timer, name, pos)
{}

hbs::CMP_4012::~CMP_4012(void)
{}
