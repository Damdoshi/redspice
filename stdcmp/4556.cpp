// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4556.hpp"

const std::string	&hbs::CMP_4556::GetType(void) const
{
  return (type = "4556");
}

hbs::CMP_4556::CMP_4556(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : CMP_4555(time, name, pos)
{}

hbs::CMP_4556::~CMP_4556(void)
{}

bool			hbs::CMP_4556::TypeMatches(const std::string &type)
{
  if (type == "4556")
    return (true);
  if (type == "74139")
    return (true);
  if (type == "74HC139")
    return (true);
  if (type == "74LS139")
    return (true);
  if (type == "74HC4556")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4556::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4556(timer, name, position));
}

std::string		hbs::CMP_4556::GetDisplayType(void) const
{
  return ("4556/74139 Dual 2-to-4 decoder/demultiplexer, active-low outputs");
}
