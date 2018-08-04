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

nts::IComponent		*nts::Circuit::Create(const std::string		&type,
					      const std::string		&value)
{
  if (type == "input")
    return (new nts::Input(timer, value));
  if (type == "clock")
    return (new nts::Clock(timer, value));
  if (type == "output")
    return (new nts::Output(timer));
  if (type == "true")
    return (new nts::True(timer));
  if (type == "false")
    return (new nts::False(timer));

  if (type == "4001")
    return (new nts::CMP_4001(timer));
  if (type == "4008")
    return (new nts::CMP_4008(timer));
  if (type == "4011")
    return (new nts::CMP_4011(timer));
  if (type == "4013")
    return (new nts::CMP_4013(timer));
  if (type == "4017")
    return (new nts::CMP_4017(timer));
  if (type == "4030")
    return (new nts::CMP_4030(timer));
  if (type == "4040")
    return (new nts::CMP_4040(timer));
  if (type == "4069")
    return (new nts::CMP_4069(timer));
  if (type == "4071")
    return (new nts::CMP_4071(timer));
  if (type == "4081")
    return (new nts::CMP_4081(timer));
  if (type == "4094")
    return (new nts::CMP_4094(timer));
  if (type == "4514")
    return (new nts::CMP_4514(timer));

  if (type == "2716")
    return (new nts::CMP_2716(timer, value));

  if (type == "terminal")
    return (new nts::Terminal(timer));

  throw nts::BadComponent(std::string("Unknown component ") + type);
  return (NULL);
}

