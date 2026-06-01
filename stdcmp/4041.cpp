// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"Logic.hpp"
#include		"4041.hpp"

const std::string	&hbs::CMP_4041::GetType(void) const
{
  return (type = "4041");
}

bool			hbs::CMP_4041::TypeMatches(const std::string	&type)
{
  if (type == "4041")
    return (true);
  if (type == "74HC4041")
    return (true);
  if (type == "CD4041")
    return (true);
  if (type == "HEF4041")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4041::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4041(timer, name, position));
}

std::string		hbs::CMP_4041::GetDisplayType(void) const
{
  return ("4041 Quad true/complement buffer");
}

bool			hbs::CMP_4041::DrivesPin(size_t		n) const
{
  return (n == 2 || n == 3 || n == 5 || n == 6 ||
	  n == 8 || n == 10 || n == 11 || n == 13);
}

hbs::Tristate		hbs::CMP_4041::Compute(size_t		n)
{
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);
  Preset		p = PresetOutput(n);

  if (n == 2)
    return (p->second = GetPin(1));
  if (n == 3)
    return (p->second = hbs::LogicNot(GetPin(1)));
  if (n == 5)
    return (p->second = GetPin(4));
  if (n == 6)
    return (p->second = hbs::LogicNot(GetPin(4)));
  if (n == 8)
    return (p->second = GetPin(9));
  if (n == 10)
    return (p->second = hbs::LogicNot(GetPin(9)));
  if (n == 13)
    return (p->second = GetPin(12));
  if (n == 11)
    return (p->second = hbs::LogicNot(GetPin(12)));
  if (n == 7 || n == 14)
    return (p->second = hbs::UNDEFINED);
  return (p->second = GetPin(n));
}

hbs::CMP_4041::CMP_4041(const hbs::Timer	&timer,
			 const std::string	&name,
			 const std::string	&pos)
  : AComponent(timer, name, pos)
{}

hbs::CMP_4041::~CMP_4041(void)
{}
