// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74244.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74244::GetType(void) const
{
  return (type = "74244");
}

static const size_t	a244[8] = {2, 4, 6, 8, 11, 13, 15, 17};
static const size_t	y244[8] = {18, 16, 14, 12, 9, 7, 5, 3};

bool			hbs::CMP_74244::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 8; ++i)
    if (n == y244[i])
      return (true);
  return (false);
}

hbs::Tristate		hbs::CMP_74244::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  hbs::Tristate		oe;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  for (size_t i = 0; i < 8; ++i)
    if (n == y244[i])
      {
	oe = GetPin(i < 4 ? 1 : 19);
	if (oe == hbs::FALSE)
	  return (p->second = GetPin(a244[i]));
	if (oe == hbs::TRUE)
	  return (p->second = hbs::HIGH_IMPEDANCE);
	return (p->second = hbs::UNDEFINED);
      }
  if (n == 10 || n == 20)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74244::CMP_74244(const hbs::Timer	&time,
				  const std::string	&name,
				  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74244::~CMP_74244(void)
{}

bool			hbs::CMP_74244::TypeMatches(const std::string &type)
{
  if (type == "74244" || type == "74HC244" || type == "74HCT244" || type == "74LS244")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74244::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74244(timer, name, position));
}

std::string		hbs::CMP_74244::GetDisplayType(void) const
{
  return ("74244 Octal buffer/line driver, 3-state outputs");
}
