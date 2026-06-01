// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4006.hpp"

const std::string	&hbs::CMP_4006::GetType(void) const
{
  return (type = "4006");
}

bool			hbs::CMP_4006::DrivesPin(size_t n) const
{
  return (n == 2 || (n >= 8 && n <= 13));
}

void			hbs::CMP_4006::Tick(void)
{
  hbs::Tristate		in[4];

  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(3) != hbs::TRUE || GetPrevious(3) != hbs::FALSE)
    return ;
  last_tick = timer.GetTime();
  in[0] = GetPin(1);
  in[1] = GetPin(4);
  in[2] = GetPin(5);
  in[3] = GetPin(6);
  for (size_t i = 0; i < 4; ++i)
    {
      chain[i] <<= 1;
      if (in[i] == hbs::TRUE)
	chain[i].set(0);
      else
	chain[i].reset(0);
    }
}

hbs::Tristate		hbs::CMP_4006::Output(size_t n) const
{
  if (n == 2)
    return (chain[0][3] ? hbs::FALSE : hbs::TRUE);
  if (n == 13)
    return (chain[0][3] ? hbs::TRUE : hbs::FALSE);
  if (n == 11)
    return (chain[1][3] ? hbs::TRUE : hbs::FALSE);
  if (n == 12)
    return (chain[1][4] ? hbs::TRUE : hbs::FALSE);
  if (n == 10)
    return (chain[2][3] ? hbs::TRUE : hbs::FALSE);
  if (n == 8)
    return (chain[3][3] ? hbs::TRUE : hbs::FALSE);
  if (n == 9)
    return (chain[3][4] ? hbs::TRUE : hbs::FALSE);
  return (hbs::UNDEFINED);
}

hbs::Tristate		hbs::CMP_4006::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      Tick();
      return (p->second = Output(n));
    }
  if (n == 7 || n == 14)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4006::CMP_4006(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos),
    last_tick(0)
{}

hbs::CMP_4006::~CMP_4006(void)
{}

bool			hbs::CMP_4006::TypeMatches(const std::string &type)
{
  if (type == "4006")
    return (true);
  if (type == "74HC4006")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4006::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4006(timer, name, position));
}

std::string		hbs::CMP_4006::GetDisplayType(void) const
{
  return ("4006 18-stage static shift register");
}
