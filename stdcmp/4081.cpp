// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"Logic.hpp"
#include		"4081.hpp"

const std::string	&hbs::CMP_4081::GetType(void) const
{
  return (type = "4081");
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::CMP_4081::Logic(hbs::Tristate	a,
					     hbs::Tristate	b) const
{
  hbs::Tristate values[] = {a, b};

  return (hbs::LogicAnd(values, 2));
}

hbs::CMP_4081::CMP_4081(const hbs::Timer	&timer,
			const std::string	&name,
			const std::string	&pos)
  : AGate(timer, name, pos)
{}
hbs::CMP_4081::~CMP_4081(void)
{}


bool			hbs::CMP_4081::TypeMatches(const std::string	&type)
{
  if (type == "4081")
    return (true);
  if (type == "7408")
    return (true);
  if (type == "74HC08")
    return (true);
  if (type == "74HCT08")
    return (true);
  if (type == "74LS08")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4081::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4081(timer, name, position));
}

std::string		hbs::CMP_4081::GetDisplayType(void) const
{
  return ("4081/7408 Quad 2-input AND");
}
