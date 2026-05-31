// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4069.hpp"

const std::string	&hbs::CMP_4069::GetType(void) const
{
  return (type = "4069");
  return (type = typeid(*this).name());
}

static hbs::Tristate	Reverse(hbs::Tristate			n)
{
  if (n == hbs::TRUE)
    return (hbs::FALSE);
  if (n == hbs::FALSE)
    return (hbs::TRUE);
  return (n);
}

hbs::Tristate		hbs::CMP_4069::Compute(size_t		n)
{
  /// Check if it already computed (Maybe temporary...)
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);

  /// Create an undefined state that will be set
  Preset		p = PresetOutput(n);

  if (n == 2)
    return (p->second = Reverse(GetPin(1)));
  if (n == 4)
    return (p->second = Reverse(GetPin(3)));
  if (n == 6)
    return (p->second = Reverse(GetPin(5)));
  if (n == 8)
    return (p->second = Reverse(GetPin(9)));
  if (n == 10)
    return (p->second = Reverse(GetPin(11)));
  if (n == 12)
    return (p->second = Reverse(GetPin(13)));
  if (n == 7 || n == 14)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4069::CMP_4069(const hbs::Timer	&tim,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(tim, name, pos)
{}

hbs::CMP_4069::~CMP_4069(void)
{}


bool			hbs::CMP_4069::TypeMatches(const std::string	&type)
{
  if (type == "4069")
    return (true);
  if (type == "7404")
    return (true);
  if (type == "74HC04")
    return (true);
  if (type == "74HCT04")
    return (true);
  if (type == "74LS04")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4069::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4069(timer, name, position));
}

std::string		hbs::CMP_4069::GetDisplayType(void) const
{
  return ("4069/7404 Hex inverter");
}
