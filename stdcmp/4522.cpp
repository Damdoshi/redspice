// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4522.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_4522::GetType(void) const
{
  return (type = canonical);
}

unsigned char           hbs::CMP_4522::ReadPreset(void)
{
  unsigned char         out = 0;

  if (GetPin(5) == hbs::TRUE)  out |= 1 << 0;
  if (GetPin(11) == hbs::TRUE) out |= 1 << 1;
  if (GetPin(14) == hbs::TRUE) out |= 1 << 2;
  if (GetPin(2) == hbs::TRUE)  out |= 1 << 3;
  return (out % modulo);
}

void                    hbs::CMP_4522::Update(void)
{
  if (last_tick == timer.GetTime())
    return ;
  last_tick = timer.GetTime();
  if (GetPin(10) == hbs::TRUE)
    {
      value = 0;
      return ;
    }
  if (GetPin(3) == hbs::TRUE)
    {
      value = ReadPreset();
      return ;
    }
  if (GetPin(4) == hbs::TRUE)
    return ;
  if (!(GetPrevious(6) == hbs::FALSE && GetPin(6) == hbs::TRUE))
    return ;
  if (value == 0)
    value = ReadPreset();
  else
    --value;
}

hbs::Tristate           hbs::CMP_4522::OutputBit(size_t n) const
{
  unsigned int          bit;

  if (n == 7) bit = 0;
  else if (n == 9) bit = 1;
  else if (n == 15) bit = 2;
  else if (n == 1) bit = 3;
  else return (hbs::UNDEFINED);
  return (((value >> bit) & 1) ? hbs::TRUE : hbs::FALSE);
}

bool                    hbs::CMP_4522::DrivesPin(size_t n) const
{
  return (n == 1 || n == 7 || n == 9 || n == 12 || n == 13 || n == 15);
}

hbs::Tristate           hbs::CMP_4522::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      Update();
      if (n == 12 || n == 13)
        return (p->second = (value == 0 ? hbs::TRUE : hbs::FALSE));
      return (p->second = OutputBit(n));
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4522::CMP_4522(const hbs::Timer  &time,
                        const std::string &name,
                        const std::string &position,
                        const std::string &canonical,
                        const std::string &display,
                        unsigned char      modulo)
  : AComponent(time, name, position),
    canonical(canonical),
    display(display),
    modulo(modulo),
    value(0),
    last_tick(0)
{}

hbs::CMP_4522::~CMP_4522(void)
{}

bool                    hbs::CMP_4522::TypeMatches(const std::string &type)
{
  if (type == "4522" || type == "74HC4522" || type == "74HCT4522")
    return (true);
  if (type == "4526" || type == "74HC4526" || type == "74HCT4526")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4522::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (type == "4522" || type == "74HC4522" || type == "74HCT4522")
    return (new hbs::CMP_4522(timer, name, position, "4522", "4522 Programmable BCD divide-by-N counter", 10));
  if (type == "4526" || type == "74HC4526" || type == "74HCT4526")
    return (new hbs::CMP_4522(timer, name, position, "4526", "4526 Programmable binary divide-by-N counter", 16));
  return (NULL);
}

std::string             hbs::CMP_4522::GetDisplayType(void) const
{
  return (display);
}
