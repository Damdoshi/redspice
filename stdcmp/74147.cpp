// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "74147.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_74147::GetType(void) const
{
  return (type = "74147");
}

static const int        SELECT_BROKEN = -2;
static const int        SELECT_UNDEFINED = -1;
static const size_t     input_pin[10] = {0, 11, 12, 13, 1, 2, 3, 4, 5, 10};
static const size_t     output_pin[4] = {9, 7, 6, 14};

bool                    hbs::CMP_74147::DrivesPin(size_t n) const
{
  return (n == 6 || n == 7 || n == 9 || n == 14);
}

static int              selected_digit(hbs::CMP_74147 &component)
{
  bool                  uncertain = false;
  hbs::Tristate         v;

  for (int digit = 9; digit >= 1; --digit)
    {
      v = component.GetPin(input_pin[digit]);
      if (v == hbs::BROKEN)
        return (SELECT_BROKEN);
      if (v == hbs::FALSE)
        return (digit);
      if (v != hbs::TRUE)
        uncertain = true;
    }
  if (uncertain)
    return (SELECT_UNDEFINED);
  return (0);
}

hbs::Tristate           hbs::CMP_74147::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;
  int                   digit;
  int                   bit;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  for (bit = 0; bit < 4; ++bit)
    if (n == output_pin[bit])
      {
        digit = selected_digit(*this);
        if (digit == SELECT_BROKEN)
          return (p->second = hbs::BROKEN);
        if (digit == SELECT_UNDEFINED)
          return (p->second = hbs::UNDEFINED);
        return (p->second = ((digit & (1 << bit)) ? hbs::FALSE : hbs::TRUE));
      }
  if (n == 8 || n == 15 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74147::CMP_74147(const hbs::Timer   &time,
                          const std::string  &name,
                          const std::string  &position)
  : AComponent(time, name, position)
{}

hbs::CMP_74147::~CMP_74147(void)
{}

bool                    hbs::CMP_74147::TypeMatches(const std::string &type)
{
  if (type == "74147")
    return (true);
  if (type == "74HC147")
    return (true);
  if (type == "74HCT147")
    return (true);
  if (type == "74LS147")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_74147::Create(hbs::Timer        &timer,
                                                const std::string &type,
                                                const std::string &name,
                                                const std::string &value,
                                                const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74147(timer, name, position));
}

std::string             hbs::CMP_74147::GetDisplayType(void) const
{
  return ("74147 10-to-4 priority encoder, active-low inputs/outputs");
}
