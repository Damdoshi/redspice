// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "74573.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_74573::GetType(void) const
{
  return (type = "74573");
}

bool                    hbs::CMP_74573::DrivesPin(size_t n) const
{
  return (n >= 12 && n <= 19);
}

void                    hbs::CMP_74573::UpdateRegister(void)
{
  static const size_t   dpins[8] = {2, 3, 4, 5, 6, 7, 8, 9};

  if (GetPin(11) == hbs::TRUE)
    for (size_t i = 0; i < 8; ++i)
      value[i] = GetPin(dpins[i]);
  else if (!hbs::LogicKnown(GetPin(11)))
    for (size_t i = 0; i < 8; ++i)
      value[i] = hbs::UNDEFINED;
}

hbs::Tristate           hbs::CMP_74573::Compute(size_t n)
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

hbs::CMP_74573::CMP_74573(const hbs::Timer       &time,
                          const std::string      &name,
                          const std::string      &pos)
  : AComponent(time, name, pos), last_tick(0)
{
  for (size_t i = 0; i < 8; ++i)
    value[i] = hbs::UNDEFINED;
}

hbs::CMP_74573::~CMP_74573(void)
{}

bool                    hbs::CMP_74573::TypeMatches(const std::string &type)
{
  if (type == "74573")
    return (true);
  if (type == "74HC573")
    return (true);
  if (type == "74HCT573")
    return (true);
  if (type == "74LS573")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_74573::Create(hbs::Timer           &timer,
                                                const std::string    &type,
                                                const std::string    &name,
                                                const std::string    &value,
                                                const std::string    &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74573(timer, name, position));
}

std::string             hbs::CMP_74573::GetDisplayType(void) const
{
  return ("74573 Octal transparent latch with 3-state outputs");
}
