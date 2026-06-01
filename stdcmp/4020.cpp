// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4020.hpp"

const std::string	&hbs::CMP_4020::GetType(void) const
{
  return (type = "4020");
}

static int		output_stage(size_t n)
{
  static const int map[16] =
    {
      12, 13, 14, 6, 5, 7, 4, -1,
       1, -1, -1, 9, 8, 10, 11, -1
    };

  if (n == 0 || n > 16)
    return (-1);
  return (map[n - 1]);
}

bool			hbs::CMP_4020::DrivesPin(size_t n) const
{
  return (output_stage(n) > 0);
}

hbs::Tristate		hbs::CMP_4020::Compute(size_t n)
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
      if (GetPin(11) == hbs::TRUE)
	return (p->second = hbs::FALSE);
      if (GetPin(10) == hbs::FALSE && GetPrevious(10) == hbs::TRUE && last_tick != timer.GetTime())
	{
	  last_tick = timer.GetTime();
	  value = (value + 1) & 0x3FFF;
	}
      return (p->second = ((value >> (stage - 1)) & 1) ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4020::CMP_4020(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos),
    value(0),
    last_tick(0)
{}

hbs::CMP_4020::~CMP_4020(void)
{}

bool			hbs::CMP_4020::TypeMatches(const std::string &type)
{
  if (type == "4020")
    return (true);
  if (type == "74HC4020")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4020::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4020(timer, name, position));
}

std::string		hbs::CMP_4020::GetDisplayType(void) const
{
  return ("4020 14-stage binary ripple counter/divider");
}
