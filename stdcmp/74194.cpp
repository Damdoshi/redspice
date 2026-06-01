// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74194.hpp"

const std::string	&hbs::CMP_74194::GetType(void) const
{
  return (type = "74194");
}

static const size_t	parallel_pin[4] = {3, 4, 5, 6};
static const size_t	output_pin[4] = {15, 14, 13, 12};

bool			hbs::CMP_74194::DrivesPin(size_t n) const
{
  return (n >= 12 && n <= 15);
}

void			hbs::CMP_74194::Tick(void)
{
  hbs::Tristate		s0;
  hbs::Tristate		s1;

  if (GetPin(1) == hbs::FALSE)
    {
      value.reset();
      return ;
    }
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(11) != hbs::TRUE || GetPrevious(11) != hbs::FALSE)
    return ;
  last_tick = timer.GetTime();
  s0 = GetPin(9);
  s1 = GetPin(10);
  if (s0 == hbs::FALSE && s1 == hbs::FALSE)
    return ;
  if (s0 == hbs::TRUE && s1 == hbs::TRUE)
    {
      for (size_t i = 0; i < 4; ++i)
	if (GetPin(parallel_pin[i]) == hbs::TRUE)
	  value.set(i);
	else
	  value.reset(i);
      return ;
    }
  if (s0 == hbs::TRUE && s1 == hbs::FALSE)
    {
      value >>= 1;
      if (GetPin(7) == hbs::TRUE)
	value.set(3);
      else
	value.reset(3);
      return ;
    }
  if (s0 == hbs::FALSE && s1 == hbs::TRUE)
    {
      value <<= 1;
      if (GetPin(2) == hbs::TRUE)
	value.set(0);
      else
	value.reset(0);
    }
}

hbs::Tristate		hbs::CMP_74194::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      Tick();
      for (size_t i = 0; i < 4; ++i)
	if (n == output_pin[i])
	  return (p->second = value[i] ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74194::CMP_74194(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : AComponent(time, name, pos),
    last_tick(0)
{}

hbs::CMP_74194::~CMP_74194(void)
{}

bool			hbs::CMP_74194::TypeMatches(const std::string &type)
{
  if (type == "74194" || type == "74HC194" || type == "74HCT194" || type == "74LS194")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74194::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74194(timer, name, position));
}

std::string		hbs::CMP_74194::GetDisplayType(void) const
{
  return ("74194 4-bit bidirectional universal shift register");
}
