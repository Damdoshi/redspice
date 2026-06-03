// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "74193.hpp"

const std::string       &hbs::CMP_74193::GetType(void) const
{
  return (type = canonical);
}

unsigned char           hbs::CMP_74193::Modulo(void) const
{
  return (bcd ? 10 : 16);
}

unsigned char           hbs::CMP_74193::ReadPreset(void)
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
  return (out % Modulo());
}

void                    hbs::CMP_74193::UpdateCounter(void)
{
  bool                  up_edge;
  bool                  down_edge;

  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(14) == hbs::TRUE)
    {
      value = 0;
      last_tick = timer.GetTime();
      return ;
    }
  if (GetPin(11) == hbs::FALSE)
    {
      value = ReadPreset();
      last_tick = timer.GetTime();
      return ;
    }
  up_edge = (GetPrevious(5) == hbs::FALSE && GetPin(5) == hbs::TRUE);
  down_edge = (GetPrevious(4) == hbs::FALSE && GetPin(4) == hbs::TRUE);
  if (up_edge && !down_edge && GetPin(4) == hbs::TRUE)
    value = (value + 1) % Modulo();
  else if (down_edge && !up_edge && GetPin(5) == hbs::TRUE)
    value = (value + Modulo() - 1) % Modulo();
  last_tick = timer.GetTime();
}

hbs::Tristate           hbs::CMP_74193::OutputBit(size_t n) const
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

hbs::Tristate           hbs::CMP_74193::CarryOut(void) const
{
  return (value == Modulo() - 1 ? hbs::FALSE : hbs::TRUE);
}

hbs::Tristate           hbs::CMP_74193::BorrowOut(void) const
{
  return (value == 0 ? hbs::FALSE : hbs::TRUE);
}

bool                    hbs::CMP_74193::DrivesPin(size_t n) const
{
  return (n == 2 || n == 3 || n == 6 || n == 7 || n == 12 || n == 13);
}

hbs::Tristate           hbs::CMP_74193::Compute(size_t n)
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
        return (p->second = CarryOut());
      if (n == 13)
        return (p->second = BorrowOut());
      return (p->second = OutputBit(n));
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74193::CMP_74193(const hbs::Timer  &time,
                          const std::string &name,
                          const std::string &position,
                          const std::string &canonical,
                          const std::string &display,
                          bool              bcd)
  : AComponent(time, name, position),
    bcd(bcd),
    canonical(canonical),
    display(display),
    value(0),
    last_tick(0)
{}

hbs::CMP_74193::~CMP_74193(void)
{}

bool                    hbs::CMP_74193::TypeMatches(const std::string &type)
{
  if (type == "74192" || type == "74HC192" || type == "74HCT192" || type == "74LS192")
    return (true);
  if (type == "74193" || type == "74HC193" || type == "74HCT193" || type == "74LS193")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_74193::Create(hbs::Timer        &timer,
                                                const std::string &type,
                                                const std::string &name,
                                                const std::string &value,
                                                const std::string &position)
{
  (void)value;
  if (type == "74192" || type == "74HC192" || type == "74HCT192" || type == "74LS192")
    return (new hbs::CMP_74193(timer, name, position, "74192", "74192 Presettable BCD up-down counter", true));
  if (type == "74193" || type == "74HC193" || type == "74HCT193" || type == "74LS193")
    return (new hbs::CMP_74193(timer, name, position, "74193", "74193 Presettable binary up-down counter", false));
  return (NULL);
}

std::string             hbs::CMP_74193::GetDisplayType(void) const
{
  return (display);
}
