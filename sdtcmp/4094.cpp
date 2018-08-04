// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4094.hpp"
#define			XXX					0
#define			PIN					-1 + 

const std::string	&nts::CMP_4094::GetType(void) const
{
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

nts::Tristate		nts::CMP_4094::Compute(size_t		n)
{
  /// Check if it already computed (Maybe temporary...)
  nts::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);

  /// Create a new state base on the previous one
  Preset		p = PresetOutput(n);

  if ((n >= 4 && n <= 8) || (n >= 9 && n <= 14))
    {
      // TICK UP
      if (GetPin(3) == nts::TRUE && GetPrevious(3) == nts::FALSE && last_tick_up != timer.GetTime())
	{
	  last_tick_up = timer.GetTime();
	  value <<= 1;
	  if (GetPin(2) == nts::TRUE)
	    value.set(0);
	}
      // TICK DOWN
      if (GetPin(3) == nts::FALSE && GetPrevious(3) == nts::TRUE && last_tick_down != timer.GetTime())
	{
	  last_tick_down = timer.GetTime();
	  qns = value[7];
	}

      // STROBE
      if (GetPin(1) != nts::TRUE)
	return (p->second);

      if (n == 10)
	{
	  p->second = qns ? nts::TRUE : nts::FALSE;
	  return (p->second);
	}
      if (n == 9)
	{
	  p->second = value[7] ? nts::TRUE : nts::FALSE;
	  return (p->second);
	}

      // OE
      if (GetPin(15) != nts::TRUE)
	{
	  p->second = nts::UNDEFINED;
	  return (p->second);
	}

      p->second = value[pinstrenght[n - 1]] ? nts::TRUE : nts::FALSE;
      return (p->second);
    }
  return (nts::UNDEFINED);
}

nts::CMP_4094::CMP_4094(const nts::Timer		&time)
  : AComponent(time),
    last_tick_up(0),
    last_tick_down(0)
{}

nts::CMP_4094::~CMP_4094(void)
{}

