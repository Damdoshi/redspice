// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4051.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_4051::GetType(void) const
{
  return (type = "4051");
}

static const size_t	channel_pin[8] = {13, 14, 15, 12, 1, 5, 2, 4};

bool			hbs::CMP_4051::DrivesPin(size_t n) const
{
  if (n == 3)
    return (true);
  for (size_t i = 0; i < 8; ++i)
    if (n == channel_pin[i])
      return (true);
  return (false);
}

size_t			hbs::CMP_4051::SelectedChannel(void)
{
  hbs::Tristate		a = GetPin(11);
  hbs::Tristate		b = GetPin(10);
  hbs::Tristate		c = GetPin(9);

  if (!hbs::LogicKnown(a) || !hbs::LogicKnown(b) || !hbs::LogicKnown(c))
    return (8);
  return ((a == hbs::TRUE ? 1 : 0) |
	  (b == hbs::TRUE ? 2 : 0) |
	  (c == hbs::TRUE ? 4 : 0));
}

hbs::Tristate		hbs::CMP_4051::SwitchValue(size_t n)
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
  if (selected >= 8)
    return (hbs::UNDEFINED);
  if (n == 3)
    return (GetPin(channel_pin[selected]));
  if (n == channel_pin[selected])
    return (GetPin(3));
  return (hbs::HIGH_IMPEDANCE);
}

hbs::Tristate		hbs::CMP_4051::Compute(size_t n)
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

hbs::CMP_4051::CMP_4051(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_4051::~CMP_4051(void)
{}

bool			hbs::CMP_4051::TypeMatches(const std::string &type)
{
  if (type == "4051" || type == "74HC4051" || type == "74HCT4051")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4051::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4051(timer, name, position));
}

std::string		hbs::CMP_4051::GetDisplayType(void) const
{
  return ("4051 8-channel analog multiplexer/demultiplexer");
}
