// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"4511.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_4511::GetType(void) const
{
  return (type = "4511");
}

static int		segment_index(size_t n)
{
  static const int map[16] =
    {
      -1, -1, -1, -1, -1, -1, -1, -1,
       4,  3,  2,  1,  0,  6,  5, -1
    };

  if (n == 0 || n > 16)
    return (-1);
  return (map[n - 1]);
}

static bool		read_bcd(hbs::CMP_4511 &component, unsigned char &value)
{
  hbs::Tristate		pins[4];

  pins[0] = component.GetPin(7);
  pins[1] = component.GetPin(1);
  pins[2] = component.GetPin(2);
  pins[3] = component.GetPin(6);
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

static hbs::Tristate	segment_value(unsigned char value, int segment)
{
  static const bool table[10][7] =
    {
      {true,  true,  true,  true,  true,  true,  false},
      {false, true,  true,  false, false, false, false},
      {true,  true,  false, true,  true,  false, true },
      {true,  true,  true,  true,  false, false, true },
      {false, true,  true,  false, false, true,  true },
      {true,  false, true,  true,  false, true,  true },
      {true,  false, true,  true,  true,  true,  true },
      {true,  true,  true,  false, false, false, false},
      {true,  true,  true,  true,  true,  true,  true },
      {true,  true,  true,  true,  false, true,  true }
    };

  if (value > 9 || segment < 0 || segment > 6)
    return (hbs::FALSE);
  return (table[value][segment] ? hbs::TRUE : hbs::FALSE);
}

void			hbs::CMP_4511::UpdateLatch(void)
{
  hbs::Tristate		le;
  unsigned char		value;

  if (last_update == timer.GetTime())
    return ;
  last_update = timer.GetTime();
  le = GetPin(5);
  if (le == hbs::TRUE)
    return ;
  if (le != hbs::FALSE)
    {
      latched_valid = false;
      return ;
    }
  if (!read_bcd(*this, value))
    {
      latched_valid = false;
      return ;
    }
  latched_value = value;
  latched_valid = true;
}

hbs::Tristate		hbs::CMP_4511::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  int			segment;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  segment = segment_index(n);
  if (segment >= 0)
    {
      if (GetPin(3) == hbs::FALSE)
	return (p->second = hbs::TRUE);
      if (GetPin(4) == hbs::FALSE)
	return (p->second = hbs::FALSE);
      UpdateLatch();
      if (!latched_valid)
	return (p->second = hbs::UNDEFINED);
      return (p->second = segment_value(latched_value, segment));
    }
  if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4511::CMP_4511(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos),
    latched_value(0),
    latched_valid(false),
    last_update((size_t)-1)
{}

hbs::CMP_4511::~CMP_4511(void)
{}

bool			hbs::CMP_4511::TypeMatches(const std::string &type)
{
  if (type == "4511")
    return (true);
  if (type == "74HC4511")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4511::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4511(timer, name, position));
}

std::string		hbs::CMP_4511::GetDisplayType(void) const
{
  return ("4511 BCD-to-7-segment latch/decoder/driver");
}
