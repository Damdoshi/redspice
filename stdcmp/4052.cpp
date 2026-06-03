// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4052.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_4052::GetType(void) const
{
  return (type = "4052");
}

static const size_t	x_channel_pin[4] = {12, 14, 15, 11};
static const size_t	y_channel_pin[4] = {1, 5, 2, 4};
static const size_t	x_common_pin = 13;
static const size_t	y_common_pin = 3;

bool			hbs::CMP_4052::DrivesPin(size_t n) const
{
  if (n == x_common_pin || n == y_common_pin)
    return (true);
  for (size_t i = 0; i < 4; ++i)
    if (n == x_channel_pin[i] || n == y_channel_pin[i])
      return (true);
  return (false);
}

size_t			hbs::CMP_4052::SelectedChannel(void)
{
  hbs::Tristate		a = GetPin(10);
  hbs::Tristate		b = GetPin(9);

  if (!hbs::LogicKnown(a) || !hbs::LogicKnown(b))
    return (4);
  return ((a == hbs::TRUE ? 1 : 0) |
	  (b == hbs::TRUE ? 2 : 0));
}

hbs::Tristate		hbs::CMP_4052::SwitchValue(size_t n)
{
  hbs::Tristate		inh = GetPin(6);
  size_t		selected;

  if (inh == hbs::BROKEN)
    return (hbs::BROKEN);
  if (!hbs::LogicKnown(inh))
    return (hbs::UNDEFINED);
  if (inh == hbs::TRUE)
    return (hbs::HIGH_IMPEDANCE);
  selected = SelectedChannel();
  if (selected >= 4)
    return (hbs::UNDEFINED);
  if (n == x_common_pin)
    return (GetPin(x_channel_pin[selected]));
  if (n == y_common_pin)
    return (GetPin(y_channel_pin[selected]));
  if (n == x_channel_pin[selected])
    return (GetPin(x_common_pin));
  if (n == y_channel_pin[selected])
    return (GetPin(y_common_pin));
  return (hbs::HIGH_IMPEDANCE);
}

hbs::Tristate		hbs::CMP_4052::Compute(size_t n)
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

hbs::CMP_4052::CMP_4052(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_4052::~CMP_4052(void)
{}

bool			hbs::CMP_4052::TypeMatches(const std::string &type)
{
  if (type == "4052" || type == "74HC4052" || type == "74HCT4052")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4052::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4052(timer, name, position));
}

std::string		hbs::CMP_4052::GetDisplayType(void) const
{
  return ("4052 Dual 4-channel analog multiplexer/demultiplexer");
}
