// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4024.hpp"

const std::string	&hbs::CMP_4024::GetType(void) const
{
  return (type = "4024");
}

static int		output_stage(size_t n)
{
  static const int map[14] =
    {
      -1, -1, 7, 6, 5, 4, -1,
      -1, 3, -1, 2, 1, -1, -1
    };

  if (n == 0 || n > 14)
    return (-1);
  return (map[n - 1]);
}

bool			hbs::CMP_4024::DrivesPin(size_t n) const
{
  return (output_stage(n) > 0);
}

hbs::Tristate		hbs::CMP_4024::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  int			stage;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  stage = output_stage(n);
  if (stage > 0)
    {
      if (GetPin(2) == hbs::TRUE)
	return (p->second = hbs::FALSE);
      if (GetPin(1) == hbs::FALSE && GetPrevious(1) == hbs::TRUE && last_tick != timer.GetTime())
	{
	  last_tick = timer.GetTime();
	  value = (value + 1) & 0x7F;
	}
      return (p->second = ((value >> (stage - 1)) & 1) ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 7 || n == 14)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4024::CMP_4024(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos),
    value(0),
    last_tick(0)
{}

hbs::CMP_4024::~CMP_4024(void)
{}

bool			hbs::CMP_4024::TypeMatches(const std::string &type)
{
  if (type == "4024")
    return (true);
  if (type == "74HC4024")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4024::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4024(timer, name, position));
}

std::string		hbs::CMP_4024::GetDisplayType(void) const
{
  return ("4024 7-stage binary ripple counter/divider");
}
