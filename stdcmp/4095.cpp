// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4095.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_4095::GetType(void) const
{
  return (type = canonical);
}

void			hbs::CMP_4095::Update(size_t idx, size_t clk, size_t reset, size_t k, size_t j, size_t set)
{
  hbs::Tristate		jn;
  hbs::Tristate		kn;

  if (GetPin(set) == hbs::TRUE && GetPin(reset) == hbs::TRUE)
    {
      val[idx] = hbs::BROKEN;
      return ;
    }
  if (GetPin(set) == hbs::TRUE)
    {
      val[idx] = hbs::TRUE;
      return ;
    }
  if (GetPin(reset) == hbs::TRUE)
    {
      val[idx] = hbs::FALSE;
      return ;
    }
  if (last_tick[idx] == timer.GetTime() || GetPin(clk) != hbs::TRUE || GetPrevious(clk) != hbs::FALSE)
    return ;
  last_tick[idx] = timer.GetTime();
  jn = GetPin(j);
  kn = GetPin(k);
  if (!hbs::LogicKnown(jn) || !hbs::LogicKnown(kn) || !hbs::LogicKnown(val[idx]))
    {
      val[idx] = hbs::UNDEFINED;
      return ;
    }
  if (jn == hbs::FALSE && kn == hbs::FALSE)
    return ;
  if (jn == hbs::TRUE && kn == hbs::FALSE)
    val[idx] = hbs::TRUE;
  else if (jn == hbs::FALSE && kn == hbs::TRUE)
    val[idx] = hbs::FALSE;
  else
    val[idx] = hbs::LogicNot(val[idx]);
}

hbs::Tristate		hbs::CMP_4095::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  size_t		idx;
  size_t		qpin;
  size_t		clk;
  size_t		reset;
  size_t		k;
  size_t		j;
  size_t		set;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 1 || n == 2)
    {
      idx = 0; qpin = 1; clk = 3; reset = 4; k = 5; j = 6; set = 7;
    }
  else if (n == 14 || n == 15)
    {
      idx = 1; qpin = 15; clk = 13; reset = 12; k = 11; j = 10; set = 9;
    }
  else if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  else
    return (GetPin(n));
  Update(idx, clk, reset, k, j, set);
  if (n == qpin)
    return (p->second = val[idx]);
  return (p->second = hbs::LogicNot(val[idx]));
}

hbs::CMP_4095::CMP_4095(const hbs::Timer	&time,
				const std::string	&name,
				const std::string	&pos,
				const std::string	&canonical,
				const std::string	&display)
  : AComponent(time, name, pos),
    canonical(canonical),
    display(display)
{
  val[0] = hbs::UNDEFINED;
  val[1] = hbs::UNDEFINED;
  last_tick[0] = 0;
  last_tick[1] = 0;
}

hbs::CMP_4095::~CMP_4095(void)
{}

bool			hbs::CMP_4095::TypeMatches(const std::string &type)
{
  if (type == "4095" || type == "74HC4095" || type == "74HCT4095")
    return (true);
  if (type == "4096" || type == "74HC4096" || type == "74HCT4096")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4095::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (type == "4095" || type == "74HC4095" || type == "74HCT4095")
    return (new hbs::CMP_4095(timer, name, position, "4095", "4095 Gated JK flip-flop"));
  if (type == "4096" || type == "74HC4096" || type == "74HCT4096")
    return (new hbs::CMP_4095(timer, name, position, "4096", "4096 Gated JK flip-flop"));
  return (NULL);
}

std::string		hbs::CMP_4095::GetDisplayType(void) const
{
  return (display);
}
