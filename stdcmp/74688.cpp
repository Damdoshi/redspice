// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74688.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74688::GetType(void) const
{
  return (type = "74688");
}

static const size_t	p_pin[8] = {2, 4, 6, 8, 11, 13, 15, 17};
static const size_t	q_pin[8] = {3, 5, 7, 9, 12, 14, 16, 18};

bool			hbs::CMP_74688::DrivesPin(size_t n) const
{
  return (n == 19);
}

hbs::Tristate		hbs::CMP_74688::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  hbs::Tristate		oe;
  hbs::Tristate		a;
  hbs::Tristate		b;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 19)
    {
      oe = GetPin(1);
      if (oe == hbs::TRUE)
	return (p->second = hbs::TRUE);
      if (!hbs::LogicKnown(oe))
	return (p->second = hbs::UNDEFINED);
      for (size_t i = 0; i < 8; ++i)
	{
	  a = GetPin(p_pin[i]);
	  b = GetPin(q_pin[i]);
	  if (a == hbs::BROKEN || b == hbs::BROKEN)
	    return (p->second = hbs::BROKEN);
	  if (!hbs::LogicKnown(a) || !hbs::LogicKnown(b))
	    return (p->second = hbs::UNDEFINED);
	  if (a != b)
	    return (p->second = hbs::TRUE);
	}
      return (p->second = hbs::FALSE);
    }
  if (n == 10 || n == 20)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74688::CMP_74688(const hbs::Timer	&time,
				  const std::string	&name,
				  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74688::~CMP_74688(void)
{}

bool			hbs::CMP_74688::TypeMatches(const std::string &type)
{
  if (type == "74688" || type == "74HC688" || type == "74HCT688" || type == "74LS688")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74688::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74688(timer, name, position));
}

std::string		hbs::CMP_74688::GetDisplayType(void) const
{
  return ("74688 8-bit identity comparator, active-low equality output");
}
