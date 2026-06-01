// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4028.hpp"
#include		"Logic.hpp"
#define			PIN					-1 +

const std::string	&hbs::CMP_4028::GetType(void) const
{
  return (type = "4028");
}

static int		output_index_from_pin(size_t n)
{
  static const int map[16] =
    {
      [PIN  1] = 4,
      [PIN  2] = 2,
      [PIN  3] = 0,
      [PIN  4] = 7,
      [PIN  5] = 9,
      [PIN  6] = 5,
      [PIN  7] = 6,
      [PIN  8] = -1,
      [PIN  9] = 8,
      [PIN 10] = -1,
      [PIN 11] = -1,
      [PIN 12] = -1,
      [PIN 13] = -1,
      [PIN 14] = 1,
      [PIN 15] = 3,
      [PIN 16] = -1
    };

  if (n == 0 || n > 16)
    return (-1);
  return (map[n - 1]);
}

static bool		read_bcd(hbs::CMP_4028 &component, unsigned char &value)
{
  hbs::Tristate		pins[4];

  pins[0] = component.GetPin(10);
  pins[1] = component.GetPin(13);
  pins[2] = component.GetPin(12);
  pins[3] = component.GetPin(11);
  value = 0;
  for (size_t i = 0; i < 4; ++i)
    {
      if (!hbs::LogicKnown(pins[i]))
	return (false);
      if (pins[i] == hbs::TRUE)
	value |= 1 << i;
    }
  return (true);
}

bool			hbs::CMP_4028::DrivesPin(size_t n) const
{
  return (output_index_from_pin(n) >= 0);
}

hbs::Tristate		hbs::CMP_4028::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  unsigned char		value;
  int			output;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  output = output_index_from_pin(n);
  if (output >= 0)
    {
      if (!read_bcd(*this, value))
	return (p->second = hbs::UNDEFINED);
      if (value > 9)
	return (p->second = hbs::FALSE);
      return (p->second = value == output ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4028::CMP_4028(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_4028::~CMP_4028(void)
{}

bool			hbs::CMP_4028::TypeMatches(const std::string &type)
{
  if (type == "4028")
    return (true);
  if (type == "74HC4028")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4028::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4028(timer, name, position));
}

std::string		hbs::CMP_4028::GetDisplayType(void) const
{
  return ("4028 BCD-to-decimal decoder");
}
