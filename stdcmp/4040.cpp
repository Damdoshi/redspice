// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		<stdio.h>
#include		"4040.hpp"
#define			XXX					0
#define			PIN					-1 + 
#define			STR(a)					a - 1

const std::string	&hbs::CMP_4040::GetType(void) const
{
  return (type = typeid(*this).name());
}

static const size_t	pinstrenght[16] =
  {
    [PIN  1] = STR(12),
    [PIN  2] = STR(6),
    [PIN  3] = STR(5),
    [PIN  4] = STR(7),
    [PIN  5] = STR(4),
    [PIN  6] = STR(3),
    [PIN  7] = STR(2),
    [PIN  8] = XXX,
    [PIN  9] = STR(1),
    [PIN 10] = XXX,
    [PIN 11] = XXX,
    [PIN 12] = STR(9),
    [PIN 13] = STR(8),
    [PIN 14] = STR(10),
    [PIN 15] = STR(11),
    [PIN 16] = XXX
  };

hbs::Tristate		hbs::CMP_4040::Compute(size_t		n)
{
  /// Check if it already computed (Maybe temporary...)
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);

  /// Create a new state base on the previous one
  Preset		p = PresetOutput(n);

  if ((n >= 1 && n <= 7) || n == 9 || (n >= 12 && n <= 15))
    {
      if (GetPin(11) == hbs::TRUE)
	{
	  value = 0;
	  p->second = hbs::FALSE;
	  return (p->second);
	}
      if (GetPin(10) == hbs::FALSE && GetPrevious(10) == hbs::TRUE && last_tick != timer.GetTime())
	{
	  last_tick = timer.GetTime();
	  value += 1;
	}
      if ((value >> pinstrenght[n - 1]) & 1)
	p->second = hbs::TRUE;
      else
	p->second = hbs::FALSE;
      return (p->second);
    }
  else if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4040::CMP_4040(const hbs::Timer		&time,
			const std::string		&pos)
  : AComponent(time, pos),
    value(0),
    last_tick(0)
{}

hbs::CMP_4040::~CMP_4040(void)
{}

