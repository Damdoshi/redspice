// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74245.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74245::GetType(void) const
{
  return (type = "74245");
}

static const size_t	a245[8] = {2, 3, 4, 5, 6, 7, 8, 9};
static const size_t	b245[8] = {18, 17, 16, 15, 14, 13, 12, 11};

bool			hbs::CMP_74245::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 8; ++i)
    if (n == a245[i] || n == b245[i])
      return (true);
  return (false);
}

hbs::Tristate		hbs::CMP_74245::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  hbs::Tristate		oe;
  hbs::Tristate		dir;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  oe = GetPin(19);
  dir = GetPin(1);
  for (size_t i = 0; i < 8; ++i)
    {
      if (n == b245[i])
	{
	  if (oe == hbs::TRUE)
	    return (p->second = hbs::HIGH_IMPEDANCE);
	  if (oe == hbs::FALSE && dir == hbs::TRUE)
	    return (p->second = GetPin(a245[i]));
	  if (oe == hbs::FALSE && dir == hbs::FALSE)
	    return (p->second = hbs::HIGH_IMPEDANCE);
	  return (p->second = hbs::UNDEFINED);
	}
      if (n == a245[i])
	{
	  if (oe == hbs::TRUE)
	    return (p->second = hbs::HIGH_IMPEDANCE);
	  if (oe == hbs::FALSE && dir == hbs::FALSE)
	    return (p->second = GetPin(b245[i]));
	  if (oe == hbs::FALSE && dir == hbs::TRUE)
	    return (p->second = hbs::HIGH_IMPEDANCE);
	  return (p->second = hbs::UNDEFINED);
	}
    }
  if (n == 10 || n == 20)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74245::CMP_74245(const hbs::Timer	&time,
				  const std::string	&name,
				  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74245::~CMP_74245(void)
{}

bool			hbs::CMP_74245::TypeMatches(const std::string &type)
{
  if (type == "74245" || type == "74HC245" || type == "74HCT245" || type == "74LS245")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74245::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74245(timer, name, position));
}

std::string		hbs::CMP_74245::GetDisplayType(void) const
{
  return ("74245 Octal bus transceiver, 3-state outputs");
}
