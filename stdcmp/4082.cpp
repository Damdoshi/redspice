// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"Logic.hpp"
#include		"4082.hpp"

const std::string	&hbs::CMP_4082::GetType(void) const
{
  return (type = "4082");
}

bool			hbs::CMP_4082::TypeMatches(const std::string	&type)
{
  if (type == "4082")
    return (true);
  if (type == "74HC4082")
    return (true);
  if (type == "CD4082")
    return (true);
  if (type == "HEF4082")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4082::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4082(timer, name, position));
}

std::string		hbs::CMP_4082::GetDisplayType(void) const
{
  return ("4082 Dual 4-input AND");
}

bool			hbs::CMP_4082::DrivesPin(size_t		n) const
{
  return (n == 1 || n == 13);
}

hbs::Tristate		hbs::CMP_4082::Compute(size_t		n)
{
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);
  Preset		p = PresetOutput(n);

  if (n == 1)
    {
      hbs::Tristate values[] = {GetPin(2), GetPin(3), GetPin(4), GetPin(5)};
      return (p->second = hbs::LogicAnd(values, 4));
    }
  if (n == 13)
    {
      hbs::Tristate values[] = {GetPin(9), GetPin(10), GetPin(11), GetPin(12)};
      return (p->second = hbs::LogicAnd(values, 4));
    }
  if (n == 7 || n == 14)
    return (p->second = hbs::UNDEFINED);
  return (p->second = GetPin(n));
}

hbs::CMP_4082::CMP_4082(const hbs::Timer	&timer,
			 const std::string	&name,
			 const std::string	&pos)
  : AComponent(timer, name, pos)
{}

hbs::CMP_4082::~CMP_4082(void)
{}
