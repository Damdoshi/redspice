// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4047.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_4047::GetType(void) const
{
  return (type = "4047");
}

bool                    hbs::CMP_4047::DrivesPin(size_t n) const
{
  return (n == 10 || n == 11 || n == 13);
}

bool                    hbs::CMP_4047::AstableMode(void)
{
  return (GetPin(4) == hbs::TRUE || GetPin(5) == hbs::FALSE);
}

bool                    hbs::CMP_4047::Triggered(void)
{
  if (GetPrevious(8) == hbs::FALSE && GetPin(8) == hbs::TRUE)
    return (true);
  if (GetPrevious(6) == hbs::TRUE && GetPin(6) == hbs::FALSE)
    return (true);
  if (GetPrevious(12) == hbs::FALSE && GetPin(12) == hbs::TRUE)
    return (true);
  return (false);
}

void                    hbs::CMP_4047::Update(void)
{
  if (last_tick == timer.GetTime())
    return ;
  last_tick = timer.GetTime();
  if (GetPin(9) == hbs::TRUE)
    {
      q = hbs::FALSE;
      pulse_until = 0;
      return ;
    }
  if (AstableMode())
    q = q == hbs::TRUE ? hbs::FALSE : hbs::TRUE;
  else if (Triggered())
    {
      q = hbs::TRUE;
      pulse_until = timer.GetTime() + 2;
    }
  else if (pulse_until != 0 && timer.GetTime() >= pulse_until)
    {
      q = hbs::FALSE;
      pulse_until = 0;
    }
}

hbs::Tristate           hbs::CMP_4047::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      Update();
      if (n == 10)
        return (p->second = q);
      if (n == 11)
        return (p->second = q == hbs::TRUE ? hbs::FALSE : hbs::TRUE);
      return (p->second = AstableMode() ? (timer.GetTime() & 1 ? hbs::TRUE : hbs::FALSE) : q);
    }
  if (n == 7 || n == 14)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4047::CMP_4047(const hbs::Timer       &time,
                        const std::string      &name,
                        const std::string      &pos)
  : AComponent(time, name, pos),
    q(hbs::FALSE),
    pulse_until(0),
    last_tick(0)
{}

hbs::CMP_4047::~CMP_4047(void)
{}

bool                    hbs::CMP_4047::TypeMatches(const std::string &type)
{
  if (type == "4047" || type == "74HC4047" || type == "74HCT4047")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4047::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4047(timer, name, position));
}

std::string             hbs::CMP_4047::GetDisplayType(void) const
{
  return ("4047 Monostable / astable multivibrator, simplified digital model");
}
