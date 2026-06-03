// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74597.hpp"

const std::string	&hbs::CMP_74597::GetType(void) const
{
  return (type = "74597");
}

static const size_t	parallel_pin[8] = {15, 1, 2, 3, 4, 5, 6, 7};

bool			hbs::CMP_74597::DrivesPin(size_t n) const
{
  return (n == 9);
}

void			hbs::CMP_74597::LoadInputs(void)
{
  for (size_t i = 0; i < 8; ++i)
    if (GetPin(parallel_pin[i]) == hbs::TRUE)
      input.set(i);
    else
      input.reset(i);
}

void			hbs::CMP_74597::Tick(void)
{
  if (GetPin(10) == hbs::FALSE)
    {
      shift.reset();
      return ;
    }
  if (last_storage_tick != timer.GetTime() && GetPin(12) == hbs::TRUE && GetPrevious(12) == hbs::FALSE)
    {
      last_storage_tick = timer.GetTime();
      LoadInputs();
    }
  if (GetPin(13) == hbs::FALSE)
    {
      shift = input;
      return ;
    }
  if (last_shift_tick != timer.GetTime() && GetPin(11) == hbs::TRUE && GetPrevious(11) == hbs::FALSE)
    {
      last_shift_tick = timer.GetTime();
      shift <<= 1;
      if (GetPin(14) == hbs::TRUE)
	shift.set(0);
      else
	shift.reset(0);
    }
}

hbs::Tristate		hbs::CMP_74597::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 9)
    {
      Tick();
      return (p->second = shift[7] ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74597::CMP_74597(const hbs::Timer	&time,
			  const std::string	&name,
			  const std::string	&pos)
  : AComponent(time, name, pos),
    last_storage_tick(0),
    last_shift_tick(0)
{}

hbs::CMP_74597::~CMP_74597(void)
{}

bool			hbs::CMP_74597::TypeMatches(const std::string &type)
{
  if (type == "74597" || type == "74HC597" || type == "74HCT597" || type == "74LS597")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74597::Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74597(timer, name, position));
}

std::string		hbs::CMP_74597::GetDisplayType(void) const
{
  return ("74597 8-bit shift register with input flip-flops");
}
