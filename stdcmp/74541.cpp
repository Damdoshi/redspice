// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74541.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74541::GetType(void) const
{
  return (type = "74541");
}

static const size_t	a541[8] = {2, 3, 4, 5, 6, 7, 8, 9};
static const size_t	y541[8] = {18, 17, 16, 15, 14, 13, 12, 11};

bool			hbs::CMP_74541::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 8; ++i)
    if (n == y541[i])
      return (true);
  return (false);
}

hbs::Tristate		hbs::CMP_74541::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  hbs::Tristate		oe1;
  hbs::Tristate		oe2;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  for (size_t i = 0; i < 8; ++i)
    if (n == y541[i])
      {
	oe1 = GetPin(1);
	oe2 = GetPin(19);
	if (oe1 == hbs::FALSE && oe2 == hbs::FALSE)
	  return (p->second = GetPin(a541[i]));
	if (oe1 == hbs::TRUE || oe2 == hbs::TRUE)
	  return (p->second = hbs::HIGH_IMPEDANCE);
	return (p->second = hbs::UNDEFINED);
      }
  if (n == 10 || n == 20)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74541::CMP_74541(const hbs::Timer	&time,
				  const std::string	&name,
				  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74541::~CMP_74541(void)
{}

bool			hbs::CMP_74541::TypeMatches(const std::string &type)
{
  if (type == "74541" || type == "74HC541" || type == "74HCT541" || type == "74LS541")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74541::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74541(timer, name, position));
}

std::string		hbs::CMP_74541::GetDisplayType(void) const
{
  return ("74541 Octal buffer/line driver, 3-state outputs");
}
