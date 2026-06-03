// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "74374.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_74374::GetType(void) const
{
  return (type = "74374");
}

bool                    hbs::CMP_74374::DrivesPin(size_t n) const
{
  return (n == 2 || n == 5 || n == 6 || n == 9 || n == 12 || n == 15 || n == 16 || n == 19);
}

void                    hbs::CMP_74374::UpdateRegister(void)
{
  static const size_t   dpins[8] = {3, 4, 7, 8, 13, 14, 17, 18};

  if (last_tick != timer.GetTime() && GetPin(11) == hbs::TRUE && GetPrevious(11) == hbs::FALSE)
    {
      last_tick = timer.GetTime();
      for (size_t i = 0; i < 8; ++i)
        value[i] = GetPin(dpins[i]);
    }
}

hbs::Tristate           hbs::CMP_74374::Compute(size_t n)
{
  static const size_t   qpins[8] = {2, 5, 6, 9, 12, 15, 16, 19};
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  for (size_t i = 0; i < 8; ++i)
    if (n == qpins[i])
      {
        UpdateRegister();
        if (GetPin(1) == hbs::TRUE)
          return (p->second = hbs::HIGH_IMPEDANCE);
        if (!hbs::LogicKnown(GetPin(1)))
          return (p->second = hbs::UNDEFINED);
        return (p->second = value[i]);
      }
  if (n == 10 || n == 20)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74374::CMP_74374(const hbs::Timer       &time,
                          const std::string      &name,
                          const std::string      &pos)
  : AComponent(time, name, pos), last_tick(0)
{
  for (size_t i = 0; i < 8; ++i)
    value[i] = hbs::UNDEFINED;
}

hbs::CMP_74374::~CMP_74374(void)
{}

bool                    hbs::CMP_74374::TypeMatches(const std::string &type)
{
  if (type == "74374")
    return (true);
  if (type == "74HC374")
    return (true);
  if (type == "74HCT374")
    return (true);
  if (type == "74LS374")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_74374::Create(hbs::Timer           &timer,
                                                const std::string    &type,
                                                const std::string    &name,
                                                const std::string    &value,
                                                const std::string    &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74374(timer, name, position));
}

std::string             hbs::CMP_74374::GetDisplayType(void) const
{
  return ("74374 Octal D flip-flop with 3-state outputs");
}
