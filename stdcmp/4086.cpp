// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"Logic.hpp"
#include		"4086.hpp"

const std::string	&hbs::CMP_4086::GetType(void) const
{
  return (type = "4086");
}

bool			hbs::CMP_4086::TypeMatches(const std::string	&type)
{
  if (type == "4086")
    return (true);
  if (type == "74HC55")
    return (true);
  if (type == "7455")
    return (true);
  if (type == "74LS55")
    return (true);
  if (type == "CD4086")
    return (true);
  if (type == "HEF4086")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4086::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4086(timer, name, position));
}

std::string		hbs::CMP_4086::GetDisplayType(void) const
{
  return ("4086/7455 Expandable 4-wide 2-input AND-OR-INVERT");
}

bool			hbs::CMP_4086::DrivesPin(size_t		n) const
{
  return (n == 3);
}

hbs::Tristate		hbs::CMP_4086::Compute(size_t		n)
{
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);
  Preset		p = PresetOutput(n);

  if (n == 3)
    {
      hbs::Tristate ab[] = {GetPin(1), GetPin(2)};
      hbs::Tristate cd[] = {GetPin(12), GetPin(13)};
      hbs::Tristate ef[] = {GetPin(5), GetPin(6)};
      hbs::Tristate gh[] = {GetPin(8), GetPin(9)};
      hbs::Tristate sums[] = {GetPin(10), hbs::LogicAnd(ab, 2), hbs::LogicAnd(cd, 2), hbs::LogicAnd(ef, 2), hbs::LogicAnd(gh, 2)};
      hbs::Tristate en = GetPin(11);

      if (en == hbs::BROKEN)
	return (p->second = hbs::BROKEN);
      if (en == hbs::FALSE)
	return (p->second = hbs::FALSE);
      if (en != hbs::TRUE)
	return (p->second = hbs::UNDEFINED);
      return (p->second = hbs::LogicNor(sums, 5));
    }
  if (n == 4 || n == 7 || n == 14)
    return (p->second = hbs::UNDEFINED);
  return (p->second = GetPin(n));
}

hbs::CMP_4086::CMP_4086(const hbs::Timer	&timer,
				 const std::string	&name,
				 const std::string	&pos)
  : AComponent(timer, name, pos)
{}

hbs::CMP_4086::~CMP_4086(void)
{}
