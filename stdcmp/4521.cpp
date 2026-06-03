// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4521.hpp"

const std::string       &hbs::CMP_4521::GetType(void) const
{
  return (type = "4521");
}

struct                  DividerOutput
{
  size_t                pin;
  unsigned int          stage;
};

static const DividerOutput outputs[] = {
  {10, 18}, {11, 19}, {12, 20}, {13, 21}, {14, 22}, {15, 23}, {1, 24}
};

bool                    hbs::CMP_4521::DrivesPin(size_t n) const
{
  if (n == 4 || n == 7)
    return (true);
  for (size_t i = 0; i < sizeof(outputs) / sizeof(outputs[0]); ++i)
    if (n == outputs[i].pin)
      return (true);
  return (false);
}

void                    hbs::CMP_4521::Tick(void)
{
  if (GetPin(2) == hbs::TRUE)
    {
      counter = 0;
      last_tick = timer.GetTime();
      return ;
    }
  if (last_tick == timer.GetTime())
    return ;
  last_tick = timer.GetTime();
  counter = (counter + 1) & 0xFFFFFF;
}

hbs::Tristate           hbs::CMP_4521::Output(size_t n) const
{
  if (n == 7)
    return ((counter & 1) ? hbs::TRUE : hbs::FALSE);
  if (n == 4)
    return ((counter & 1) ? hbs::FALSE : hbs::TRUE);
  for (size_t i = 0; i < sizeof(outputs) / sizeof(outputs[0]); ++i)
    if (n == outputs[i].pin)
      return ((counter & (1u << (outputs[i].stage - 1))) ? hbs::TRUE : hbs::FALSE);
  return (hbs::UNDEFINED);
}

hbs::Tristate           hbs::CMP_4521::Compute(size_t n)
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

hbs::CMP_4521::CMP_4521(const hbs::Timer       &time,
                        const std::string      &name,
                        const std::string      &pos)
  : AComponent(time, name, pos),
    counter(0),
    last_tick(0)
{}

hbs::CMP_4521::~CMP_4521(void)
{}

bool                    hbs::CMP_4521::TypeMatches(const std::string &type)
{
  if (type == "4521" || type == "74HC4521" || type == "74HCT4521")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4521::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4521(timer, name, position));
}

std::string             hbs::CMP_4521::GetDisplayType(void) const
{
  return ("4521 24-stage frequency divider, simplified digital model");
}
