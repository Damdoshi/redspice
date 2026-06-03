// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "7493.hpp"

const std::string       &hbs::CMP_7493::GetType(void) const
{
  return (type = "7493");
}

void                    hbs::CMP_7493::UpdateCounter(void)
{
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(2) == hbs::TRUE && GetPin(3) == hbs::TRUE)
    {
      value = 0;
      last_tick = timer.GetTime();
      return ;
    }
  if (GetPrevious(14) == hbs::TRUE && GetPin(14) == hbs::FALSE)
    value ^= 0x01;
  if (GetPrevious(1) == hbs::TRUE && GetPin(1) == hbs::FALSE)
    value = (value & 0x01) | (((((value >> 1) + 1) & 0x07) << 1));
  last_tick = timer.GetTime();
}

hbs::Tristate           hbs::CMP_7493::Output(size_t n) const
{
  if (n == 12)
    return ((value & 0x01) ? hbs::TRUE : hbs::FALSE);
  if (n == 9)
    return ((value & 0x02) ? hbs::TRUE : hbs::FALSE);
  if (n == 8)
    return ((value & 0x04) ? hbs::TRUE : hbs::FALSE);
  if (n == 11)
    return ((value & 0x08) ? hbs::TRUE : hbs::FALSE);
  return (hbs::UNDEFINED);
}

bool                    hbs::CMP_7493::DrivesPin(size_t n) const
{
  return (n == 8 || n == 9 || n == 11 || n == 12);
}

hbs::Tristate           hbs::CMP_7493::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      UpdateCounter();
      return (p->second = Output(n));
    }
  if (n == 5 || n == 10)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_7493::CMP_7493(const hbs::Timer   &time,
                        const std::string  &name,
                        const std::string  &position)
  : AComponent(time, name, position),
    value(0),
    last_tick(0)
{}

hbs::CMP_7493::~CMP_7493(void)
{}

bool                    hbs::CMP_7493::TypeMatches(const std::string &type)
{
  if (type == "7493" || type == "74HC93" || type == "74HCT93" || type == "74LS93")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_7493::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_7493(timer, name, position));
}

std::string             hbs::CMP_7493::GetDisplayType(void) const
{
  return ("7493 4-bit binary ripple counter");
}
