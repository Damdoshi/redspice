// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "4538.hpp"

const std::string       &hbs::CMP_4538::GetType(void) const
{
  return (type = "4538");
}

hbs::CMP_4538::CMP_4538(const hbs::Timer       &time,
                        const std::string      &name,
                        const std::string      &pos)
  : CMP_4098(time, name, pos)
{}

hbs::CMP_4538::~CMP_4538(void)
{}

bool                    hbs::CMP_4538::TypeMatches(const std::string &type)
{
  if (type == "4538" || type == "74HC4538" || type == "74HCT4538")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_4538::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4538(timer, name, position));
}

std::string             hbs::CMP_4538::GetDisplayType(void) const
{
  return ("4538 Dual precision monostable multivibrator, simplified digital model");
}
