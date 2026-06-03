// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74157.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74157::GetType(void) const
{
  return (type = "74157");
}

static const size_t	a_pin[4] = {2, 5, 11, 14};
static const size_t	b_pin[4] = {3, 6, 10, 13};
static const size_t	out_pin[4] = {4, 7, 9, 12};

bool			hbs::CMP_74157::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 4; ++i)
    if (n == out_pin[i])
      return (true);
  return (false);
}

hbs::Tristate		hbs::CMP_74157::DisabledValue(void) const
{
  return (hbs::FALSE);
}

hbs::Tristate		hbs::CMP_74157::OutputValue(hbs::Tristate v) const
{
  return (v);
}

hbs::Tristate		hbs::CMP_74157::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  hbs::Tristate		sel;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      if (GetPin(15) == hbs::TRUE)
	return (p->second = DisabledValue());
      if (!hbs::LogicKnown(GetPin(15)))
	return (p->second = hbs::UNDEFINED);
      sel = GetPin(1);
      if (!hbs::LogicKnown(sel))
	return (p->second = hbs::UNDEFINED);
      for (size_t i = 0; i < 4; ++i)
	if (n == out_pin[i])
	  return (p->second = OutputValue(GetPin(sel == hbs::FALSE ? a_pin[i] : b_pin[i])));
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74157::CMP_74157(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74157::~CMP_74157(void)
{}

bool			hbs::CMP_74157::TypeMatches(const std::string &type)
{
  if (type == "74157" || type == "74HC157" || type == "74HCT157" || type == "74LS157")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74157::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74157(timer, name, position));
}

std::string		hbs::CMP_74157::GetDisplayType(void) const
{
  return ("74157 Quad 2-to-1 multiplexer");
}
