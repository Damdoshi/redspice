// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74237.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74237::GetType(void) const
{
  return (type = "74237");
}

static const size_t	output_pin_74237[8] = {15, 14, 13, 12, 11, 10, 9, 7};

bool			hbs::CMP_74237::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 8; ++i)
    if (n == output_pin_74237[i])
      return (true);
  return (false);
}

void			hbs::CMP_74237::UpdateLatch(void)
{
  if (GetPin(4) == hbs::FALSE)
    {
      latched[0] = GetPin(1);
      latched[1] = GetPin(2);
      latched[2] = GetPin(3);
    }
  else if (!hbs::LogicKnown(GetPin(4)))
    {
      latched[0] = hbs::UNDEFINED;
      latched[1] = hbs::UNDEFINED;
      latched[2] = hbs::UNDEFINED;
    }
}

hbs::Tristate		hbs::CMP_74237::DecodeOutput(size_t n)
{
  unsigned int		addr;

  if (GetPin(5) == hbs::TRUE || GetPin(6) == hbs::FALSE)
    return (hbs::FALSE);
  if (!hbs::LogicKnown(GetPin(5)) || !hbs::LogicKnown(GetPin(6)))
    return (hbs::UNDEFINED);
  UpdateLatch();
  if (!hbs::LogicKnown(GetPin(4)))
    return (hbs::UNDEFINED);
  if (!hbs::LogicKnown(latched[0]) || !hbs::LogicKnown(latched[1]) || !hbs::LogicKnown(latched[2]))
    return (hbs::UNDEFINED);
  addr = (latched[0] == hbs::TRUE ? 1 : 0) | (latched[1] == hbs::TRUE ? 2 : 0) | (latched[2] == hbs::TRUE ? 4 : 0);
  return (n == output_pin_74237[addr] ? hbs::TRUE : hbs::FALSE);
}

hbs::Tristate		hbs::CMP_74237::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    return (p->second = DecodeOutput(n));
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74237::CMP_74237(const hbs::Timer	&time,
				  const std::string	&name,
				  const std::string	&pos)
  : AComponent(time, name, pos)
{
  for (size_t i = 0; i < 3; ++i)
    latched[i] = hbs::UNDEFINED;
}

hbs::CMP_74237::~CMP_74237(void)
{}

bool			hbs::CMP_74237::TypeMatches(const std::string &type)
{
  if (type == "74237" || type == "74HC237" || type == "74HCT237" || type == "74LS237")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74237::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74237(timer, name, position));
}

std::string		hbs::CMP_74237::GetDisplayType(void) const
{
  return ("74237 3-to-8 decoder/demultiplexer with address latch, active-high outputs");
}
