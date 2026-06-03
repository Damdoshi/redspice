// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4089.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_4089::GetType(void) const
{
  return (type = canonical);
}

unsigned char           hbs::CMP_4089::ReadRate(void)
{
  unsigned char         out = 0;

  if (GetPin(14) == hbs::TRUE) out |= 1 << 0;
  if (GetPin(15) == hbs::TRUE) out |= 1 << 1;
  if (GetPin(2) == hbs::TRUE)  out |= 1 << 2;
  if (GetPin(3) == hbs::TRUE)  out |= 1 << 3;
  if (GetPin(4) == hbs::TRUE)
    out = modulo - 1;
  return (out % modulo);
}

void                    hbs::CMP_4089::Update(void)
{
  unsigned char         rate;
  unsigned int          sum;

  if (last_tick == timer.GetTime())
    return ;
  last_tick = timer.GetTime();
  pulse = hbs::FALSE;
  if (GetPin(13) == hbs::TRUE)
    {
      accumulator = 0;
      return ;
    }
  if (GetPin(11) == hbs::TRUE)
    return ;
  if (!(GetPrevious(9) == hbs::FALSE && GetPin(9) == hbs::TRUE))
    return ;
  rate = ReadRate();
  sum = accumulator + rate;
  if (sum >= modulo)
    {
      pulse = hbs::TRUE;
      sum -= modulo;
    }
  accumulator = sum;
}

bool                    hbs::CMP_4089::DrivesPin(size_t n) const
{
  return (n == 1 || n == 5 || n == 6 || n == 7);
}

hbs::Tristate           hbs::CMP_4089::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      Update();
      if (n == 6)
        return (p->second = pulse);
      if (n == 5)
        return (p->second = hbs::LogicNot(pulse));
      if (n == 1)
        return (p->second = (ReadRate() == modulo - 1 ? hbs::TRUE : hbs::FALSE));
      return (p->second = GetPin(11));
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4089::CMP_4089(const hbs::Timer  &time,
                        const std::string &name,
                        const std::string &position,
                        const std::string &canonical,
                        const std::string &display,
                        unsigned char      modulo)
  : AComponent(time, name, position),
    canonical(canonical),
    display(display),
    modulo(modulo),
    accumulator(0),
    pulse(hbs::FALSE),
    last_tick(0)
{}

hbs::CMP_4089::~CMP_4089(void)
{}

bool                    hbs::CMP_4089::TypeMatches(const std::string &type)
{
  if (type == "4089" || type == "74HC4089" || type == "74HCT4089")
    return (true);
  if (type == "4527" || type == "74HC4527" || type == "74HCT4527")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4089::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (type == "4089" || type == "74HC4089" || type == "74HCT4089")
    return (new hbs::CMP_4089(timer, name, position, "4089", "4089 Binary rate multiplier", 16));
  if (type == "4527" || type == "74HC4527" || type == "74HCT4527")
    return (new hbs::CMP_4089(timer, name, position, "4527", "4527 BCD rate multiplier", 10));
  return (NULL);
}

std::string             hbs::CMP_4089::GetDisplayType(void) const
{
  return (display);
}
