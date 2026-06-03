// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74280.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74280::GetType(void) const
{
  return (type = "74280");
}

static const size_t	input_pin[9] = {8, 9, 10, 11, 12, 13, 1, 2, 4};

bool			hbs::CMP_74280::DrivesPin(size_t n) const
{
  return (n == 5 || n == 6);
}

static hbs::Tristate	parity(hbs::CMP_74280 &component)
{
  size_t		ones = 0;
  hbs::Tristate		v;

  for (size_t i = 0; i < 9; ++i)
    {
      v = component.GetPin(input_pin[i]);
      if (v == hbs::BROKEN)
	return (hbs::BROKEN);
      if (!hbs::LogicKnown(v))
	return (hbs::UNDEFINED);
      ones += v == hbs::TRUE;
    }
  return ((ones & 1) ? hbs::TRUE : hbs::FALSE);
}

hbs::Tristate		hbs::CMP_74280::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  hbs::Tristate		odd;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 5 || n == 6)
    {
      odd = parity(*this);
      if (odd == hbs::BROKEN || odd == hbs::UNDEFINED)
	return (p->second = odd);
      if (n == 6)
	return (p->second = odd);
      return (p->second = hbs::LogicNot(odd));
    }
  if (n == 3 || n == 7 || n == 14)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74280::CMP_74280(const hbs::Timer	&time,
				  const std::string	&name,
				  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74280::~CMP_74280(void)
{}

bool			hbs::CMP_74280::TypeMatches(const std::string &type)
{
  if (type == "74280" || type == "74HC280" || type == "74HCT280" || type == "74LS280")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74280::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74280(timer, name, position));
}

std::string		hbs::CMP_74280::GetDisplayType(void) const
{
  return ("74280 9-bit odd/even parity generator/checker");
}
