// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "74109.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_74109::GetType(void) const
{
  return (type = "74109");
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
    state = hbs::LogicNot(state);
}

bool                    hbs::CMP_74109::DrivesPin(size_t n) const
{
  return (n == 6 || n == 7 || n == 9 || n == 10);
}

void                    hbs::CMP_74109::UpdateFlipFlop(size_t id)
{
  static const size_t   clr[2] = {1, 15};
  static const size_t   j[2] = {2, 14};
  static const size_t   kbar[2] = {3, 13};
  static const size_t   clk[2] = {4, 12};
  static const size_t   pre[2] = {5, 11};
  hbs::Tristate         k;

  if (GetPin(pre[id]) == hbs::FALSE && GetPin(clr[id]) == hbs::FALSE)
    {
      value[id] = hbs::UNDEFINED;
      return ;
    }
  if (GetPin(pre[id]) == hbs::FALSE)
    {
      value[id] = hbs::TRUE;
      return ;
    }
  if (GetPin(clr[id]) == hbs::FALSE)
    {
      value[id] = hbs::FALSE;
      return ;
    }
  if (!hbs::LogicKnown(GetPin(pre[id])) || !hbs::LogicKnown(GetPin(clr[id])))
    {
      value[id] = hbs::UNDEFINED;
      return ;
    }
  if (last_tick[id] != timer.GetTime() && GetPin(clk[id]) == hbs::TRUE && GetPrevious(clk[id]) == hbs::FALSE)
    {
      last_tick[id] = timer.GetTime();
      k = hbs::LogicNot(GetPin(kbar[id]));
      update_jk(value[id], GetPin(j[id]), k);
    }
}

hbs::Tristate           hbs::CMP_74109::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 6 || n == 7 || n == 9 || n == 10)
    {
      size_t id = (n == 6 || n == 7) ? 0 : 1;
      UpdateFlipFlop(id);
      if (n == 7 || n == 9)
        return (p->second = hbs::LogicNot(value[id]));
      return (p->second = value[id]);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74109::CMP_74109(const hbs::Timer       &time,
                          const std::string      &name,
                          const std::string      &pos)
  : AComponent(time, name, pos)
{
  value[0] = hbs::UNDEFINED;
  value[1] = hbs::UNDEFINED;
  last_tick[0] = 0;
  last_tick[1] = 0;
}

hbs::CMP_74109::~CMP_74109(void)
{}

bool                    hbs::CMP_74109::TypeMatches(const std::string &type)
{
  if (type == "74109")
    return (true);
  if (type == "74HC109")
    return (true);
  if (type == "74HCT109")
    return (true);
  if (type == "74LS109")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_74109::Create(hbs::Timer          &timer,
                                                const std::string   &type,
                                                const std::string   &name,
                                                const std::string   &value,
                                                const std::string   &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74109(timer, name, position));
}

std::string             hbs::CMP_74109::GetDisplayType(void) const
{
  return ("74109 Dual JK flip-flop with inverted K, preset/clear");
}
