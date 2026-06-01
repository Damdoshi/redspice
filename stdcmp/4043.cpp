// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4043.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_4043::GetType(void) const
{
  return (type = "4043");
}

static int              latch_index(size_t n)
{
  if (n == 2)
    return (0);
  if (n == 9)
    return (1);
  if (n == 10)
    return (2);
  if (n == 1)
    return (3);
  return (-1);
}

static void             latch_pins(size_t idx, size_t &set, size_t &reset)
{
  static const size_t   spins[4] = {4, 6, 12, 14};
  static const size_t   rpins[4] = {3, 7, 11, 15};

  set = spins[idx];
  reset = rpins[idx];
}

void                    hbs::CMP_4043::UpdateLatch(void)
{
  if (last_update == timer.GetTime())
    return ;
  last_update = timer.GetTime();
  for (size_t i = 0; i < 4; ++i)
    {
      size_t            setpin;
      size_t            resetpin;
      hbs::Tristate     s;
      hbs::Tristate     r;

      latch_pins(i, setpin, resetpin);
      s = GetPin(setpin);
      r = GetPin(resetpin);
      if (!hbs::LogicKnown(s) || !hbs::LogicKnown(r))
        {
          valid[i] = false;
          value[i] = hbs::UNDEFINED;
        }
      else if (!NandLatch())
        {
          if (s == hbs::TRUE && r == hbs::TRUE)
            { valid[i] = false; value[i] = hbs::BROKEN; }
          else if (s == hbs::TRUE)
            { valid[i] = true; value[i] = hbs::TRUE; }
          else if (r == hbs::TRUE)
            { valid[i] = true; value[i] = hbs::FALSE; }
        }
      else
        {
          if (s == hbs::FALSE && r == hbs::FALSE)
            { valid[i] = false; value[i] = hbs::BROKEN; }
          else if (s == hbs::FALSE)
            { valid[i] = true; value[i] = hbs::TRUE; }
          else if (r == hbs::FALSE)
            { valid[i] = true; value[i] = hbs::FALSE; }
        }
    }
}

hbs::Tristate           hbs::CMP_4043::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;
  int                   idx;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  idx = latch_index(n);
  if (idx >= 0)
    {
      if (GetPin(5) == hbs::FALSE)
        return (p->second = hbs::HIGH_IMPEDANCE);
      if (GetPin(5) != hbs::TRUE)
        return (p->second = hbs::UNDEFINED);
      UpdateLatch();
      if (value[idx] == hbs::BROKEN)
        return (p->second = hbs::BROKEN);
      if (!valid[idx])
        return (p->second = hbs::UNDEFINED);
      return (p->second = value[idx]);
    }
  if (n == 8 || n == 13 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4043::CMP_4043(const hbs::Timer        &time,
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

hbs::CMP_4043::~CMP_4043(void)
{}

bool                    hbs::CMP_4043::TypeMatches(const std::string &type)
{
  if (type == "4043")
    return (true);
  if (type == "74HC4043")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4043::Create(hbs::Timer           &timer,
                                               const std::string    &type,
                                               const std::string    &name,
                                               const std::string    &value,
                                               const std::string    &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4043(timer, name, position));
}

std::string             hbs::CMP_4043::GetDisplayType(void) const
{
  return ("4043 Quad NOR R/S latch with 3-state outputs");
}
