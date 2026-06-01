// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"Logic.hpp"
#include		"4073.hpp"

const std::string	&hbs::CMP_4073::GetType(void) const
{
  return (type = "4073");
}

bool			hbs::CMP_4073::TypeMatches(const std::string	&type)
{
  if (type == "4073")
    return (true);
  if (type == "7411")
    return (true);
  if (type == "74HC11")
    return (true);
  if (type == "74HCT11")
    return (true);
  if (type == "74LS11")
    return (true);
  if (type == "74HC4073")
    return (true);
  if (type == "CD4073")
    return (true);
  if (type == "HEF4073")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4073::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4073(timer, name, position));
}

std::string		hbs::CMP_4073::GetDisplayType(void) const
{
  return ("4073/7411 Triple 3-input AND");
}

bool			hbs::CMP_4073::DrivesPin(size_t		n) const
{
  return (n == 6 || n == 8 || n == 12);
}

hbs::Tristate		hbs::CMP_4073::Compute(size_t		n)
{
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);
  Preset		p = PresetOutput(n);

  if (n == 12)
    {
      hbs::Tristate values[] = {GetPin(1), GetPin(2), GetPin(13)};
      return (p->second = hbs::LogicAnd(values, 3));
    }
  if (n == 6)
    {
      hbs::Tristate values[] = {GetPin(3), GetPin(4), GetPin(5)};
      return (p->second = hbs::LogicAnd(values, 3));
    }
  if (n == 8)
    {
      hbs::Tristate values[] = {GetPin(9), GetPin(10), GetPin(11)};
      return (p->second = hbs::LogicAnd(values, 3));
    }
  if (n == 7 || n == 14)
    return (p->second = hbs::UNDEFINED);
  return (p->second = GetPin(n));
}

hbs::CMP_4073::CMP_4073(const hbs::Timer	&timer,
			 const std::string	&name,
			 const std::string	&pos)
  : AComponent(timer, name, pos)
{}

hbs::CMP_4073::~CMP_4073(void)
{}
