// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "74190.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_74190::GetType(void) const
{
  return (type = canonical);
}

unsigned char           hbs::CMP_74190::ReadPreset(void)
{
  unsigned char         out = 0;

  if (GetPin(15) == hbs::TRUE)
    out |= 1 << 0;
  if (GetPin(1) == hbs::TRUE)
    out |= 1 << 1;
  if (GetPin(10) == hbs::TRUE)
    out |= 1 << 2;
  if (GetPin(9) == hbs::TRUE)
    out |= 1 << 3;
  return (out % modulo);
}

bool                    hbs::CMP_74190::Down(void)
{
  return (GetPin(5) == hbs::TRUE);
}

bool                    hbs::CMP_74190::Terminal(void)
{
  if (GetPin(4) != hbs::FALSE)
    return (false);
  if (Down())
    return (value == 0);
  return (value == modulo - 1);
}

void                    hbs::CMP_74190::UpdateCounter(void)
{
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(11) == hbs::FALSE)
    {
      value = ReadPreset();
      last_tick = timer.GetTime();
      return ;
    }
  if (GetPrevious(14) == hbs::FALSE && GetPin(14) == hbs::TRUE)
    if (GetPin(4) == hbs::FALSE)
      {
        if (Down())
          value = (value == 0 ? modulo - 1 : value - 1);
        else
          value = (value + 1) % modulo;
      }
  last_tick = timer.GetTime();
}

hbs::Tristate           hbs::CMP_74190::OutputBit(size_t n) const
{
  unsigned int          bit;

  if (n == 3)
    bit = 0;
  else if (n == 2)
    bit = 1;
  else if (n == 6)
    bit = 2;
  else if (n == 7)
    bit = 3;
  else
    return (hbs::UNDEFINED);
  return (((value >> bit) & 1) ? hbs::TRUE : hbs::FALSE);
}

hbs::Tristate           hbs::CMP_74190::MaxMin(void)
{
  return (Terminal() ? hbs::TRUE : hbs::FALSE);
}

hbs::Tristate           hbs::CMP_74190::RippleCarry(void)
{
  if (!Terminal())
    return (hbs::TRUE);
  if (GetPin(14) == hbs::FALSE)
    return (hbs::FALSE);
  return (hbs::TRUE);
}

bool                    hbs::CMP_74190::DrivesPin(size_t n) const
{
  return (n == 2 || n == 3 || n == 6 || n == 7 || n == 12 || n == 13);
}

hbs::Tristate           hbs::CMP_74190::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      UpdateCounter();
      if (n == 12)
        return (p->second = MaxMin());
      if (n == 13)
        return (p->second = RippleCarry());
      return (p->second = OutputBit(n));
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74190::CMP_74190(const hbs::Timer  &time,
                          const std::string &name,
                          const std::string &position,
                          const std::string &canonical,
                          const std::string &display,
                          unsigned char     modulo)
  : AComponent(time, name, position),
    canonical(canonical),
    display(display),
    modulo(modulo),
    value(0),
    last_tick(0)
{}

hbs::CMP_74190::~CMP_74190(void)
{}

bool                    hbs::CMP_74190::TypeMatches(const std::string &type)
{
  if (type == "74190" || type == "74HC190" || type == "74HCT190" || type == "74LS190")
    return (true);
  if (type == "74191" || type == "74HC191" || type == "74HCT191" || type == "74LS191")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_74190::Create(hbs::Timer        &timer,
                                                const std::string &type,
                                                const std::string &name,
                                                const std::string &value,
                                                const std::string &position)
{
  (void)value;
  if (type == "74190" || type == "74HC190" || type == "74HCT190" || type == "74LS190")
    return (new hbs::CMP_74190(timer, name, position, "74190", "74190 Presettable BCD up-down counter", 10));
  if (type == "74191" || type == "74HC191" || type == "74HCT191" || type == "74LS191")
    return (new hbs::CMP_74190(timer, name, position, "74191", "74191 Presettable binary up-down counter", 16));
  return (NULL);
}

std::string             hbs::CMP_74190::GetDisplayType(void) const
{
  return (display);
}
