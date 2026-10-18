// Jason Brillante "Damdoshi" / RED Spice
#include "6502.hpp"
#include <limits>

static int address_bit(size_t pin)
{
  if (pin >= 9 && pin <= 20) return int(pin - 9);
  if (pin >= 22 && pin <= 25) return int(pin - 10);
  return -1;
}

hbs::CMP_6502::CMP_6502(const Timer &time, const std::string &name,
                       const std::string &position)
  : AComponent(time, name, position), last_tick(std::numeric_limits<size_t>::max())
{}

const std::string &hbs::CMP_6502::GetType() const
{ return type = "W65C02S"; }

bool hbs::CMP_6502::DrivesPin(size_t pin) const
{
  return address_bit(pin) >= 0 || (pin >= 26 && pin <= 33) ||
    pin == 1 || pin == 2 || pin == 3 || pin == 5 || pin == 7 || pin == 34 || pin == 39;
}

hbs::Tristate hbs::CMP_6502::PinValue(size_t pin) const
{
  const auto &bus = core.GetBus();
  if (address_bit(pin) >= 0)
    return !bus_enabled ? HIGH_IMPEDANCE : (bus.address & (1u << address_bit(pin))) ? TRUE : FALSE;
  if (pin >= 26 && pin <= 33)
    return !bus_enabled || bus.read ? HIGH_IMPEDANCE : (bus.data & (1u << (33 - pin))) ? TRUE : FALSE;
  switch (pin)
    {
    case 1: return bus.vector ? FALSE : TRUE; // VPB
    case 2: return core.IsWaiting() || core.IsStopped() ? FALSE : HIGH_IMPEDANCE; // open-drain RDY
    case 3: return clock == TRUE ? FALSE : clock == FALSE ? TRUE : UNDEFINED;
    case 5: return bus.locked ? FALSE : TRUE; // MLB
    case 7: return bus.sync ? TRUE : FALSE;
    case 34: return !bus_enabled ? HIGH_IMPEDANCE : bus.read ? TRUE : FALSE;
    case 39: return clock;
    default: return UNDEFINED;
    }
}

void hbs::CMP_6502::Publish()
{
  for (size_t pin = 1; pin <= 40; ++pin)
    if (DrivesPin(pin)) timeline[timer.GetTime()][pin] = PinValue(pin);
}

void hbs::CMP_6502::Tick()
{
  if (last_tick == timer.GetTime()) return;
  last_tick = timer.GetTime(); // Also guards feedback through clock/control nets.
  CleanOld();
  clock = GetPin(37);
  auto reset = GetPin(40);
  bus_enabled = GetPin(36) == TRUE;
  core.SetInterrupts(GetPin(4) == FALSE, GetPin(6) == FALSE);
  auto so = GetPin(38);
  bool rising = clock == TRUE && previous_clock == FALSE;
  bool falling = clock == FALSE && previous_clock == TRUE;
  if (previous_so == UNDEFINED) previous_so = so;
  if (reset == FALSE)
    {
      if (!reset_asserted) core.Reset();
      reset_asserted = true;
    }
  else if (reset == TRUE)
    {
      reset_asserted = false;
      // SOB is sampled at PHI2's rising edge. A pulse must still be present
      // at a rising edge to be seen by the physical processor.
      if (rising)
        {
          if (previous_so == TRUE && so == FALSE) core.SetOverflow();
          previous_so = so;
        }
      if (falling) core.Clock(sampled_data, GetPin(2) == TRUE);
    }
  previous_clock = clock;
  // RAM/ROM address and enable logic may query any CPU pin, in any order.
  Publish();
  if (clock == TRUE && bus_enabled)
    {
      // Sample stable data during PHI2 high, before PHI2-qualified /OE falls.
      // Evaluating all data nets also commits SRAM writes while /WE is active.
      for (unsigned bit = 0; bit < 8; ++bit)
        {
          auto data = GetPin(33 - bit);
          if (core.GetBus().read && (data == TRUE || data == FALSE))
            sampled_data = uint8_t((sampled_data & ~(1u << bit)) | (data == TRUE ? 1u << bit : 0));
        }
      Publish(); // GetPin resolves nets; Compute returns the CPU driver alone.
    }
}

hbs::Tristate hbs::CMP_6502::Compute(size_t pin)
{
  if (pin < 1 || pin > 40) throw BadPin("W65C02S: Bad pin.");
  Tick();
  if (DrivesPin(pin)) return PinValue(pin);
  if (pin == 8 || pin == 21 || pin == 35) return UNDEFINED; // supplies, NC
  Tristate tri;
  if (AlreadyComputed(pin, tri)) return tri;
  return GetPin(pin);
}

bool hbs::CMP_6502::TypeMatches(const std::string &type)
{
  return type == "W65C02S" || type == "W65C02" || type == "WDC65C02" ||
    type == "65C02" || type == "6502" || type == "MOS6502" || type == "NMOS6502";
}

hbs::IComponent *hbs::CMP_6502::Create(Timer &timer, const std::string &type,
                                     const std::string &name, const std::string &value,
                                     const std::string &position)
{
  (void)value;
  return TypeMatches(type) ? new CMP_6502(timer, name, position) : nullptr;
}

std::string hbs::CMP_6502::GetDisplayType() const
{ return "WDC W65C02S CMOS CPU, cycle-by-cycle PDIP-40 bus"; }

std::string hbs::CMP_6502::GetDefinitionValue() const
{ return ""; }
