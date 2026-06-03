// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4541.hpp"

const std::string       &hbs::CMP_4541::GetType(void) const
{
  return (type = "4541");
}

bool                    hbs::CMP_4541::DrivesPin(size_t n) const
{
  return (n == 8);
}

void                    hbs::CMP_4541::Tick(void)
{
  if (GetPin(6) == hbs::TRUE)
    {
      counter = 0;
      last_tick = timer.GetTime();
      return ;
    }
  if (last_tick == timer.GetTime())
    return ;
  last_tick = timer.GetTime();
  counter = (counter + 1) & 0xFFFF;
}

unsigned int            hbs::CMP_4541::SelectedStage(void)
{
  bool                  a = GetPin(12) == hbs::TRUE;
  bool                  b = GetPin(13) == hbs::TRUE;

  if (!a && !b)
    return (13);
  if (!a && b)
    return (16);
  if (a && !b)
    return (8);
  return (10);
}

hbs::Tristate           hbs::CMP_4541::Output(void)
{
  unsigned int          stage = SelectedStage();
  bool                  bit;

  bit = (counter & (1u << (stage - 1))) != 0;
  if (GetPin(10) == hbs::FALSE && bit)
    bit = true;
  if (GetPin(9) == hbs::TRUE)
    bit = !bit;
  return (bit ? hbs::TRUE : hbs::FALSE);
}

hbs::Tristate           hbs::CMP_4541::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 8)
    {
      Tick();
      return (p->second = Output());
    }
  if (n == 7 || n == 14)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4541::CMP_4541(const hbs::Timer       &time,
                        const std::string      &name,
                        const std::string      &pos)
  : AComponent(time, name, pos),
    counter(0),
    last_tick(0)
{}

hbs::CMP_4541::~CMP_4541(void)
{}

bool                    hbs::CMP_4541::TypeMatches(const std::string &type)
{
  if (type == "4541" || type == "74HC4541" || type == "74HCT4541")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4541::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4541(timer, name, position));
}

std::string             hbs::CMP_4541::GetDisplayType(void) const
{
  return ("4541 Programmable timer / oscillator, simplified digital model");
}
