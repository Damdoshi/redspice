// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "74175.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_74175::GetType(void) const
{
  return (type = "74175");
}

bool                    hbs::CMP_74175::DrivesPin(size_t n) const
{
  return (n == 2 || n == 3 || n == 6 || n == 7 || n == 10 || n == 11 || n == 14 || n == 15);
}

void                    hbs::CMP_74175::UpdateRegister(void)
{
  static const size_t   dpins[4] = {4, 5, 12, 13};

  if (GetPin(1) == hbs::FALSE)
    {
      for (size_t i = 0; i < 4; ++i)
        value[i] = hbs::FALSE;
      return ;
    }
  if (!hbs::LogicKnown(GetPin(1)))
    {
      for (size_t i = 0; i < 4; ++i)
        value[i] = hbs::UNDEFINED;
      return ;
    }
  if (last_tick != timer.GetTime() && GetPin(9) == hbs::TRUE && GetPrevious(9) == hbs::FALSE)
    {
      last_tick = timer.GetTime();
      for (size_t i = 0; i < 4; ++i)
        value[i] = GetPin(dpins[i]);
    }
}

hbs::Tristate           hbs::CMP_74175::Compute(size_t n)
{
  static const size_t   qpins[4] = {2, 7, 10, 15};
  static const size_t   npins[4] = {3, 6, 11, 14};
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  for (size_t i = 0; i < 4; ++i)
    if (n == qpins[i] || n == npins[i])
      {
        UpdateRegister();
        if (n == npins[i])
          return (p->second = hbs::LogicNot(value[i]));
        return (p->second = value[i]);
      }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74175::CMP_74175(const hbs::Timer       &time,
                          const std::string      &name,
                          const std::string      &pos)
  : AComponent(time, name, pos), last_tick(0)
{
  for (size_t i = 0; i < 4; ++i)
    value[i] = hbs::UNDEFINED;
}

hbs::CMP_74175::~CMP_74175(void)
{}

bool                    hbs::CMP_74175::TypeMatches(const std::string &type)
{
  if (type == "74175")
    return (true);
  if (type == "74HC175")
    return (true);
  if (type == "74HCT175")
    return (true);
  if (type == "74LS175")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_74175::Create(hbs::Timer           &timer,
                                                const std::string    &type,
                                                const std::string    &name,
                                                const std::string    &value,
                                                const std::string    &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74175(timer, name, position));
}

std::string             hbs::CMP_74175::GetDisplayType(void) const
{
  return ("74175 Quad D flip-flop with complementary outputs");
}
