// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74155.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74155::GetType(void) const
{
  return (type = "74155");
}

static const size_t	output_a_74155[4] = {7, 6, 5, 4};
static const size_t	output_b_74155[4] = {9, 10, 11, 12};

bool			hbs::CMP_74155::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 4; ++i)
    if (n == output_a_74155[i] || n == output_b_74155[i])
      return (true);
  return (false);
}

hbs::Tristate		hbs::CMP_74155::DecodeA(size_t n)
{
  unsigned int		addr;

  if (GetPin(2) == hbs::TRUE || GetPin(1) == hbs::FALSE)
    return (hbs::TRUE);
  if (!hbs::LogicKnown(GetPin(2)) || !hbs::LogicKnown(GetPin(1)) || !hbs::LogicKnown(GetPin(13)) || !hbs::LogicKnown(GetPin(3)))
    return (hbs::UNDEFINED);
  addr = (GetPin(13) == hbs::TRUE ? 1 : 0) | (GetPin(3) == hbs::TRUE ? 2 : 0);
  return (n == output_a_74155[addr] ? hbs::FALSE : hbs::TRUE);
}

hbs::Tristate		hbs::CMP_74155::DecodeB(size_t n)
{
  unsigned int		addr;

  if (GetPin(14) == hbs::TRUE || GetPin(15) == hbs::TRUE)
    return (hbs::TRUE);
  if (!hbs::LogicKnown(GetPin(14)) || !hbs::LogicKnown(GetPin(15)) || !hbs::LogicKnown(GetPin(13)) || !hbs::LogicKnown(GetPin(3)))
    return (hbs::UNDEFINED);
  addr = (GetPin(13) == hbs::TRUE ? 1 : 0) | (GetPin(3) == hbs::TRUE ? 2 : 0);
  return (n == output_b_74155[addr] ? hbs::FALSE : hbs::TRUE);
}

hbs::Tristate		hbs::CMP_74155::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  for (size_t i = 0; i < 4; ++i)
    if (n == output_a_74155[i])
      return (p->second = DecodeA(n));
  for (size_t i = 0; i < 4; ++i)
    if (n == output_b_74155[i])
      return (p->second = DecodeB(n));
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74155::CMP_74155(const hbs::Timer	&time,
				  const std::string	&name,
				  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74155::~CMP_74155(void)
{}

bool			hbs::CMP_74155::TypeMatches(const std::string &type)
{
  if (type == "74155" || type == "74HC155" || type == "74HCT155" || type == "74LS155")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74155::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74155(timer, name, position));
}

std::string		hbs::CMP_74155::GetDisplayType(void) const
{
  return ("74155 Dual 2-to-4 decoder/demultiplexer, active-low outputs");
}
