// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4017.hpp"
#define			XXX					0
#define			PIN					-1 + 

const std::string	&hbs::CMP_4017::GetType(void) const
{
  return (type = "4017");
  return (type = typeid(*this).name());
}

static const size_t	pinstrenght[16] =
  {
    [PIN  1] = 5,
    [PIN  2] = 1,
    [PIN  3] = 0,
    [PIN  4] = 2,
    [PIN  5] = 6,
    [PIN  6] = 7,
    [PIN  7] = 3,
    [PIN  8] = XXX,
    [PIN  9] = 8,
    [PIN 10] = 4,
    [PIN 11] = 9,
    [PIN 12] = XXX,
    [PIN 13] = XXX,
    [PIN 14] = XXX,
    [PIN 15] = XXX,
    [PIN 16] = XXX
  };

hbs::Tristate		hbs::CMP_4017::Compute(size_t		n)
{
  /// Check if it already computed (Maybe temporary...)
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);

  /// Create a new state base on the previous one
  Preset		p = PresetOutput(n);
  bool			tick;

  if ((n >= 1 && n <= 7) || (n >= 9 && n <= 11))
    {
      if (GetPin(15) == hbs::TRUE)
	{
	  value.reset();
	  value.set(0);
	  p->second = hbs::FALSE;
	  return (p->second);
	}
      tick = false;
      if (GetPin(14) == hbs::TRUE && GetPrevious(14) == hbs::FALSE)
	tick = true;
      else if (GetPin(13) == hbs::FALSE && GetPrevious(13) == hbs::TRUE)
	tick = true;
      if (tick && last_tick != timer.GetTime())
	{
	  last_tick = timer.GetTime();
	  if (!(value <<= 1).any())
	    value.set(0);
	}
      if (value[pinstrenght[n - 1]])
	p->second = hbs::TRUE;
      else
	p->second = hbs::FALSE;
      return (p->second);
    }
  else if (n == 12)
    {
      if (GetPin(15) == hbs::TRUE)
	p->second = hbs::TRUE;
      else if ((value >> 5).any())
	p->second = hbs::FALSE;
      else
	p->second = hbs::TRUE;
      return (p->second);
    }
  else if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4017::CMP_4017(const hbs::Timer		&time,
			const std::string		&name,
			const std::string		&pos)
  : AComponent(time, name, pos),
    value(1),
    last_tick(0)
{}

hbs::CMP_4017::~CMP_4017(void)
{}


bool			hbs::CMP_4017::TypeMatches(const std::string	&type)
{
  if (type == "4017")
    return (true);
  if (type == "74HC4017")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_4017::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_4017(timer, name, position));
}

std::string		hbs::CMP_4017::GetDisplayType(void) const
{
  return ("4017 Decade Johnson counter");
}
