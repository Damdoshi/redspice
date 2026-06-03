// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "7475.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_7475::GetType(void) const
{
  return (type = "7475");
}

static size_t           data_pin(size_t bit)
{
  static const size_t   pin[4] = {3, 4, 12, 13};

  return (pin[bit]);
}

static size_t           enable_pin(size_t bit)
{
  return (bit < 2 ? 7 : 9);
}

void                    hbs::CMP_7475::UpdateLatch(void)
{
  if (last_tick == timer.GetTime())
    return ;
  for (size_t i = 0; i < 4; ++i)
    if (GetPin(enable_pin(i)) == hbs::TRUE)
      {
        if (GetPin(data_pin(i)) == hbs::TRUE)
          value.set(i);
        else if (GetPin(data_pin(i)) == hbs::FALSE)
          value.reset(i);
      }
  last_tick = timer.GetTime();
}

hbs::Tristate           hbs::CMP_7475::Output(size_t n) const
{
  if (n == 2)
    return (value[0] ? hbs::TRUE : hbs::FALSE);
  if (n == 1)
    return (value[0] ? hbs::FALSE : hbs::TRUE);
  if (n == 5)
    return (value[1] ? hbs::TRUE : hbs::FALSE);
  if (n == 6)
    return (value[1] ? hbs::FALSE : hbs::TRUE);
  if (n == 11)
    return (value[2] ? hbs::TRUE : hbs::FALSE);
  if (n == 10)
    return (value[2] ? hbs::FALSE : hbs::TRUE);
  if (n == 14)
    return (value[3] ? hbs::TRUE : hbs::FALSE);
  if (n == 15)
    return (value[3] ? hbs::FALSE : hbs::TRUE);
  return (hbs::UNDEFINED);
}

bool                    hbs::CMP_7475::DrivesPin(size_t n) const
{
  return (n == 1 || n == 2 || n == 5 || n == 6 ||
          n == 10 || n == 11 || n == 14 || n == 15);
}

hbs::Tristate           hbs::CMP_7475::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      UpdateLatch();
      return (p->second = Output(n));
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_7475::CMP_7475(const hbs::Timer   &time,
                        const std::string  &name,
                        const std::string  &position)
  : AComponent(time, name, position),
    last_tick(0)
{}

hbs::CMP_7475::~CMP_7475(void)
{}

bool                    hbs::CMP_7475::TypeMatches(const std::string &type)
{
  if (type == "7475" || type == "74HC75" || type == "74HCT75" || type == "74LS75")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_7475::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_7475(timer, name, position));
}

std::string             hbs::CMP_7475::GetDisplayType(void) const
{
  return ("7475 4-bit bistable latch with complementary outputs");
}
