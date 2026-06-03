// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "74390.hpp"

const std::string       &hbs::CMP_74390::GetType(void) const
{
  return (type = "74390");
}

static size_t           cp0_pin(size_t half)
{
  return (half == 0 ? 1 : 15);
}

static size_t           mr_pin(size_t half)
{
  return (half == 0 ? 2 : 14);
}

static size_t           cp1_pin(size_t half)
{
  return (half == 0 ? 4 : 12);
}

void                    hbs::CMP_74390::UpdateHalf(size_t half)
{
  if (last_tick[half] == timer.GetTime())
    return ;
  if (GetPin(mr_pin(half)) == hbs::TRUE)
    {
      q0[half] = 0;
      div5[half] = 0;
      last_tick[half] = timer.GetTime();
      return ;
    }
  if (GetPrevious(cp0_pin(half)) == hbs::TRUE && GetPin(cp0_pin(half)) == hbs::FALSE)
    q0[half] = !q0[half];
  if (GetPrevious(cp1_pin(half)) == hbs::TRUE && GetPin(cp1_pin(half)) == hbs::FALSE)
    div5[half] = (div5[half] + 1) % 5;
  last_tick[half] = timer.GetTime();
}

hbs::Tristate           hbs::CMP_74390::Output(size_t n) const
{
  if (n == 3)
    return (q0[0] ? hbs::TRUE : hbs::FALSE);
  if (n == 5)
    return ((div5[0] & 1) ? hbs::TRUE : hbs::FALSE);
  if (n == 6)
    return ((div5[0] & 2) ? hbs::TRUE : hbs::FALSE);
  if (n == 7)
    return ((div5[0] & 4) ? hbs::TRUE : hbs::FALSE);
  if (n == 13)
    return (q0[1] ? hbs::TRUE : hbs::FALSE);
  if (n == 11)
    return ((div5[1] & 1) ? hbs::TRUE : hbs::FALSE);
  if (n == 10)
    return ((div5[1] & 2) ? hbs::TRUE : hbs::FALSE);
  if (n == 9)
    return ((div5[1] & 4) ? hbs::TRUE : hbs::FALSE);
  return (hbs::UNDEFINED);
}

bool                    hbs::CMP_74390::DrivesPin(size_t n) const
{
  return (n == 3 || n == 5 || n == 6 || n == 7 || n == 9 || n == 10 || n == 11 || n == 13);
}

hbs::Tristate           hbs::CMP_74390::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      UpdateHalf(n < 8 ? 0 : 1);
      return (p->second = Output(n));
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74390::CMP_74390(const hbs::Timer  &time,
                          const std::string &name,
                          const std::string &position)
  : AComponent(time, name, position)
{
  q0[0] = q0[1] = 0;
  div5[0] = div5[1] = 0;
  last_tick[0] = last_tick[1] = 0;
}

hbs::CMP_74390::~CMP_74390(void)
{}

bool                    hbs::CMP_74390::TypeMatches(const std::string &type)
{
  if (type == "74390" || type == "74HC390" || type == "74HCT390" || type == "74LS390")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_74390::Create(hbs::Timer        &timer,
                                                const std::string &type,
                                                const std::string &name,
                                                const std::string &value,
                                                const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74390(timer, name, position));
}

std::string             hbs::CMP_74390::GetDisplayType(void) const
{
  return ("74390 Dual decade ripple counter");
}
