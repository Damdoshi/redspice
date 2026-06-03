// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74241.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74241::GetType(void) const
{
  return (type = "74241");
}

static const size_t	a241[8] = {2, 4, 6, 8, 11, 13, 15, 17};
static const size_t	y241[8] = {18, 16, 14, 12, 9, 7, 5, 3};

bool			hbs::CMP_74241::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 8; ++i)
    if (n == y241[i])
      return (true);
  return (false);
}

hbs::Tristate		hbs::CMP_74241::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  hbs::Tristate		oe;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  for (size_t i = 0; i < 8; ++i)
    if (n == y241[i])
      {
	oe = GetPin(i < 4 ? 1 : 19);
	if ((i < 4 && oe == hbs::FALSE) || (i >= 4 && oe == hbs::TRUE))
	  return (p->second = GetPin(a241[i]));
	if ((i < 4 && oe == hbs::TRUE) || (i >= 4 && oe == hbs::FALSE))
	  return (p->second = hbs::HIGH_IMPEDANCE);
	return (p->second = hbs::UNDEFINED);
      }
  if (n == 10 || n == 20)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74241::CMP_74241(const hbs::Timer	&time,
				  const std::string	&name,
				  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74241::~CMP_74241(void)
{}

bool			hbs::CMP_74241::TypeMatches(const std::string &type)
{
  if (type == "74241" || type == "74HC241" || type == "74HCT241" || type == "74LS241")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74241::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74241(timer, name, position));
}

std::string		hbs::CMP_74241::GetDisplayType(void) const
{
  return ("74241 Octal buffer/line driver, 3-state outputs, mixed enables");
}
