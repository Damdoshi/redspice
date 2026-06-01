// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4044.hpp"

const std::string       &hbs::CMP_4044::GetType(void) const
{
  return (type = "4044");
}

hbs::CMP_4044::CMP_4044(const hbs::Timer        &time,
                        const std::string       &name,
                        const std::string       &pos)
  : CMP_4043(time, name, pos)
{}

hbs::CMP_4044::~CMP_4044(void)
{}

bool                    hbs::CMP_4044::TypeMatches(const std::string &type)
{
  if (type == "4044")
    return (true);
  if (type == "74HC4044")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4044::Create(hbs::Timer           &timer,
                                               const std::string    &type,
                                               const std::string    &name,
                                               const std::string    &value,
                                               const std::string    &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4044(timer, name, position));
}

std::string             hbs::CMP_4044::GetDisplayType(void) const
{
  return ("4044 Quad NAND R/S latch with 3-state outputs");
}
