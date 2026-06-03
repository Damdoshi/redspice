// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "7474.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_7474::GetType(void) const
{
  return (type = "7474");
}

bool                    hbs::CMP_7474::DrivesPin(size_t n) const
{
  return (n == 5 || n == 6 || n == 8 || n == 9);
}

void                    hbs::CMP_7474::UpdateFlipFlop(size_t id)
{
  static const size_t   clr[2] = {1, 13};
  static const size_t   d[2] = {2, 12};
  static const size_t   clk[2] = {3, 11};
  static const size_t   pre[2] = {4, 10};

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
      value[id] = GetPin(d[id]);
    }
}

hbs::Tristate           hbs::CMP_7474::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 5 || n == 6 || n == 8 || n == 9)
    {
      size_t id = (n == 5 || n == 6) ? 0 : 1;
      UpdateFlipFlop(id);
      if (n == 6 || n == 8)
        return (p->second = hbs::LogicNot(value[id]));
      return (p->second = value[id]);
    }
  if (n == 7 || n == 14)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_7474::CMP_7474(const hbs::Timer        &time,
                        const std::string       &name,
                        const std::string       &pos)
  : AComponent(time, name, pos)
{
  for (size_t i = 0; i < 2; ++i)
    {
      value[i] = hbs::UNDEFINED;
      last_tick[i] = 0;
    }
}

hbs::CMP_7474::~CMP_7474(void)
{}

bool                    hbs::CMP_7474::TypeMatches(const std::string &type)
{
  if (type == "7474")
    return (true);
  if (type == "74HC74")
    return (true);
  if (type == "74HCT74")
    return (true);
  if (type == "74LS74")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_7474::Create(hbs::Timer           &timer,
                                               const std::string    &type,
                                               const std::string    &name,
                                               const std::string    &value,
                                               const std::string    &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_7474(timer, name, position));
}

std::string             hbs::CMP_7474::GetDisplayType(void) const
{
  return ("7474 Dual D flip-flop with preset/clear");
}
