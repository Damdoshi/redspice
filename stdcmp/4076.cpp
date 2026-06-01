// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4076.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_4076::GetType(void) const
{
  return (type = "4076");
}

void                    hbs::CMP_4076::UpdateRegister(void)
{
  static const size_t   dpins[4] = {14, 13, 12, 11};

  if (GetPin(15) == hbs::TRUE)
    {
      for (size_t i = 0; i < 4; ++i)
        value[i] = hbs::FALSE;
      return ;
    }
  if (last_tick != timer.GetTime() && GetPin(7) == hbs::TRUE && GetPrevious(7) == hbs::FALSE)
    {
      last_tick = timer.GetTime();
      if (GetPin(9) == hbs::FALSE && GetPin(10) == hbs::FALSE)
        for (size_t i = 0; i < 4; ++i)
          value[i] = GetPin(dpins[i]);
      else if (!hbs::LogicKnown(GetPin(9)) || !hbs::LogicKnown(GetPin(10)))
        for (size_t i = 0; i < 4; ++i)
          value[i] = hbs::UNDEFINED;
    }
}

hbs::Tristate           hbs::CMP_4076::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n >= 3 && n <= 6)
    {
      UpdateRegister();
      if (GetPin(1) == hbs::TRUE || GetPin(2) == hbs::TRUE)
        return (p->second = hbs::HIGH_IMPEDANCE);
      if (!hbs::LogicKnown(GetPin(1)) || !hbs::LogicKnown(GetPin(2)))
        return (p->second = hbs::UNDEFINED);
      return (p->second = value[n - 3]);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4076::CMP_4076(const hbs::Timer        &time,
                        const std::string       &name,
                        const std::string       &pos)
  : AComponent(time, name, pos),
    last_tick(0)
{
  for (size_t i = 0; i < 4; ++i)
    value[i] = hbs::UNDEFINED;
}

hbs::CMP_4076::~CMP_4076(void)
{}

bool                    hbs::CMP_4076::TypeMatches(const std::string &type)
{
  if (type == "4076")
    return (true);
  if (type == "74173")
    return (true);
  if (type == "74HC173")
    return (true);
  if (type == "74LS173")
    return (true);
  if (type == "74HC4076")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4076::Create(hbs::Timer           &timer,
                                               const std::string    &type,
                                               const std::string    &name,
                                               const std::string    &value,
                                               const std::string    &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4076(timer, name, position));
}

std::string             hbs::CMP_4076::GetDisplayType(void) const
{
  return ("4076/74173 Quad D register with 3-state outputs");
}
