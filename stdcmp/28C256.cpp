// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "28C256.hpp"

const std::string       &hbs::CMP_28C256::GetType(void) const
{
  return (type = "28C256");
}

static const size_t     address_pin_28c256[15] =
{
  10, 9, 8, 7, 6, 5, 4, 3, 25, 24, 21, 23, 2, 26, 1
};

static const size_t     data_pin_28c256[8] =
{
  11, 12, 13, 15, 16, 17, 18, 19
};

bool                    hbs::CMP_28C256::Selected(void)
{
  return (GetPin(20) == hbs::FALSE);
}

bool                    hbs::CMP_28C256::OutputEnabled(void)
{
  return (Selected() && GetPin(22) == hbs::FALSE && GetPin(27) == hbs::TRUE);
}

bool                    hbs::CMP_28C256::WriteEnabled(void)
{
  return (Selected() && GetPin(27) == hbs::FALSE);
}

size_t                  hbs::CMP_28C256::Address(void)
{
  size_t                addr = 0;

  for (size_t i = 0; i < 15; ++i)
    if (GetPin(address_pin_28c256[i]) == hbs::TRUE)
      addr |= (size_t)1 << i;
  return (addr & 0x7FFF);
}

uint8_t                 hbs::CMP_28C256::ReadDataBus(void)
{
  uint8_t               out = 0;

  for (size_t i = 0; i < 8; ++i)
    if (GetPin(data_pin_28c256[i]) == hbs::TRUE)
      out |= (uint8_t)(1 << i);
  return (out);
}

void                    hbs::CMP_28C256::TickWrite(void)
{
  if (!WriteEnabled())
    return ;
  if (last_write_tick == timer.GetTime())
    return ;
  last_write_tick = timer.GetTime();
  memory[Address()] = ReadDataBus();
}

hbs::Tristate           hbs::CMP_28C256::DataBit(size_t n)
{
  uint8_t               v;

  TickWrite();
  if (!OutputEnabled())
    return (hbs::HIGH_IMPEDANCE);
  v = memory[Address()];
  for (size_t i = 0; i < 8; ++i)
    if (n == data_pin_28c256[i])
      return ((v & (1 << i)) ? hbs::TRUE : hbs::FALSE);
  return (hbs::UNDEFINED);
}

bool                    hbs::CMP_28C256::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 8; ++i)
    if (n == data_pin_28c256[i])
      return (true);
  return (false);
}

hbs::Tristate           hbs::CMP_28C256::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    return (p->second = DataBit(n));
  if (n == 14 || n == 28)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_28C256::CMP_28C256(const hbs::Timer &time,
                            const std::string &name,
                            const std::string &file,
                            const std::string &position)
  : AComponent(time, name, position),
    memory(32768),
    last_write_tick(0)
{
  LoadFile(file, memory, 0xFF);
}

hbs::CMP_28C256::~CMP_28C256(void)
{}

bool                    hbs::CMP_28C256::TypeMatches(const std::string &type)
{
  if (type == "28C256" || type == "AT28C256" || type == "EEPROM28C256")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_28C256::Create(hbs::Timer        &timer,
                                                 const std::string &type,
                                                 const std::string &name,
                                                 const std::string &value,
                                                 const std::string &position)
{
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_28C256(timer, name, value, position));
}

std::string             hbs::CMP_28C256::GetDisplayType(void) const
{
  return ("28C256 32Kx8 EEPROM");
}
