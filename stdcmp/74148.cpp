// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "74148.hpp"
#include                "Logic.hpp"

const std::string       &hbs::CMP_74148::GetType(void) const
{
  return (type = "74148");
}

static const int        SELECT_BROKEN = -3;
static const int        SELECT_DISABLED = -2;
static const int        SELECT_UNDEFINED = -1;
static const int        SELECT_NONE = 8;
static const size_t     input_pin[8] = {10, 11, 12, 13, 1, 2, 3, 4};
static const size_t     output_pin[3] = {9, 7, 6};

bool                    hbs::CMP_74148::DrivesPin(size_t n) const
{
  return (n == 6 || n == 7 || n == 9 || n == 14 || n == 15);
}

static int              selected_input(hbs::CMP_74148 &component)
{
  bool                  uncertain = false;
  hbs::Tristate         v;

  if (component.GetPin(5) == hbs::TRUE)
    return (SELECT_DISABLED);
  if (component.GetPin(5) != hbs::FALSE)
    return (SELECT_UNDEFINED);
  for (int input = 7; input >= 0; --input)
    {
      v = component.GetPin(input_pin[input]);
      if (v == hbs::BROKEN)
        return (SELECT_BROKEN);
      if (v == hbs::FALSE)
        return (input);
      if (v != hbs::TRUE)
        uncertain = true;
    }
  if (uncertain)
    return (SELECT_UNDEFINED);
  return (SELECT_NONE);
}

hbs::Tristate           hbs::CMP_74148::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;
  int                   input;
  int                   bit;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (n == 6 || n == 7 || n == 9)
    {
      input = selected_input(*this);
      if (input == SELECT_DISABLED)
        return (p->second = hbs::TRUE);
      if (input == SELECT_BROKEN)
        return (p->second = hbs::BROKEN);
      if (input == SELECT_UNDEFINED)
        return (p->second = hbs::UNDEFINED);
      for (bit = 0; bit < 3; ++bit)
        if (n == output_pin[bit])
          return (p->second = ((input & (1 << bit)) ? hbs::FALSE : hbs::TRUE));
    }
  if (n == 14)
    {
      input = selected_input(*this);
      if (input == SELECT_DISABLED)
        return (p->second = hbs::TRUE);
      if (input == SELECT_BROKEN)
        return (p->second = hbs::BROKEN);
      if (input == SELECT_UNDEFINED)
        return (p->second = hbs::UNDEFINED);
      return (p->second = input == SELECT_NONE ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 15)
    {
      input = selected_input(*this);
      if (input == SELECT_DISABLED)
        return (p->second = hbs::TRUE);
      if (input == SELECT_BROKEN)
        return (p->second = hbs::BROKEN);
      if (input == SELECT_UNDEFINED)
        return (p->second = hbs::UNDEFINED);
      return (p->second = input == SELECT_NONE ? hbs::FALSE : hbs::TRUE);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74148::CMP_74148(const hbs::Timer   &time,
                          const std::string  &name,
                          const std::string  &position)
  : AComponent(time, name, position)
{}

hbs::CMP_74148::~CMP_74148(void)
{}

bool                    hbs::CMP_74148::TypeMatches(const std::string &type)
{
  if (type == "74148")
    return (true);
  if (type == "74HC148")
    return (true);
  if (type == "74HCT148")
    return (true);
  if (type == "74LS148")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_74148::Create(hbs::Timer        &timer,
                                                const std::string &type,
                                                const std::string &name,
                                                const std::string &value,
                                                const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74148(timer, name, position));
}

std::string             hbs::CMP_74148::GetDisplayType(void) const
{
  return ("74148 8-to-3 priority encoder, active-low inputs/outputs");
}
