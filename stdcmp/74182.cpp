// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74182.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74182::GetType(void) const
{
  return (type = "74182");
}

static const size_t	gpin[4] = {2, 4, 6, 10};
static const size_t	ppin[4] = {3, 5, 7, 11};

hbs::Tristate		hbs::CMP_74182::Carry(size_t stage)
{
  hbs::Tristate		c;

  c = GetPin(13);
  for (size_t i = 0; i < stage; ++i)
    {
      if (!hbs::LogicKnown(c) || !hbs::LogicKnown(GetPin(gpin[i])) || !hbs::LogicKnown(GetPin(ppin[i])))
	return (hbs::UNDEFINED);
      c = (GetPin(gpin[i]) == hbs::TRUE || (GetPin(ppin[i]) == hbs::TRUE && c == hbs::TRUE)) ? hbs::TRUE : hbs::FALSE;
    }
  return (c);
}

hbs::Tristate		hbs::CMP_74182::GroupPropagate(void)
{
  for (size_t i = 0; i < 4; ++i)
    if (GetPin(ppin[i]) != hbs::TRUE)
      return (GetPin(ppin[i]) == hbs::FALSE ? hbs::FALSE : hbs::UNDEFINED);
  return (hbs::TRUE);
}

hbs::Tristate		hbs::CMP_74182::GroupGenerate(void)
{
  hbs::Tristate		c;

  c = Carry(4);
  return (c);
}

bool			hbs::CMP_74182::DrivesPin(size_t n) const
{
  return (n == 1 || n == 8 || n == 9 || n == 12 || n == 14 || n == 15);
}

hbs::Tristate		hbs::CMP_74182::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 1)
    return (p->second = Carry(3));
  if (n == 14)
    return (p->second = Carry(1));
  if (n == 15)
    return (p->second = Carry(2));
  if (n == 12)
    return (p->second = Carry(4));
  if (n == 9)
    return (p->second = GroupPropagate());
  if (n == 8)
    return (p->second = GroupGenerate());
  if (n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74182::CMP_74182(const hbs::Timer	&time,
				  const std::string	&name,
				  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74182::~CMP_74182(void)
{}

bool			hbs::CMP_74182::TypeMatches(const std::string &type)
{
  if (type == "74182" || type == "74HC182" || type == "74HCT182" || type == "74LS182")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74182::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74182(timer, name, position));
}

std::string		hbs::CMP_74182::GetDisplayType(void) const
{
  return ("74182 Carry lookahead generator");
}
