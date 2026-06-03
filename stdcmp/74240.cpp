// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74240.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74240::GetType(void) const
{
  return (type = "74240");
}

static const size_t	a240[8] = {2, 4, 6, 8, 11, 13, 15, 17};
static const size_t	y240[8] = {18, 16, 14, 12, 9, 7, 5, 3};

bool			hbs::CMP_74240::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 8; ++i)
    if (n == y240[i])
      return (true);
  return (false);
}

hbs::Tristate		hbs::CMP_74240::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  hbs::Tristate		oe;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  for (size_t i = 0; i < 8; ++i)
    if (n == y240[i])
      {
	oe = GetPin(i < 4 ? 1 : 19);
	if (oe == hbs::FALSE)
	  return (p->second = hbs::LogicNot(GetPin(a240[i])));
	if (oe == hbs::TRUE)
	  return (p->second = hbs::HIGH_IMPEDANCE);
	return (p->second = hbs::UNDEFINED);
      }
  if (n == 10 || n == 20)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74240::CMP_74240(const hbs::Timer	&time,
				  const std::string	&name,
				  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74240::~CMP_74240(void)
{}

bool			hbs::CMP_74240::TypeMatches(const std::string &type)
{
  if (type == "74240" || type == "74HC240" || type == "74HCT240" || type == "74LS240")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74240::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74240(timer, name, position));
}

std::string		hbs::CMP_74240::GetDisplayType(void) const
{
  return ("74240 Octal inverting buffer/line driver, 3-state outputs");
}
