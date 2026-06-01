// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4555.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_4555::GetType(void) const
{
  return (type = "4555");
}

static int		output_index(size_t n)
{
  if (n >= 4 && n <= 7)
    return (n - 4);
  if (n >= 9 && n <= 12)
    return (12 - n);
  return (-1);
}

static bool		read_decoder(hbs::CMP_4555 &component,
				     size_t n,
				     unsigned char &value,
				     bool &enabled)
{
  hbs::Tristate		enable;
  hbs::Tristate		a;
  hbs::Tristate		b;

  if (n >= 4 && n <= 7)
    {
      enable = component.GetPin(1);
      a = component.GetPin(2);
      b = component.GetPin(3);
    }
  else
    {
      enable = component.GetPin(15);
      a = component.GetPin(14);
      b = component.GetPin(13);
    }
  if (!hbs::LogicKnown(enable))
    return (false);
  enabled = enable == hbs::FALSE;
  if (!enabled)
    return (true);
  if (!hbs::LogicKnown(a) || !hbs::LogicKnown(b))
    return (false);
  value = 0;
  if (a == hbs::TRUE)
    value |= 1 << 0;
  if (b == hbs::TRUE)
    value |= 1 << 1;
  return (true);
}

hbs::Tristate		hbs::CMP_4555::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  int			out;
  unsigned char		value;
  bool			enabled;
  bool			selected;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  out = output_index(n);
  if (out >= 0)
    {
      if (!read_decoder(*this, n, value, enabled))
	return (p->second = hbs::UNDEFINED);
      selected = enabled && value == (unsigned char)out;
      if (ActiveLowOutputs())
	return (p->second = selected ? hbs::FALSE : hbs::TRUE);
      return (p->second = selected ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4555::CMP_4555(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_4555::~CMP_4555(void)
{}

bool			hbs::CMP_4555::TypeMatches(const std::string &type)
{
  if (type == "4555")
    return (true);
  if (type == "74HC4555")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4555::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4555(timer, name, position));
}

std::string		hbs::CMP_4555::GetDisplayType(void) const
{
  return ("4555 Dual 2-to-4 decoder/demultiplexer, active-high outputs");
}
