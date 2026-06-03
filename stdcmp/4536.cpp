// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4536.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_4536::GetType(void) const
{
  return (type = "4536");
}

bool                    hbs::CMP_4536::DrivesPin(size_t n) const
{
  return (n == 4 || n == 5 || n == 13);
}

void                    hbs::CMP_4536::Tick(void)
{
  if (GetPin(2) == hbs::TRUE)
    {
      counter = 0;
      last_tick = timer.GetTime();
      return ;
    }
  if (GetPin(1) == hbs::TRUE)
    counter = 0;
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(7) == hbs::TRUE || GetPin(14) == hbs::TRUE)
    return ;
  last_tick = timer.GetTime();
  counter = (counter + 1) & 0xFFFFFF;
}

unsigned int            hbs::CMP_4536::SelectedStage(void)
{
  unsigned int          select = 0;

  if (GetPin(9) == hbs::TRUE)
    select |= 1;
  if (GetPin(10) == hbs::TRUE)
    select |= 2;
  if (GetPin(11) == hbs::TRUE)
    select |= 4;
  if (GetPin(12) == hbs::TRUE)
    select |= 8;
  if (GetPin(6) == hbs::TRUE)
    return (select + 1);
  return (select + 9);
}

hbs::Tristate           hbs::CMP_4536::DecodedOutput(void)
{
  unsigned int          stage = SelectedStage();

  if (stage > 24)
    stage = 24;
  return ((counter & (1u << (stage - 1))) ? hbs::TRUE : hbs::FALSE);
}

hbs::Tristate           hbs::CMP_4536::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      Tick();
      if (n == 4)
        return (p->second = counter & 1 ? hbs::TRUE : hbs::FALSE);
      if (n == 5)
        return (p->second = counter & 1 ? hbs::FALSE : hbs::TRUE);
      return (p->second = DecodedOutput());
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4536::CMP_4536(const hbs::Timer       &time,
                        const std::string      &name,
                        const std::string      &pos)
  : AComponent(time, name, pos),
    counter(0),
    last_tick(0)
{}

hbs::CMP_4536::~CMP_4536(void)
{}

bool                    hbs::CMP_4536::TypeMatches(const std::string &type)
{
  if (type == "4536" || type == "74HC4536" || type == "74HCT4536")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4536::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4536(timer, name, position));
}

std::string             hbs::CMP_4536::GetDisplayType(void) const
{
  return ("4536 Programmable timer, simplified digital model");
}
