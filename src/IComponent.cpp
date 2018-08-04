// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		<iostream>
#include		"IComponent.hpp"

std::ostream		&operator<<(std::ostream	&os,
				    nts::Tristate	t)
{
  if (t == nts::TRUE)
    os << "1";
  else if (t == nts::FALSE)
    os << "0";
  else if (t == nts::UNDEFINED)
    os << "U";
  else
    os << "B";
  return (os);
}

