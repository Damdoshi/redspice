// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"Logic.hpp"
#include		"4085.hpp"

const std::string	&hbs::CMP_4085::GetType(void) const
{
  return (type = "4085");
}

bool			hbs::CMP_4085::TypeMatches(const std::string	&type)
{
  if (type == "4085")
    return (true);
  if (type == "74HC51")
    return (true);
  if (type == "7451")
    return (true);
  if (type == "74LS51")
    return (true);
  if (type == "CD4085")
    return (true);
  if (type == "HEF4085")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4085::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4085(timer, name, position));
}

std::string		hbs::CMP_4085::GetDisplayType(void) const
{
  return ("4085/7451 Dual 2-wide 2-input AND-OR-INVERT");
}

bool			hbs::CMP_4085::DrivesPin(size_t		n) const
{
  return (n == 3 || n == 4);
}

hbs::Tristate		hbs::CMP_4085::Compute(size_t		n)
{
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);
  Preset		p = PresetOutput(n);

  if (n == 3)
    {
      hbs::Tristate ab[] = {GetPin(1), GetPin(2)};
      hbs::Tristate cd[] = {GetPin(12), GetPin(13)};
      hbs::Tristate values[] = {GetPin(10), hbs::LogicAnd(ab, 2), hbs::LogicAnd(cd, 2)};
      return (p->second = hbs::LogicNor(values, 3));
    }
  if (n == 4)
    {
      hbs::Tristate ab[] = {GetPin(5), GetPin(6)};
      hbs::Tristate cd[] = {GetPin(8), GetPin(9)};
      hbs::Tristate values[] = {GetPin(11), hbs::LogicAnd(ab, 2), hbs::LogicAnd(cd, 2)};
      return (p->second = hbs::LogicNor(values, 3));
    }
  if (n == 7 || n == 14)
    return (p->second = hbs::UNDEFINED);
  return (p->second = GetPin(n));
}

hbs::CMP_4085::CMP_4085(const hbs::Timer	&timer,
				 const std::string	&name,
				 const std::string	&pos)
  : AComponent(timer, name, pos)
{}

hbs::CMP_4085::~CMP_4085(void)
{}
