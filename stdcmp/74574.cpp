// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "74574.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_74574::GetType(void) const
{
  return (type = "74574");
}

bool                    hbs::CMP_74574::DrivesPin(size_t n) const
{
  return (n >= 12 && n <= 19);
}

void                    hbs::CMP_74574::UpdateRegister(void)
{
  static const size_t   dpins[8] = {2, 3, 4, 5, 6, 7, 8, 9};

  if (last_tick != timer.GetTime() && GetPin(11) == hbs::TRUE && GetPrevious(11) == hbs::FALSE)
    {
      last_tick = timer.GetTime();
      for (size_t i = 0; i < 8; ++i)
        value[i] = GetPin(dpins[i]);
    }
}

hbs::Tristate           hbs::CMP_74574::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n >= 12 && n <= 19)
    {
      UpdateRegister();
      if (GetPin(1) == hbs::TRUE)
        return (p->second = hbs::HIGH_IMPEDANCE);
      if (!hbs::LogicKnown(GetPin(1)))
        return (p->second = hbs::UNDEFINED);
      return (p->second = value[19 - n]);
    }
  if (n == 10 || n == 20)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74574::CMP_74574(const hbs::Timer       &time,
                          const std::string      &name,
                          const std::string      &pos)
  : AComponent(time, name, pos), last_tick(0)
{
  for (size_t i = 0; i < 8; ++i)
    value[i] = hbs::UNDEFINED;
}

hbs::CMP_74574::~CMP_74574(void)
{}

bool                    hbs::CMP_74574::TypeMatches(const std::string &type)
{
  if (type == "74574")
    return (true);
  if (type == "74HC574")
    return (true);
  if (type == "74HCT574")
    return (true);
  if (type == "74LS574")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_74574::Create(hbs::Timer           &timer,
                                                const std::string    &type,
                                                const std::string    &name,
                                                const std::string    &value,
                                                const std::string    &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74574(timer, name, position));
}

std::string             hbs::CMP_74574::GetDisplayType(void) const
{
  return ("74574 Octal D flip-flop with 3-state outputs");
}
