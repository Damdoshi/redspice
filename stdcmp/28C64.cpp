// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "28C64.hpp"

const std::string       &hbs::CMP_28C64::GetType(void) const
{
  return (type = "28C64");
}

static const size_t     address_pin_28c64[13] =
{
  10, 9, 8, 7, 6, 5, 4, 3, 25, 24, 21, 23, 2
};

static const size_t     data_pin_28c64[8] =
{
  11, 12, 13, 15, 16, 17, 18, 19
};

bool                    hbs::CMP_28C64::Selected(void)
{
  return (GetPin(20) == hbs::FALSE);
}

bool                    hbs::CMP_28C64::OutputEnabled(void)
{
  return (Selected() && GetPin(22) == hbs::FALSE && GetPin(27) == hbs::TRUE);
}

bool                    hbs::CMP_28C64::WriteEnabled(void)
{
  return (Selected() && GetPin(27) == hbs::FALSE);
}

size_t                  hbs::CMP_28C64::Address(void)
{
  size_t                addr = 0;

  for (size_t i = 0; i < 13; ++i)
    if (GetPin(address_pin_28c64[i]) == hbs::TRUE)
      addr |= (size_t)1 << i;
  return (addr & 0x1FFF);
}

uint8_t                 hbs::CMP_28C64::ReadDataBus(void)
{
  uint8_t               out = 0;

  for (size_t i = 0; i < 8; ++i)
    if (GetPin(data_pin_28c64[i]) == hbs::TRUE)
      out |= (uint8_t)(1 << i);
  return (out);
}

void                    hbs::CMP_28C64::TickWrite(void)
{
  if (!WriteEnabled())
    return ;
  if (last_write_tick == timer.GetTime())
    return ;
  last_write_tick = timer.GetTime();
  memory[Address()] = ReadDataBus();
}

hbs::Tristate           hbs::CMP_28C64::DataBit(size_t n)
{
  uint8_t               v;

  TickWrite();
  if (!OutputEnabled())
    return (hbs::HIGH_IMPEDANCE);
  v = memory[Address()];
  for (size_t i = 0; i < 8; ++i)
    if (n == data_pin_28c64[i])
      return ((v & (1 << i)) ? hbs::TRUE : hbs::FALSE);
  return (hbs::UNDEFINED);
}

bool                    hbs::CMP_28C64::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 8; ++i)
    if (n == data_pin_28c64[i])
      return (true);
  return (false);
}

hbs::Tristate           hbs::CMP_28C64::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    return (p->second = DataBit(n));
  if (n == 1 || n == 14 || n == 26 || n == 28)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_28C64::CMP_28C64(const hbs::Timer  &time,
                          const std::string &name,
                          const std::string &file,
                          const std::string &position)
  : AComponent(time, name, position),
    memory(8192),
    last_write_tick(0)
{
  LoadFile(file, memory, 0xFF);
}

hbs::CMP_28C64::~CMP_28C64(void)
{}

bool                    hbs::CMP_28C64::TypeMatches(const std::string &type)
{
  if (type == "28C64" || type == "AT28C64" || type == "EEPROM28C64")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_28C64::Create(hbs::Timer        &timer,
                                                const std::string &type,
                                                const std::string &name,
                                                const std::string &value,
                                                const std::string &position)
{
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_28C64(timer, name, value, position));
}

std::string             hbs::CMP_28C64::GetDisplayType(void) const
{
  return ("28C64 8Kx8 EEPROM");
}
