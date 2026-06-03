// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4060.hpp"

const std::string       &hbs::CMP_4060::GetType(void) const
{
  return (type = "4060");
}

struct                  OutputPin
{
  size_t                pin;
  unsigned int          stage;
};

static const OutputPin  output_pins[] = {
  {7, 4}, {5, 5}, {4, 6}, {6, 7}, {14, 8}, {13, 9},
  {15, 10}, {1, 12}, {2, 13}, {3, 14}
};

bool                    hbs::CMP_4060::DrivesPin(size_t n) const
{
  for (size_t i = 0; i < sizeof(output_pins) / sizeof(output_pins[0]); ++i)
    if (n == output_pins[i].pin)
      return (true);
  return (false);
}

void                    hbs::CMP_4060::Tick(void)
{
  if (GetPin(12) == hbs::TRUE)
    {
      counter = 0;
      last_tick = timer.GetTime();
      return ;
    }
  if (last_tick == timer.GetTime())
    return ;
  last_tick = timer.GetTime();
  counter = (counter + 1) & 0x3FFF;
}

hbs::Tristate           hbs::CMP_4060::Output(size_t n) const
{
  for (size_t i = 0; i < sizeof(output_pins) / sizeof(output_pins[0]); ++i)
    if (n == output_pins[i].pin)
      return ((counter & (1u << (output_pins[i].stage - 1))) ? hbs::TRUE : hbs::FALSE);
  return (hbs::UNDEFINED);
}

hbs::Tristate           hbs::CMP_4060::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      Tick();
      return (p->second = Output(n));
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4060::CMP_4060(const hbs::Timer       &time,
                        const std::string      &name,
                        const std::string      &pos)
  : AComponent(time, name, pos),
    counter(0),
    last_tick(0)
{}

hbs::CMP_4060::~CMP_4060(void)
{}

bool                    hbs::CMP_4060::TypeMatches(const std::string &type)
{
  if (type == "4060" || type == "74HC4060" || type == "74HCT4060")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4060::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4060(timer, name, position));
}

std::string             hbs::CMP_4060::GetDisplayType(void) const
{
  return ("4060 Oscillator + 14-stage divider, simplified digital model");
}
