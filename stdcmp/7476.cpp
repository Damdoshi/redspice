// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "7476.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_7476::GetType(void) const
{
  return (type = "7476");
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

bool                    hbs::CMP_7476::DrivesPin(size_t n) const
{
  return (n == 10 || n == 11 || n == 14 || n == 15);
}

void                    hbs::CMP_7476::UpdateFlipFlop(size_t id)
{
  static const size_t   clk[2] = {1, 6};
  static const size_t   pre[2] = {2, 7};
  static const size_t   clr[2] = {3, 8};
  static const size_t   j[2] = {4, 16};
  static const size_t   k[2] = {13, 9};

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
  if (last_tick[id] != timer.GetTime() && GetPin(clk[id]) == hbs::FALSE && GetPrevious(clk[id]) == hbs::TRUE)
    {
      last_tick[id] = timer.GetTime();
      update_jk(value[id], GetPin(j[id]), GetPin(k[id]));
    }
}

hbs::Tristate           hbs::CMP_7476::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 14 || n == 15 || n == 10 || n == 11)
    {
      size_t id = (n == 14 || n == 15) ? 0 : 1;
      UpdateFlipFlop(id);
      if (n == 15 || n == 11)
        return (p->second = hbs::LogicNot(value[id]));
      return (p->second = value[id]);
    }
  if (n == 5 || n == 12)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_7476::CMP_7476(const hbs::Timer        &time,
                        const std::string       &name,
                        const std::string       &pos)
  : AComponent(time, name, pos)
{
  value[0] = hbs::UNDEFINED;
  value[1] = hbs::UNDEFINED;
  last_tick[0] = 0;
  last_tick[1] = 0;
}

hbs::CMP_7476::~CMP_7476(void)
{}

bool                    hbs::CMP_7476::TypeMatches(const std::string &type)
{
  if (type == "7476")
    return (true);
  if (type == "74HC76")
    return (true);
  if (type == "74HCT76")
    return (true);
  if (type == "74LS76")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_7476::Create(hbs::Timer           &timer,
                                               const std::string    &type,
                                               const std::string    &name,
                                               const std::string    &value,
                                               const std::string    &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_7476(timer, name, position));
}

std::string             hbs::CMP_7476::GetDisplayType(void) const
{
  return ("7476 Dual JK flip-flop with preset/clear");
}
