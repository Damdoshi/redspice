// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4066.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_4066::GetType(void) const
{
  return (type = "4066");
}

struct				SwitchPins
{
  size_t		a;
  size_t		b;
  size_t		enable;
};

static const SwitchPins	switches[4] = {
  {1, 2, 6},
  {4, 3, 5},
  {12, 13, 8},
  {10, 11, 9}
};

bool			hbs::CMP_4066::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 4; ++i)
    if (n == switches[i].a || n == switches[i].b)
      return (true);
  return (false);
}

hbs::Tristate		hbs::CMP_4066::SwitchValue(size_t n)
{
  hbs::Tristate		enable;

  for (size_t i = 0; i < 4; ++i)
    if (n == switches[i].a || n == switches[i].b)
      {
	enable = GetPin(switches[i].enable);
	if (enable == hbs::BROKEN)
	  return (hbs::BROKEN);
	if (!hbs::LogicKnown(enable))
	  return (hbs::UNDEFINED);
	if (enable == hbs::FALSE)
	  return (hbs::HIGH_IMPEDANCE);
	return (GetPin(n == switches[i].a ? switches[i].b : switches[i].a));
      }
  return (hbs::UNDEFINED);
}

hbs::Tristate		hbs::CMP_4066::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    return (p->second = SwitchValue(n));
  if (n == 7 || n == 14)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4066::CMP_4066(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_4066::~CMP_4066(void)
{}

bool			hbs::CMP_4066::TypeMatches(const std::string &type)
{
  if (type == "4066" || type == "4016")
    return (true);
  if (type == "74HC4066" || type == "74HCT4066")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4066::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4066(timer, name, position));
}

std::string		hbs::CMP_4066::GetDisplayType(void) const
{
  return ("4016/4066 Quad bilateral switch");
}
