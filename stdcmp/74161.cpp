// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "74161.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_74161::GetType(void) const
{
  return (type = canonical);
}

unsigned char           hbs::CMP_74161::Modulo(void) const
{
  if (kind == BCD_ASYNC_CLEAR || kind == BCD_SYNC_CLEAR)
    return (10);
  return (16);
}

bool                    hbs::CMP_74161::IsSynchronousClear(void) const
{
  return (kind == BCD_SYNC_CLEAR || kind == BINARY_SYNC_CLEAR);
}

unsigned char           hbs::CMP_74161::ReadPreset(void)
{
  unsigned char         out = 0;

  if (GetPin(3) == hbs::TRUE)
    out |= 1 << 0;
  if (GetPin(4) == hbs::TRUE)
    out |= 1 << 1;
  if (GetPin(5) == hbs::TRUE)
    out |= 1 << 2;
  if (GetPin(6) == hbs::TRUE)
    out |= 1 << 3;
  return (out % Modulo());
}

void                    hbs::CMP_74161::UpdateCounter(void)
{
  if (last_tick == timer.GetTime())
    return ;
  if (!IsSynchronousClear() && GetPin(1) == hbs::FALSE)
    {
      value = 0;
      last_tick = timer.GetTime();
      return ;
    }
  if (!(GetPrevious(2) == hbs::FALSE && GetPin(2) == hbs::TRUE))
    return ;
  if (IsSynchronousClear() && GetPin(1) == hbs::FALSE)
    value = 0;
  else if (GetPin(9) == hbs::FALSE)
    value = ReadPreset();
  else if (GetPin(7) == hbs::TRUE && GetPin(10) == hbs::TRUE)
    value = (value + 1) % Modulo();
  last_tick = timer.GetTime();
}

hbs::Tristate           hbs::CMP_74161::OutputBit(size_t n) const
{
  unsigned int          bit;

  if (n == 14)
    bit = 0;
  else if (n == 13)
    bit = 1;
  else if (n == 12)
    bit = 2;
  else if (n == 11)
    bit = 3;
  else
    return (hbs::UNDEFINED);
  return (((value >> bit) & 1) ? hbs::TRUE : hbs::FALSE);
}

hbs::Tristate           hbs::CMP_74161::CarryOut(void)
{
  if (GetPin(10) != hbs::TRUE)
    {
      if (!hbs::LogicKnown(GetPin(10)))
        return (hbs::UNDEFINED);
      return (hbs::FALSE);
    }
  return (value == Modulo() - 1 ? hbs::TRUE : hbs::FALSE);
}

bool                    hbs::CMP_74161::DrivesPin(size_t n) const
{
  return (n == 11 || n == 12 || n == 13 || n == 14 || n == 15);
}

hbs::Tristate           hbs::CMP_74161::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      UpdateCounter();
      if (n == 15)
        return (p->second = CarryOut());
      return (p->second = OutputBit(n));
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74161::CMP_74161(const hbs::Timer  &time,
                          const std::string &name,
                          const std::string &position,
                          const std::string &canonical,
                          const std::string &display,
                          CounterKind       kind)
  : AComponent(time, name, position),
    canonical(canonical),
    display(display),
    kind(kind),
    value(0),
    last_tick(0)
{}

hbs::CMP_74161::~CMP_74161(void)
{}

bool                    hbs::CMP_74161::TypeMatches(const std::string &type)
{
  if (type == "74160" || type == "74HC160" || type == "74HCT160" || type == "74LS160")
    return (true);
  if (type == "74161" || type == "74HC161" || type == "74HCT161" || type == "74LS161")
    return (true);
  if (type == "74162" || type == "74HC162" || type == "74HCT162" || type == "74LS162")
    return (true);
  if (type == "74163" || type == "74HC163" || type == "74HCT163" || type == "74LS163")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_74161::Create(hbs::Timer        &timer,
                                                const std::string &type,
                                                const std::string &name,
                                                const std::string &value,
                                                const std::string &position)
{
  (void)value;
  if (type == "74160" || type == "74HC160" || type == "74HCT160" || type == "74LS160")
    return (new hbs::CMP_74161(timer, name, position, "74160", "74160 Synchronous BCD counter, asynchronous clear", BCD_ASYNC_CLEAR));
  if (type == "74161" || type == "74HC161" || type == "74HCT161" || type == "74LS161")
    return (new hbs::CMP_74161(timer, name, position, "74161", "74161 Synchronous binary counter, asynchronous clear", BINARY_ASYNC_CLEAR));
  if (type == "74162" || type == "74HC162" || type == "74HCT162" || type == "74LS162")
    return (new hbs::CMP_74161(timer, name, position, "74162", "74162 Synchronous BCD counter, synchronous clear", BCD_SYNC_CLEAR));
  if (type == "74163" || type == "74HC163" || type == "74HCT163" || type == "74LS163")
    return (new hbs::CMP_74161(timer, name, position, "74163", "74163 Synchronous binary counter, synchronous clear", BINARY_SYNC_CLEAR));
  return (NULL);
}

std::string             hbs::CMP_74161::GetDisplayType(void) const
{
  return (display);
}
