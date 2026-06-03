// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4053.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_4053::GetType(void) const
{
  return (type = "4053");
}

struct				Switch4053Pins
{
  size_t		common;
  size_t		x;
  size_t		y;
  size_t		select;
};

static const Switch4053Pins switches_4053[3] = {
  {14, 12, 13, 11},
  {15, 2, 1, 10},
  {4, 5, 3, 9}
};

bool			hbs::CMP_4053::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 3; ++i)
    if (n == switches_4053[i].common ||
	n == switches_4053[i].x ||
	n == switches_4053[i].y)
      return (true);
  return (false);
}

hbs::Tristate		hbs::CMP_4053::SwitchValue(size_t n)
{
  hbs::Tristate		inh = GetPin(6);
  hbs::Tristate		sel;
  size_t		selected;

  if (inh == hbs::BROKEN)
    return (hbs::BROKEN);
  if (!hbs::LogicKnown(inh))
    return (hbs::UNDEFINED);
  if (inh == hbs::TRUE)
    return (hbs::HIGH_IMPEDANCE);
  for (size_t i = 0; i < 3; ++i)
    if (n == switches_4053[i].common ||
	n == switches_4053[i].x ||
	n == switches_4053[i].y)
      {
	sel = GetPin(switches_4053[i].select);
	if (!hbs::LogicKnown(sel))
	  return (sel == hbs::BROKEN ? hbs::BROKEN : hbs::UNDEFINED);
	selected = sel == hbs::TRUE ? switches_4053[i].y : switches_4053[i].x;
	if (n == switches_4053[i].common)
	  return (GetPin(selected));
	if (n == selected)
	  return (GetPin(switches_4053[i].common));
	return (hbs::HIGH_IMPEDANCE);
      }
  return (hbs::UNDEFINED);
}

hbs::Tristate		hbs::CMP_4053::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    return (p->second = SwitchValue(n));
  if (n == 7 || n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4053::CMP_4053(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_4053::~CMP_4053(void)
{}

bool			hbs::CMP_4053::TypeMatches(const std::string &type)
{
  if (type == "4053" || type == "74HC4053" || type == "74HCT4053")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4053::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4053(timer, name, position));
}

std::string		hbs::CMP_4053::GetDisplayType(void) const
{
  return ("4053 Triple 2-channel analog switch/multiplexer");
}
