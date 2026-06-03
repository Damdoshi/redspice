// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4098.hpp"

const std::string       &hbs::CMP_4098::GetType(void) const
{
  return (type = "4098");
}

static const size_t     a_pin[2] = {4, 12};
static const size_t     b_pin[2] = {5, 11};
static const size_t     clear_pin[2] = {3, 13};
static const size_t     q_pin[2] = {6, 10};
static const size_t     nq_pin[2] = {7, 9};

bool                    hbs::CMP_4098::DrivesPin(size_t n) const
{
  return (n == 6 || n == 7 || n == 9 || n == 10);
}

bool                    hbs::CMP_4098::Triggered(size_t id)
{
  if (GetPrevious(a_pin[id]) == hbs::TRUE && GetPin(a_pin[id]) == hbs::FALSE)
    return (true);
  if (GetPrevious(b_pin[id]) == hbs::FALSE && GetPin(b_pin[id]) == hbs::TRUE)
    return (true);
  return (false);
}

bool                    hbs::CMP_4098::Active(size_t id) const
{
  return (pulse_until[id] != 0 && timer.GetTime() < pulse_until[id]);
}

void                    hbs::CMP_4098::Update(size_t id)
{
  if (last_tick[id] == timer.GetTime())
    return ;
  last_tick[id] = timer.GetTime();
  if (GetPin(clear_pin[id]) == hbs::FALSE)
    {
      pulse_until[id] = 0;
      return ;
    }
  if (Triggered(id))
    pulse_until[id] = timer.GetTime() + 2;
  else if (pulse_until[id] != 0 && timer.GetTime() >= pulse_until[id])
    pulse_until[id] = 0;
}

hbs::Tristate           hbs::CMP_4098::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;
  size_t                id;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      id = (n == 6 || n == 7) ? 0 : 1;
      Update(id);
      if (n == q_pin[id])
        return (p->second = Active(id) ? hbs::TRUE : hbs::FALSE);
      if (n == nq_pin[id])
        return (p->second = Active(id) ? hbs::FALSE : hbs::TRUE);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4098::CMP_4098(const hbs::Timer       &time,
                        const std::string      &name,
                        const std::string      &pos)
  : AComponent(time, name, pos)
{
  pulse_until[0] = pulse_until[1] = 0;
  last_tick[0] = last_tick[1] = 0;
}

hbs::CMP_4098::~CMP_4098(void)
{}

bool                    hbs::CMP_4098::TypeMatches(const std::string &type)
{
  if (type == "4098" || type == "74HC4098" || type == "74HCT4098")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4098::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4098(timer, name, position));
}

std::string             hbs::CMP_4098::GetDisplayType(void) const
{
  return ("4098 Dual monostable multivibrator, simplified digital model");
}
