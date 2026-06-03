// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "74174.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_74174::GetType(void) const
{
  return (type = "74174");
}

bool                    hbs::CMP_74174::DrivesPin(size_t n) const
{
  return (n == 2 || n == 5 || n == 7 || n == 10 || n == 12 || n == 15);
}

void                    hbs::CMP_74174::UpdateRegister(void)
{
  static const size_t   dpins[6] = {3, 4, 6, 11, 13, 14};

  if (GetPin(1) == hbs::FALSE)
    {
      for (size_t i = 0; i < 6; ++i)
        value[i] = hbs::FALSE;
      return ;
    }
  if (!hbs::LogicKnown(GetPin(1)))
    {
      for (size_t i = 0; i < 6; ++i)
        value[i] = hbs::UNDEFINED;
      return ;
    }
  if (last_tick != timer.GetTime() && GetPin(9) == hbs::TRUE && GetPrevious(9) == hbs::FALSE)
    {
      last_tick = timer.GetTime();
      for (size_t i = 0; i < 6; ++i)
        value[i] = GetPin(dpins[i]);
    }
}

hbs::Tristate           hbs::CMP_74174::Compute(size_t n)
{
  static const size_t   qpins[6] = {2, 5, 7, 10, 12, 15};
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  for (size_t i = 0; i < 6; ++i)
    if (n == qpins[i])
      {
        UpdateRegister();
        return (p->second = value[i]);
      }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74174::CMP_74174(const hbs::Timer       &time,
                          const std::string      &name,
                          const std::string      &pos)
  : AComponent(time, name, pos), last_tick(0)
{
  for (size_t i = 0; i < 6; ++i)
    value[i] = hbs::UNDEFINED;
}

hbs::CMP_74174::~CMP_74174(void)
{}

bool                    hbs::CMP_74174::TypeMatches(const std::string &type)
{
  if (type == "74174")
    return (true);
  if (type == "74HC174")
    return (true);
  if (type == "74HCT174")
    return (true);
  if (type == "74LS174")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_74174::Create(hbs::Timer           &timer,
                                                const std::string    &type,
                                                const std::string    &name,
                                                const std::string    &value,
                                                const std::string    &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74174(timer, name, position));
}

std::string             hbs::CMP_74174::GetDisplayType(void) const
{
  return ("74174 Hex D flip-flop with clear");
}
