// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4011.hpp"

const std::string	&hbs::CMP_4011::GetType(void) const
{
  return (type = "4011");
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::CMP_4011::Logic(hbs::Tristate	a,
					     hbs::Tristate	b) const
{
  switch (CMP_4081::Logic(a, b))
    {
    case hbs::TRUE:
      return (hbs::FALSE);
    case hbs::FALSE:
      return (hbs::TRUE);
    default:
      return (hbs::UNDEFINED);
    }
  return (hbs::UNDEFINED);
}

hbs::CMP_4011::CMP_4011(const hbs::Timer	&timer,
			const std::string	&name,
			const std::string	&pos)
  : CMP_4081(timer, name, pos)
{}
hbs::CMP_4011::~CMP_4011(void)
{}


bool			hbs::CMP_4011::TypeMatches(const std::string	&type)
{
  if (type == "4011")
    return (true);
  if (type == "7400")
    return (true);
  if (type == "74HC00")
    return (true);
  if (type == "74HCT00")
    return (true);
  if (type == "74LS00")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4011::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4011(timer, name, position));
}

std::string		hbs::CMP_4011::GetDisplayType(void) const
{
  return ("4011/7400 Quad 2-input NAND");
}
