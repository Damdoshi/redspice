// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74126.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74126::GetType(void) const
{
  return (type = "74126");
}

static const size_t	oe_pin_126[4] = {1, 4, 10, 13};
static const size_t	in_pin_126[4] = {2, 5, 9, 12};
static const size_t	out_pin_126[4] = {3, 6, 8, 11};

bool			hbs::CMP_74126::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 4; ++i)
    if (n == out_pin_126[i])
      return (true);
  return (false);
}

hbs::Tristate		hbs::CMP_74126::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  hbs::Tristate		oe;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  for (size_t i = 0; i < 4; ++i)
    if (n == out_pin_126[i])
      {
	oe = GetPin(oe_pin_126[i]);
	if (oe == hbs::TRUE)
	  return (p->second = GetPin(in_pin_126[i]));
	if (oe == hbs::FALSE)
	  return (p->second = hbs::HIGH_IMPEDANCE);
	return (p->second = hbs::UNDEFINED);
      }
  if (n == 7 || n == 14)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74126::CMP_74126(const hbs::Timer	&time,
				  const std::string	&name,
				  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74126::~CMP_74126(void)
{}

bool			hbs::CMP_74126::TypeMatches(const std::string &type)
{
  if (type == "74126" || type == "74HC126" || type == "74HCT126" || type == "74LS126")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74126::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74126(timer, name, position));
}

std::string		hbs::CMP_74126::GetDisplayType(void) const
{
  return ("74126 Quad buffer, 3-state outputs, active-high enable");
}
