// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		<iostream>
#include		"IComponent.hpp"

std::ostream		&operator<<(std::ostream	&os,
				    hbs::Tristate	t)
{
  if (t == hbs::TRUE)
    os << "1";
  else if (t == hbs::FALSE)
    os << "0";
  else if (t == hbs::UNDEFINED)
    os << "U";
  else
    os << "B";
  return (os);
}

char			&operator<<(char		&c,
				    hbs::Tristate	t)
{
  if (t == hbs::TRUE)
    return (c = '1');
  if (t == hbs::FALSE)
    return (c = '0');
  if (t == hbs::UNDEFINED)
    return (c = 'U');
  return (c = 'B');
}
