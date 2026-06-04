// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "6264.hpp"

const std::string       &hbs::CMP_6264::GetType(void) const
{
  return (type = "6264");
}

static const size_t     address_pin_6264[13] =
{
  10, 9, 8, 7, 6, 5, 4, 3, 25, 24, 21, 23, 2
};

static const size_t     data_pin_6264[8] =
{
  11, 12, 13, 15, 16, 17, 18, 19
};

bool                    hbs::CMP_6264::Selected(void)
{
  return (GetPin(20) == hbs::FALSE && GetPin(26) == hbs::TRUE);
}

bool                    hbs::CMP_6264::OutputEnabled(void)
{
  return (Selected() && GetPin(22) == hbs::FALSE && GetPin(27) == hbs::TRUE);
}

bool                    hbs::CMP_6264::WriteEnabled(void)
{
  return (Selected() && GetPin(27) == hbs::FALSE);
}

size_t                  hbs::CMP_6264::Address(void)
{
  size_t                addr = 0;

  for (size_t i = 0; i < 13; ++i)
    if (GetPin(address_pin_6264[i]) == hbs::TRUE)
      addr |= (size_t)1 << i;
  return (addr & 0x1FFF);
}

uint8_t                 hbs::CMP_6264::ReadDataBus(void)
{
  uint8_t               out = 0;

  for (size_t i = 0; i < 8; ++i)
    if (GetPin(data_pin_6264[i]) == hbs::TRUE)
      out |= (uint8_t)(1 << i);
  return (out);
}

void                    hbs::CMP_6264::TickWrite(void)
{
  if (!WriteEnabled())
    return ;
  if (last_write_tick == timer.GetTime())
    return ;
  last_write_tick = timer.GetTime();
  memory[Address()] = ReadDataBus();
}

hbs::Tristate           hbs::CMP_6264::DataBit(size_t n)
{
  uint8_t               v;

  TickWrite();
  if (!OutputEnabled())
    return (hbs::HIGH_IMPEDANCE);
  v = memory[Address()];
  for (size_t i = 0; i < 8; ++i)
    if (n == data_pin_6264[i])
      return ((v & (1 << i)) ? hbs::TRUE : hbs::FALSE);
  return (hbs::UNDEFINED);
}

bool                    hbs::CMP_6264::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < 8; ++i)
    if (n == data_pin_6264[i])
      return (true);
  return (false);
}

hbs::Tristate           hbs::CMP_6264::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    return (p->second = DataBit(n));
  if (n == 1 || n == 14 || n == 28)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_6264::CMP_6264(const hbs::Timer   &time,
                        const std::string  &name,
                        const std::string  &file,
                        const std::string  &position)
  : AComponent(time, name, position),
    memory(8192),
    last_write_tick(0)
{
  LoadFile(file, memory, 0x00);
}

hbs::CMP_6264::~CMP_6264(void)
{}

bool                    hbs::CMP_6264::TypeMatches(const std::string &type)
{
  if (type == "6264" || type == "SRAM6264")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_6264::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_6264(timer, name, value, position));
}

std::string             hbs::CMP_6264::GetDisplayType(void) const
{
  return ("6264 8Kx8 SRAM");
}
