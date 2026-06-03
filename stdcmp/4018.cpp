// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4018.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_4018::GetType(void) const
{
  return (type = "4018");
}

size_t                  hbs::CMP_4018::OutputIndex(size_t n) const
{
  if (n == 5) return (0);
  if (n == 4) return (1);
  if (n == 6) return (2);
  if (n == 11) return (3);
  if (n == 13) return (4);
  return (5);
}

hbs::Tristate           hbs::CMP_4018::ReadJam(size_t idx)
{
  static const size_t   pins[] = {2, 3, 7, 9, 12};

  return (GetPin(pins[idx]));
}

void                    hbs::CMP_4018::Update(void)
{
  hbs::Tristate         old[5];

  if (last_tick == timer.GetTime())
    return ;
  last_tick = timer.GetTime();
  if (GetPin(15) == hbs::TRUE)
    {
      for (size_t i = 0; i < 5; ++i)
        qbar[i] = hbs::TRUE;
      return ;
    }
  if (GetPin(10) == hbs::TRUE)
    {
      for (size_t i = 0; i < 5; ++i)
        qbar[i] = hbs::LogicNot(ReadJam(i));
      return ;
    }
  if (!(GetPrevious(14) == hbs::FALSE && GetPin(14) == hbs::TRUE))
    return ;
  for (size_t i = 0; i < 5; ++i)
    old[i] = qbar[i];
  qbar[0] = hbs::LogicNot(GetPin(1));
  for (size_t i = 1; i < 5; ++i)
    qbar[i] = old[i - 1];
}

bool                    hbs::CMP_4018::DrivesPin(size_t n) const
{
  return (n == 4 || n == 5 || n == 6 || n == 11 || n == 13);
}

hbs::Tristate           hbs::CMP_4018::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;
  size_t                idx;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  idx = OutputIndex(n);
  if (idx < 5)
    {
      Update();
      return (p->second = qbar[idx]);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4018::CMP_4018(const hbs::Timer  &time,
                        const std::string &name,
                        const std::string &position)
  : AComponent(time, name, position),
    last_tick(0)
{
  for (size_t i = 0; i < 5; ++i)
    qbar[i] = hbs::UNDEFINED;
}

hbs::CMP_4018::~CMP_4018(void)
{}

bool                    hbs::CMP_4018::TypeMatches(const std::string &type)
{
  return (type == "4018" || type == "74HC4018" || type == "74HCT4018");
}

hbs::IComponent         *hbs::CMP_4018::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4018(timer, name, position));
}

std::string             hbs::CMP_4018::GetDisplayType(void) const
{
  return ("4018 Presettable divide-by-N Johnson counter");
}
