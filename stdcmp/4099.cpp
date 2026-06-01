// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4099.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_4099::GetType(void) const
{
  return (type = "4099");
}

static int              output_index(size_t n)
{
  if (n == 1)
    return (7);
  if (n >= 9 && n <= 15)
    return ((int)(n - 9));
  return (-1);
}

bool                    hbs::CMP_4099::DrivesPin(size_t n) const
{
  return (output_index(n) >= 0);
}

void                    hbs::CMP_4099::UpdateLatch(void)
{
  hbs::Tristate         reset;
  hbs::Tristate         write_disable;
  hbs::Tristate         data;
  hbs::Tristate         addr[3];
  unsigned char         index;

  if (last_update == timer.GetTime())
    return ;
  last_update = timer.GetTime();
  reset = GetPin(2);
  write_disable = GetPin(4);
  if (reset == hbs::TRUE)
    {
      for (size_t i = 0; i < 8; ++i)
        value[i] = hbs::FALSE;
      return ;
    }
  if (reset != hbs::FALSE)
    {
      for (size_t i = 0; i < 8; ++i)
        value[i] = hbs::UNDEFINED;
      return ;
    }
  if (write_disable == hbs::TRUE)
    return ;
  if (write_disable != hbs::FALSE)
    return ;
  addr[0] = GetPin(5);
  addr[1] = GetPin(6);
  addr[2] = GetPin(7);
  data = GetPin(3);
  if (!hbs::LogicKnown(addr[0]) || !hbs::LogicKnown(addr[1]) || !hbs::LogicKnown(addr[2]) ||
      !hbs::LogicKnown(data))
    return ;
  index = 0;
  if (addr[0] == hbs::TRUE)
    index |= 1 << 0;
  if (addr[1] == hbs::TRUE)
    index |= 1 << 1;
  if (addr[2] == hbs::TRUE)
    index |= 1 << 2;
  value[index] = data;
}

hbs::Tristate           hbs::CMP_4099::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;
  int                   idx;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  idx = output_index(n);
  if (idx >= 0)
    {
      UpdateLatch();
      return (p->second = value[idx]);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4099::CMP_4099(const hbs::Timer        &time,
                        const std::string       &name,
                        const std::string       &pos)
  : AComponent(time, name, pos),
    last_update((size_t)-1)
{
  for (size_t i = 0; i < 8; ++i)
    value[i] = hbs::UNDEFINED;
}

hbs::CMP_4099::~CMP_4099(void)
{}

bool                    hbs::CMP_4099::TypeMatches(const std::string &type)
{
  if (type == "4099")
    return (true);
  if (type == "74259")
    return (true);
  if (type == "74HC259")
    return (true);
  if (type == "74LS259")
    return (true);
  if (type == "74HC4099")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4099::Create(hbs::Timer           &timer,
                                               const std::string    &type,
                                               const std::string    &name,
                                               const std::string    &value,
                                               const std::string    &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4099(timer, name, position));
}

std::string             hbs::CMP_4099::GetDisplayType(void) const
{
  return ("4099/74259 8-bit addressable latch");
}
