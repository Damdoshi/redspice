// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4046.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_4046::GetType(void) const
{
  return (type = "4046");
}

bool                    hbs::CMP_4046::DrivesPin(size_t n) const
{
  return (n == 1 || n == 2 || n == 4 || n == 10 || n == 13);
}

void                    hbs::CMP_4046::UpdateVCO(void)
{
  if (last_tick == timer.GetTime())
    return ;
  last_tick = timer.GetTime();
  if (GetPin(5) == hbs::TRUE)
    vco = hbs::FALSE;
  else if (vco == hbs::TRUE)
    vco = hbs::FALSE;
  else
    vco = hbs::TRUE;
}

hbs::Tristate           hbs::CMP_4046::PhaseComparator1(void)
{
  hbs::Tristate         sig = GetPin(14);
  hbs::Tristate         cmp = GetPin(3);

  if (sig == hbs::BROKEN || cmp == hbs::BROKEN)
    return (hbs::BROKEN);
  if (!hbs::LogicKnown(sig) || !hbs::LogicKnown(cmp))
    return (hbs::UNDEFINED);
  return (sig != cmp ? hbs::TRUE : hbs::FALSE);
}

hbs::Tristate           hbs::CMP_4046::PhaseComparator2(void)
{
  hbs::Tristate         sig = GetPin(14);
  hbs::Tristate         cmp = GetPin(3);

  if (sig == hbs::BROKEN || cmp == hbs::BROKEN)
    return (hbs::BROKEN);
  if (!hbs::LogicKnown(sig) || !hbs::LogicKnown(cmp))
    return (hbs::UNDEFINED);
  if (sig == hbs::TRUE && cmp == hbs::FALSE)
    return (hbs::TRUE);
  return (hbs::FALSE);
}

hbs::Tristate           hbs::CMP_4046::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 4)
    {
      UpdateVCO();
      return (p->second = vco);
    }
  if (n == 2)
    return (p->second = PhaseComparator1());
  if (n == 13)
    return (p->second = PhaseComparator2());
  if (n == 1)
    return (p->second = (GetPin(14) == hbs::TRUE && GetPin(3) == hbs::FALSE) ? hbs::TRUE : hbs::FALSE);
  if (n == 10)
    return (p->second = GetPin(9));
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4046::CMP_4046(const hbs::Timer       &time,
                        const std::string      &name,
                        const std::string      &pos)
  : AComponent(time, name, pos),
    vco(hbs::FALSE),
    last_tick(0)
{}

hbs::CMP_4046::~CMP_4046(void)
{}

bool                    hbs::CMP_4046::TypeMatches(const std::string &type)
{
  if (type == "4046" || type == "74HC4046" || type == "74HCT4046")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4046::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4046(timer, name, position));
}

std::string             hbs::CMP_4046::GetDisplayType(void) const
{
  return ("4046 Phase-locked loop with VCO, simplified digital model");
}
