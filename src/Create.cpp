// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"Exception.hpp"
#include		"Circuit.hpp"
#include		"Clock.hpp"
#include		"True.hpp"
#include		"False.hpp"
#include		"4001.hpp"
#include		"4008.hpp"
#include		"4011.hpp"
#include		"4013.hpp"
#include		"4017.hpp"
#include		"4030.hpp"
#include		"4040.hpp"
#include		"4069.hpp"
#include		"4071.hpp"
#include		"4081.hpp"
#include		"4094.hpp"
#include		"4514.hpp"

#include		"2716.hpp"

#include		"Terminal.hpp"

hbs::IComponent		*hbs::Circuit::Create(const std::string		&type,
					      const std::string		&value,
					      const std::string		&position)
{
  if (type == "input")
    return (new hbs::Input(timer, value, position));
  if (type == "clock")
    return (new hbs::Clock(timer, value, position));
  if (type == "output")
    return (new hbs::Output(timer, position));
  if (type == "true")
    return (new hbs::True(timer, position));
  if (type == "false")
    return (new hbs::False(timer, position));

  if (type == "4001")
    return (new hbs::CMP_4001(timer, position));
  if (type == "4008")
    return (new hbs::CMP_4008(timer, position));
  if (type == "4011")
    return (new hbs::CMP_4011(timer, position));
  if (type == "4013")
    return (new hbs::CMP_4013(timer, position));
  if (type == "4017")
    return (new hbs::CMP_4017(timer, position));
  if (type == "4030")
    return (new hbs::CMP_4030(timer, position));
  if (type == "4040")
    return (new hbs::CMP_4040(timer, position));
  if (type == "4069")
    return (new hbs::CMP_4069(timer, position));
  if (type == "4071")
    return (new hbs::CMP_4071(timer, position));
  if (type == "4081")
    return (new hbs::CMP_4081(timer, position));
  if (type == "4094")
    return (new hbs::CMP_4094(timer, position));
  if (type == "4514")
    return (new hbs::CMP_4514(timer, position));

  if (type == "2716")
    return (new hbs::CMP_2716(timer, value, position));

  if (type == "terminal")
    return (new hbs::Terminal(timer, position));

  throw hbs::BadComponent(std::string("Unknown component ") + type);
  return (NULL);
}

