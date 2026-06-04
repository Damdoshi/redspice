// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "6522.hpp"

const std::string       &hbs::CMP_6522::GetType(void) const
{
  return (type = "6522");
}

static const size_t     pa_pin[8] = {2, 3, 4, 5, 6, 7, 8, 9};
static const size_t     pb_pin[8] = {10, 11, 12, 13, 14, 15, 16, 17};
static const size_t     data_pin_6522[8] = {33, 32, 31, 30, 29, 28, 27, 26};
static const size_t     rs_pin[4] = {38, 37, 36, 35};

bool                    hbs::CMP_6522::Selected(void)
{
  return (GetPin(24) == hbs::TRUE && GetPin(23) == hbs::FALSE && GetPin(25) == hbs::TRUE);
}

uint8_t                 hbs::CMP_6522::RegisterAddress(void)
{
  uint8_t               addr = 0;

  for (size_t i = 0; i < 4; ++i)
    if (GetPin(rs_pin[i]) == hbs::TRUE)
      addr |= (uint8_t)(1 << i);
  return (addr & 0x0F);
}

uint8_t                 hbs::CMP_6522::ReadDataBus(void)
{
  uint8_t               out = 0;

  for (size_t i = 0; i < 8; ++i)
    if (GetPin(data_pin_6522[i]) == hbs::TRUE)
      out |= (uint8_t)(1 << i);
  return (out);
}

uint8_t                 hbs::CMP_6522::ReadPortA(void)
{
  uint8_t               out = 0;

  for (size_t i = 0; i < 8; ++i)
    if ((reg[3] & (1 << i)) != 0)
      {
        if (reg[1] & (1 << i))
          out |= (uint8_t)(1 << i);
      }
    else if (GetPin(pa_pin[i]) == hbs::TRUE)
      out |= (uint8_t)(1 << i);
  return (out);
}

uint8_t                 hbs::CMP_6522::ReadPortB(void)
{
  uint8_t               out = 0;

  for (size_t i = 0; i < 8; ++i)
    if ((reg[2] & (1 << i)) != 0)
      {
        if (reg[0] & (1 << i))
          out |= (uint8_t)(1 << i);
      }
    else if (GetPin(pb_pin[i]) == hbs::TRUE)
      out |= (uint8_t)(1 << i);
  return (out);
}

uint8_t                 hbs::CMP_6522::ReadRegister(uint8_t addr)
{
  if (addr == 0 || addr == 15)
    return (ReadPortB());
  if (addr == 1)
    return (ReadPortA());
  return (reg[addr & 0x0F]);
}

void                    hbs::CMP_6522::WriteRegister(uint8_t addr, uint8_t value)
{
  addr &= 0x0F;
  if (addr == 15)
    addr = 1;
  reg[addr] = value;
}

void                    hbs::CMP_6522::TickBus(void)
{
  if (GetPin(34) == hbs::FALSE)
    {
      for (size_t i = 0; i < 16; ++i)
        reg[i] = 0;
      last_write_tick = timer.GetTime();
      return ;
    }
  if (!Selected() || GetPin(22) != hbs::FALSE)
    return ;
  if (last_write_tick == timer.GetTime())
    return ;
  last_write_tick = timer.GetTime();
  WriteRegister(RegisterAddress(), ReadDataBus());
}

hbs::Tristate           hbs::CMP_6522::DataBit(size_t n)
{
  uint8_t               v;

  TickBus();
  if (!Selected() || GetPin(22) != hbs::TRUE)
    return (hbs::HIGH_IMPEDANCE);
  v = ReadRegister(RegisterAddress());
  for (size_t i = 0; i < 8; ++i)
    if (n == data_pin_6522[i])
      return ((v & (1 << i)) ? hbs::TRUE : hbs::FALSE);
  return (hbs::UNDEFINED);
}

hbs::Tristate           hbs::CMP_6522::PortBit(size_t n)
{
  for (size_t i = 0; i < 8; ++i)
    {
      if (n == pa_pin[i])
        {
          if ((reg[3] & (1 << i)) == 0)
            return (hbs::HIGH_IMPEDANCE);
          return ((reg[1] & (1 << i)) ? hbs::TRUE : hbs::FALSE);
        }
      if (n == pb_pin[i])
        {
          if ((reg[2] & (1 << i)) == 0)
            return (hbs::HIGH_IMPEDANCE);
          return ((reg[0] & (1 << i)) ? hbs::TRUE : hbs::FALSE);
        }
    }
  return (hbs::UNDEFINED);
}

bool                    hbs::CMP_6522::DrivesPin(size_t n) const
{
  if (n == 21)
    return (true);
  for (size_t i = 0; i < 8; ++i)
    if (n == data_pin_6522[i] || n == pa_pin[i] || n == pb_pin[i])
      return (true);
  return (false);
}

hbs::Tristate           hbs::CMP_6522::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  TickBus();
  if (n == 21)
    return (p->second = hbs::HIGH_IMPEDANCE); // IRQB inactive; timers/IRQ not modelled here yet.
  for (size_t i = 0; i < 8; ++i)
    if (n == data_pin_6522[i])
      return (p->second = DataBit(n));
  for (size_t i = 0; i < 8; ++i)
    if (n == pa_pin[i] || n == pb_pin[i])
      return (p->second = PortBit(n));
  if (n == 1 || n == 20)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_6522::CMP_6522(const hbs::Timer  &time,
                        const std::string &name,
                        const std::string &position)
  : AComponent(time, name, position),
    last_write_tick(0)
{
  for (size_t i = 0; i < 16; ++i)
    reg[i] = 0;
}

hbs::CMP_6522::~CMP_6522(void)
{}

bool                    hbs::CMP_6522::TypeMatches(const std::string &type)
{
  if (type == "6522" || type == "MOS6522" || type == "VIA6522")
    return (true);
  if (type == "W65C22" || type == "W65C22S" || type == "65C22")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_6522::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_6522(timer, name, position));
}

std::string             hbs::CMP_6522::GetDisplayType(void) const
{
  return ("6522/W65C22S VIA, simplified GPIO model");
}

std::string             hbs::CMP_6522::GetDefinitionValue(void) const
{
  return ("Registers modelled: ORB/ORA/DDRB/DDRA. Timers, shift register and interrupts are not cycle-modelled yet.");
}
