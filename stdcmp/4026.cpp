// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4026.hpp"

const std::string	&hbs::CMP_4026::GetType(void) const
{
  return (type = "4026");
}

static const bool	seven_segments[10][7] =
  {
    {true,  true,  true,  true,  true,  true,  false},
    {false, true,  true,  false, false, false, false},
    {true,  true,  false, true,  true,  false, true },
    {true,  true,  true,  true,  false, false, true },
    {false, true,  true,  false, false, true,  true },
    {true,  false, true,  true,  false, true,  true },
    {true,  false, true,  true,  true,  true,  true },
    {true,  true,  true,  false, false, false, false},
    {true,  true,  true,  true,  true,  true,  true },
    {true,  true,  true,  true,  false, true,  true }
  };

static int		pin_to_segment(size_t n)
{
  if (n == 10)
    return (0);
  if (n == 12)
    return (1);
  if (n == 13 || n == 14)
    return (2);
  if (n == 9)
    return (3);
  if (n == 11)
    return (4);
  if (n == 6)
    return (5);
  if (n == 7)
    return (6);
  return (-1);
}

bool			hbs::CMP_4026::DrivesPin(size_t n) const
{
  return (n == 4 || n == 5 || n == 6 || n == 7 || n == 9 || n == 10 ||
	  n == 11 || n == 12 || n == 13 || n == 14);
}

void			hbs::CMP_4026::UpdateCounter(void)
{
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(15) == hbs::TRUE)
    {
      value = 0;
      last_tick = timer.GetTime();
      return ;
    }
  if (GetPin(2) != hbs::FALSE)
    return ;
  if (GetPin(1) == hbs::TRUE && GetPrevious(1) == hbs::FALSE)
    {
      value = (value + 1) % 10;
      last_tick = timer.GetTime();
    }
}

hbs::Tristate		hbs::CMP_4026::Segment(size_t n)
{
  int			seg = pin_to_segment(n);

  if (seg < 0)
    return (hbs::UNDEFINED);
  if (n != 14 && GetPin(3) == hbs::FALSE)
    return (hbs::FALSE);
  return (seven_segments[value % 10][seg] ? hbs::TRUE : hbs::FALSE);
}

hbs::Tristate		hbs::CMP_4026::CarryOut(void) const
{
  return (value < 5 ? hbs::TRUE : hbs::FALSE);
}

hbs::Tristate		hbs::CMP_4026::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  if (DrivesPin(n))
    {
      UpdateCounter();
      if (n == 4)
	return (p->second = GetPin(3));
      if (n == 5)
	return (p->second = CarryOut());
      return (p->second = Segment(n));
    }
  return (GetPin(n));
}

hbs::CMP_4026::CMP_4026(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos),
    value(0),
    last_tick(0)
{}

hbs::CMP_4026::~CMP_4026(void)
{}

bool			hbs::CMP_4026::TypeMatches(const std::string &type)
{
  if (type == "4026")
    return (true);
  if (type == "74HC4026")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4026::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4026(timer, name, position));
}

std::string		hbs::CMP_4026::GetDisplayType(void) const
{
  return ("4026 Decade counter with 7-segment decoder/driver");
}
