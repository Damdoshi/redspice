// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4027.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_4027::GetType(void) const
{
  return (type = "4027");
}

static hbs::Tristate    invert(hbs::Tristate v)
{
  return (hbs::LogicNot(v));
}

static void             update_jk(hbs::Tristate &state,
                                  hbs::Tristate j,
                                  hbs::Tristate k)
{
  if (!hbs::LogicKnown(j) || !hbs::LogicKnown(k) || !hbs::LogicKnown(state))
    {
      state = hbs::UNDEFINED;
      return ;
    }
  if (j == hbs::FALSE && k == hbs::FALSE)
    return ;
  if (j == hbs::TRUE && k == hbs::FALSE)
    state = hbs::TRUE;
  else if (j == hbs::FALSE && k == hbs::TRUE)
    state = hbs::FALSE;
  else
    state = invert(state);
}

hbs::Tristate           hbs::CMP_4027::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;
  size_t                idx;
  size_t                qpin;
  size_t                nqpin;
  size_t                clk;
  size_t                reset;
  size_t                k;
  size_t                j;
  size_t                set;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 1 || n == 2)
    {
      idx = 0; qpin = 1; nqpin = 2; clk = 3; reset = 4; k = 5; j = 6; set = 7;
    }
  else if (n == 14 || n == 15)
    {
      idx = 1; qpin = 15; nqpin = 14; clk = 13; reset = 12; k = 11; j = 10; set = 9;
    }
  else if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  else
    return (GetPin(n));

  if (GetPin(set) == hbs::TRUE && GetPin(reset) == hbs::TRUE)
    val[idx] = hbs::BROKEN;
  else if (GetPin(set) == hbs::TRUE)
    val[idx] = hbs::TRUE;
  else if (GetPin(reset) == hbs::TRUE)
    val[idx] = hbs::FALSE;
  else if (last_tick[idx] != timer.GetTime() && GetPin(clk) == hbs::TRUE && GetPrevious(clk) == hbs::FALSE)
    {
      last_tick[idx] = timer.GetTime();
      update_jk(val[idx], GetPin(j), GetPin(k));
    }

  if (n == qpin)
    return (p->second = val[idx]);
  if (val[idx] == hbs::BROKEN)
    return (p->second = hbs::BROKEN);
  return (p->second = hbs::LogicNot(val[idx]));
}

hbs::CMP_4027::CMP_4027(const hbs::Timer        &time,
                        const std::string       &name,
                        const std::string       &pos)
  : AComponent(time, name, pos)
{
  val[0] = hbs::UNDEFINED;
  val[1] = hbs::UNDEFINED;
  last_tick[0] = 0;
  last_tick[1] = 0;
}

hbs::CMP_4027::~CMP_4027(void)
{}

bool                    hbs::CMP_4027::TypeMatches(const std::string &type)
{
  if (type == "4027")
    return (true);
  if (type == "74HC4027")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4027::Create(hbs::Timer           &timer,
                                               const std::string    &type,
                                               const std::string    &name,
                                               const std::string    &value,
                                               const std::string    &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4027(timer, name, position));
}

std::string             hbs::CMP_4027::GetDisplayType(void) const
{
  return ("4027 Dual JK flip-flop with set/reset");
}
