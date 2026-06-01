// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4063.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_4063::GetType(void) const
{
  return (type = "4063");
}

static bool		read_word(hbs::CMP_4063 &component, bool a_word, unsigned char &value)
{
  static const size_t apins[4] = {10, 12, 13, 15};
  static const size_t bpins[4] = {9, 11, 14, 1};
  const size_t		*pins = a_word ? apins : bpins;
  hbs::Tristate		v;

  value = 0;
  for (size_t i = 0; i < 4; ++i)
    {
      v = component.GetPin(pins[i]);
      if (!hbs::LogicKnown(v))
	return (false);
      if (v == hbs::TRUE)
	value |= 1 << i;
    }
  return (true);
}

hbs::Tristate		hbs::CMP_4063::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  unsigned char		a;
  unsigned char		b;
  hbs::Tristate		gt;
  hbs::Tristate		eq;
  hbs::Tristate		lt;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n >= 5 && n <= 7)
    {
      if (!read_word(*this, true, a) || !read_word(*this, false, b))
	return (p->second = hbs::UNDEFINED);
      if (a > b)
	{
	  gt = hbs::TRUE;
	  eq = hbs::FALSE;
	  lt = hbs::FALSE;
	}
      else if (a < b)
	{
	  gt = hbs::FALSE;
	  eq = hbs::FALSE;
	  lt = hbs::TRUE;
	}
      else
	{
	  gt = GetPin(4);
	  eq = GetPin(3);
	  lt = GetPin(2);
	  if (!hbs::LogicKnown(gt) || !hbs::LogicKnown(eq) || !hbs::LogicKnown(lt))
	    return (p->second = hbs::UNDEFINED);
	}
      if (n == 5)
	return (p->second = gt);
      if (n == 6)
	return (p->second = eq);
      return (p->second = lt);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4063::CMP_4063(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_4063::~CMP_4063(void)
{}

bool			hbs::CMP_4063::TypeMatches(const std::string &type)
{
  if (type == "4063")
    return (true);
  if (type == "7485")
    return (true);
  if (type == "74HC85")
    return (true);
  if (type == "74LS85")
    return (true);
  if (type == "74HC4063")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4063::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4063(timer, name, position));
}

std::string		hbs::CMP_4063::GetDisplayType(void) const
{
  return ("4063/7485 4-bit magnitude comparator");
}
