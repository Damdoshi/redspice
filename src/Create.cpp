// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		<sstream>
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
#include		"Track.hpp"

hbs::IComponent		*hbs::Circuit::Create(const std::string		&type,
					      const std::string		&name,
					      const std::string		&value,
					      const std::string		&pos)
{
  std::stringstream	ss;
  std::string		position;

  if (pos == "")
    {
      // Pin size is 10 by default
      ss << (10 + rand() % (int)((1440 / PINSIZE_DEFAULT) * 0.8)) << "," << (10 + rand() % (int)((900 / PINSIZE_DEFAULT) * 0.8)) << " ";
      position = ss.str();
    }
  else
    position = pos;

  if (type == "input")
    return (new hbs::Input(timer, name, value, position));
  if (type == "clock")
    return (new hbs::Clock(timer, name, value, position));
  if (type == "output")
    return (new hbs::Output(timer, name, position));
  if (type == "true")
    return (new hbs::True(timer, name, position));
  if (type == "false")
    return (new hbs::False(timer, name, position));

  if (type == "4001")
    return (new hbs::CMP_4001(timer, name, position));
  if (type == "4008")
    return (new hbs::CMP_4008(timer, name, position));
  if (type == "4011")
    return (new hbs::CMP_4011(timer, name, position));
  if (type == "4013")
    return (new hbs::CMP_4013(timer, name, position));
  if (type == "4017")
    return (new hbs::CMP_4017(timer, name, position));
  if (type == "4030")
    return (new hbs::CMP_4030(timer, name, position));
  if (type == "4040")
    return (new hbs::CMP_4040(timer, name, position));
  if (type == "4069")
    return (new hbs::CMP_4069(timer, name, position));
  if (type == "4071")
    return (new hbs::CMP_4071(timer, name, position));
  if (type == "4081")
    return (new hbs::CMP_4081(timer, name, position));
  if (type == "4094")
    return (new hbs::CMP_4094(timer, name, position));
  if (type == "4514")
    return (new hbs::CMP_4514(timer, name, position));

  if (type == "2716")
    return (new hbs::CMP_2716(timer, name, value, position));

  if (type == "terminal")
    return (new hbs::Terminal(timer, name, position));

  if (type == "wire" || type == "track")
    return (new hbs::Track(timer, name, value));

  throw hbs::BadComponent(std::string("Unknown component ") + type);
  return (NULL);
}

