// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4035.hpp"

const std::string	&hbs::CMP_4035::GetType(void) const
{
  return (type = "4035");
}

static const size_t	parallel_pin[4] = {9, 10, 11, 12};
static const size_t	output_pin[4] = {1, 13, 14, 15};

bool			hbs::CMP_4035::DrivesPin(size_t n) const
{
  return (n == 1 || n == 13 || n == 14 || n == 15);
}

void			hbs::CMP_4035::Tick(void)
{
  if (GetPin(5) == hbs::TRUE)
    {
      value.reset();
      return ;
    }
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(6) != hbs::TRUE || GetPrevious(6) != hbs::FALSE)
    return ;
  last_tick = timer.GetTime();
  if (GetPin(7) == hbs::TRUE)
    {
      for (size_t i = 0; i < 4; ++i)
	if (GetPin(parallel_pin[i]) == hbs::TRUE)
	  value.set(i);
	else
	  value.reset(i);
      return ;
    }
  value <<= 1;
  if (GetPin(3) == hbs::TRUE && GetPin(4) == hbs::FALSE)
    value.set(0);
  else if (GetPin(3) == hbs::FALSE && GetPin(4) == hbs::TRUE)
    value.reset(0);
  else if (GetPin(3) == hbs::TRUE && GetPin(4) == hbs::TRUE)
    value.set(0);
  else
    value.reset(0);
}

hbs::Tristate		hbs::CMP_4035::Compute(size_t n)
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
	  {
	    bool bit = value[i];
	    if (GetPin(2) == hbs::FALSE)
	      bit = !bit;
	    return (p->second = bit ? hbs::TRUE : hbs::FALSE);
	  }
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4035::CMP_4035(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos),
    last_tick(0)
{}

hbs::CMP_4035::~CMP_4035(void)
{}

bool			hbs::CMP_4035::TypeMatches(const std::string &type)
{
  if (type == "4035")
    return (true);
  if (type == "74HC4035")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4035::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4035(timer, name, position));
}

std::string		hbs::CMP_4035::GetDisplayType(void) const
{
  return ("4035 4-stage parallel-in parallel-out shift register");
}
