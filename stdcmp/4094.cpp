// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4094.hpp"
#define			XXX					0
#define			PIN					-1 + 

const std::string	&hbs::CMP_4094::GetType(void) const
{
  return (type = "4094");
  return (type = typeid(*this).name());
}

static const size_t	pinstrenght[16] =
  {
    [PIN  1] = XXX,
    [PIN  2] = XXX,
    [PIN  3] = XXX,
    [PIN  4] = 0,
    [PIN  5] = 1,
    [PIN  6] = 2,
    [PIN  7] = 3,
    [PIN  8] = XXX,
    [PIN  9] = XXX,
    [PIN 10] = XXX,
    [PIN 11] = 7,
    [PIN 12] = 6,
    [PIN 13] = 5,
    [PIN 14] = 4,
    [PIN 15] = XXX,
    [PIN 16] = XXX
  };

hbs::Tristate		hbs::CMP_4094::Compute(size_t		n)
{
  /// Check if it already computed (Maybe temporary...)
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);

  /// Create a new state base on the previous one
  Preset		p = PresetOutput(n);

  if ((n >= 4 && n <= 8) || (n >= 9 && n <= 14))
    {
      // TICK UP
      if (GetPin(3) == hbs::TRUE && GetPrevious(3) == hbs::FALSE && last_tick_up != timer.GetTime())
	{
	  last_tick_up = timer.GetTime();
	  value <<= 1;
	  if (GetPin(2) == hbs::TRUE)
	    value.set(0);
	}
      // TICK DOWN
      if (GetPin(3) == hbs::FALSE && GetPrevious(3) == hbs::TRUE && last_tick_down != timer.GetTime())
	{
	  last_tick_down = timer.GetTime();
	  qns = value[7];
	}

      // STROBE
      if (GetPin(1) != hbs::TRUE)
	return (p->second);

      if (n == 10)
	{
	  p->second = qns ? hbs::TRUE : hbs::FALSE;
	  return (p->second);
	}
      if (n == 9)
	{
	  p->second = value[7] ? hbs::TRUE : hbs::FALSE;
	  return (p->second);
	}

      // OE
      if (GetPin(15) != hbs::TRUE)
	{
	  p->second = hbs::HIGH_IMPEDANCE;
	  return (p->second);
	}

      p->second = value[pinstrenght[n - 1]] ? hbs::TRUE : hbs::FALSE;
      return (p->second);
    }
  return (hbs::UNDEFINED);
}

hbs::CMP_4094::CMP_4094(const hbs::Timer		&time,
			const std::string		&name,
			const std::string		&pos)
  : AComponent(time, name, pos),
    qns(false),
    last_tick_up(0),
    last_tick_down(0)
{}

hbs::CMP_4094::~CMP_4094(void)
{}


bool			hbs::CMP_4094::TypeMatches(const std::string	&type)
{
  if (type == "4094")
    return (true);
  if (type == "74HC4094")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4094::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4094(timer, name, position));
}

std::string		hbs::CMP_4094::GetDisplayType(void) const
{
  return ("4094 8-stage shift-and-store bus register");
}
