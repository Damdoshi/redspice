// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74540.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74540::GetType(void) const
{
  return (type = "74540");
}

static const size_t	a540[8] = {2, 3, 4, 5, 6, 7, 8, 9};
static const size_t	y540[8] = {18, 17, 16, 15, 14, 13, 12, 11};

bool			hbs::CMP_74540::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 8; ++i)
    if (n == y540[i])
      return (true);
  return (false);
}

hbs::Tristate		hbs::CMP_74540::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  hbs::Tristate		oe1;
  hbs::Tristate		oe2;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  for (size_t i = 0; i < 8; ++i)
    if (n == y540[i])
      {
	oe1 = GetPin(1);
	oe2 = GetPin(19);
	if (oe1 == hbs::FALSE && oe2 == hbs::FALSE)
	  return (p->second = hbs::LogicNot(GetPin(a540[i])));
	if (oe1 == hbs::TRUE || oe2 == hbs::TRUE)
	  return (p->second = hbs::HIGH_IMPEDANCE);
	return (p->second = hbs::UNDEFINED);
      }
  if (n == 10 || n == 20)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74540::CMP_74540(const hbs::Timer	&time,
				  const std::string	&name,
				  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74540::~CMP_74540(void)
{}

bool			hbs::CMP_74540::TypeMatches(const std::string &type)
{
  if (type == "74540" || type == "74HC540" || type == "74HCT540" || type == "74LS540")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74540::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74540(timer, name, position));
}

std::string		hbs::CMP_74540::GetDisplayType(void) const
{
  return ("74540 Octal inverting buffer/line driver, 3-state outputs");
}
