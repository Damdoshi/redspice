// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "74393.hpp"

const std::string       &hbs::CMP_74393::GetType(void) const
{
  return (type = "74393");
}

static size_t           cp_pin(size_t half)
{
  return (half == 0 ? 1 : 13);
}

static size_t           mr_pin_393(size_t half)
{
  return (half == 0 ? 2 : 12);
}

void                    hbs::CMP_74393::UpdateHalf(size_t half)
{
  if (last_tick[half] == timer.GetTime())
    return ;
  if (GetPin(mr_pin_393(half)) == hbs::TRUE)
    {
      value[half] = 0;
      last_tick[half] = timer.GetTime();
      return ;
    }
  if (GetPrevious(cp_pin(half)) == hbs::TRUE && GetPin(cp_pin(half)) == hbs::FALSE)
    value[half] = (value[half] + 1) & 0x0F;
  last_tick[half] = timer.GetTime();
}

hbs::Tristate           hbs::CMP_74393::Output(size_t n) const
{
  if (n >= 3 && n <= 6)
    return (((value[0] >> (n - 3)) & 1) ? hbs::TRUE : hbs::FALSE);
  if (n >= 8 && n <= 11)
    return (((value[1] >> (11 - n)) & 1) ? hbs::TRUE : hbs::FALSE);
  return (hbs::UNDEFINED);
}

bool                    hbs::CMP_74393::DrivesPin(size_t n) const
{
  return ((n >= 3 && n <= 6) || (n >= 8 && n <= 11));
}

hbs::Tristate           hbs::CMP_74393::Compute(size_t n)
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
  if (n == 7 || n == 14)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74393::CMP_74393(const hbs::Timer  &time,
                          const std::string &name,
                          const std::string &position)
  : AComponent(time, name, position)
{
  value[0] = value[1] = 0;
  last_tick[0] = last_tick[1] = 0;
}

hbs::CMP_74393::~CMP_74393(void)
{}

bool                    hbs::CMP_74393::TypeMatches(const std::string &type)
{
  if (type == "74393" || type == "74HC393" || type == "74HCT393" || type == "74LS393")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_74393::Create(hbs::Timer        &timer,
                                                const std::string &type,
                                                const std::string &name,
                                                const std::string &value,
                                                const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74393(timer, name, position));
}

std::string             hbs::CMP_74393::GetDisplayType(void) const
{
  return ("74393 Dual 4-bit binary ripple counter");
}
