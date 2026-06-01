// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4042.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_4042::GetType(void) const
{
  return (type = "4042");
}

static int              q_index(size_t n)
{
  static const size_t qpins[4] = {2, 10, 12, 1};
  static const size_t nqpin[4] = {3, 9, 11, 15};

  for (size_t i = 0; i < 4; ++i)
    if (n == qpins[i] || n == nqpin[i])
      return ((int)i);
  return (-1);
}

static bool             inverted_output(size_t n)
{
  return (n == 3 || n == 9 || n == 11 || n == 15);
}

bool                    hbs::CMP_4042::DrivesPin(size_t n) const
{
  return (q_index(n) >= 0);
}

void                    hbs::CMP_4042::UpdateLatch(void)
{
  static const size_t   dpins[4] = {4, 7, 13, 14};
  hbs::Tristate         clk;
  hbs::Tristate         polarity;

  if (last_update == timer.GetTime())
    return ;
  last_update = timer.GetTime();
  clk = GetPin(5);
  polarity = GetPin(6);
  if (!hbs::LogicKnown(clk) || !hbs::LogicKnown(polarity))
    return ;
  if (clk != polarity)
    return ;
  for (size_t i = 0; i < 4; ++i)
    {
      value[i] = GetPin(dpins[i]);
      valid[i] = hbs::LogicKnown(value[i]);
    }
}

hbs::Tristate           hbs::CMP_4042::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;
  int                   idx;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  idx = q_index(n);
  if (idx >= 0)
    {
      UpdateLatch();
      if (!valid[idx])
        return (p->second = hbs::UNDEFINED);
      if (inverted_output(n))
        return (p->second = hbs::LogicNot(value[idx]));
      return (p->second = value[idx]);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4042::CMP_4042(const hbs::Timer        &time,
                        const std::string       &name,
                        const std::string       &pos)
  : AComponent(time, name, pos),
    last_update((size_t)-1)
{
  for (size_t i = 0; i < 4; ++i)
    {
      value[i] = hbs::UNDEFINED;
      valid[i] = false;
    }
}

hbs::CMP_4042::~CMP_4042(void)
{}

bool                    hbs::CMP_4042::TypeMatches(const std::string &type)
{
  if (type == "4042")
    return (true);
  if (type == "74HC4042")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4042::Create(hbs::Timer           &timer,
                                               const std::string    &type,
                                               const std::string    &name,
                                               const std::string    &value,
                                               const std::string    &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4042(timer, name, position));
}

std::string             hbs::CMP_4042::GetDisplayType(void) const
{
  return ("4042 Quad clocked D latch");
}
