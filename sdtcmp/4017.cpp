// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4017.hpp"
#define			XXX					0
#define			PIN					-1 + 

const std::string	&nts::CMP_4017::GetType(void) const
{
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

nts::Tristate		nts::CMP_4017::Compute(size_t		n)
{
  /// Check if it already computed (Maybe temporary...)
  nts::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);

  /// Create a new state base on the previous one
  Preset		p = PresetOutput(n);
  bool			tick;

  if ((n >= 1 && n <= 7) || (n >= 9 && n <= 11))
    {
      if (GetPin(15) == nts::TRUE)
	{
	  value.reset();
	  value.set(0);
	  p->second = nts::FALSE;
	  return (p->second);
	}
      tick = false;
      if (GetPin(14) == nts::TRUE && GetPrevious(14) == nts::FALSE)
	tick = true;
      else if (GetPin(13) == nts::FALSE && GetPrevious(13) == nts::TRUE)
	tick = true;
      if (tick && last_tick != timer.GetTime())
	{
	  last_tick = timer.GetTime();
	  if (!(value <<= 1).any())
	    value.set(0);
	}
      if (value[pinstrenght[n - 1]])
	p->second = nts::TRUE;
      else
	p->second = nts::FALSE;
      return (p->second);
    }
  else if (n == 12)
    {
      if (GetPin(15) == nts::TRUE)
	p->second = nts::TRUE;
      else if ((value >> 5).any())
	p->second = nts::FALSE;
      else
	p->second = nts::TRUE;
      return (p->second);
    }
  else if (n == 8 || n == 16)
    return (nts::UNDEFINED);
  return (GetPin(n));
}

nts::CMP_4017::CMP_4017(const nts::Timer		&time)
  : AComponent(time),
    value(1),
    last_tick(0)
{}

nts::CMP_4017::~CMP_4017(void)
{}

