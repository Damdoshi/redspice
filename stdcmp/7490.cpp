// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "7490.hpp"

const std::string       &hbs::CMP_7490::GetType(void) const
{
  return (type = "7490");
}

void                    hbs::CMP_7490::UpdateCounter(void)
{
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(2) == hbs::TRUE && GetPin(3) == hbs::TRUE)
    {
      q0 = 0;
      div5 = 0;
      last_tick = timer.GetTime();
      return ;
    }
  if (GetPin(6) == hbs::TRUE && GetPin(7) == hbs::TRUE)
    {
      q0 = 1;
      div5 = 4;
      last_tick = timer.GetTime();
      return ;
    }
  if (GetPrevious(14) == hbs::TRUE && GetPin(14) == hbs::FALSE)
    q0 = !q0;
  if (GetPrevious(1) == hbs::TRUE && GetPin(1) == hbs::FALSE)
    div5 = (div5 + 1) % 5;
  last_tick = timer.GetTime();
}

hbs::Tristate           hbs::CMP_7490::Output(size_t n) const
{
  if (n == 12)
    return (q0 ? hbs::TRUE : hbs::FALSE);
  if (n == 9)
    return ((div5 & 1) ? hbs::TRUE : hbs::FALSE);
  if (n == 8)
    return ((div5 & 2) ? hbs::TRUE : hbs::FALSE);
  if (n == 11)
    return ((div5 & 4) ? hbs::TRUE : hbs::FALSE);
  return (hbs::UNDEFINED);
}

bool                    hbs::CMP_7490::DrivesPin(size_t n) const
{
  return (n == 8 || n == 9 || n == 11 || n == 12);
}

hbs::Tristate           hbs::CMP_7490::Compute(size_t n)
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

hbs::CMP_7490::CMP_7490(const hbs::Timer   &time,
                        const std::string  &name,
                        const std::string  &position)
  : AComponent(time, name, position),
    q0(0),
    div5(0),
    last_tick(0)
{}

hbs::CMP_7490::~CMP_7490(void)
{}

bool                    hbs::CMP_7490::TypeMatches(const std::string &type)
{
  if (type == "7490" || type == "74HC90" || type == "74HCT90" || type == "74LS90")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_7490::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_7490(timer, name, position));
}

std::string             hbs::CMP_7490::GetDisplayType(void) const
{
  return ("7490 Decade ripple counter");
}
