// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4013.hpp"
#define			XXX					0
#define			PIN					-1 + 

const std::string	&hbs::CMP_4013::GetType(void) const
{
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::CMP_4013::Compute(size_t		n)
{
  /// Check if it already computed (Maybe temporary...)
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);

  /// Create a new state base on the previous one
  Preset		p = PresetOutput(n);

  /// FIRST D LATCH
  if (n == 1 || n == 2)
    {
      /// TICK ?
      if (last_tick[0] != timer.GetTime())
	if (GetPin(3) == hbs::TRUE && GetPrevious(3) == hbs::FALSE)
	  {
	    last_tick[0] = timer.GetTime();
	    val[0] = GetPin(5);
	  }

      /// SET 1
      if (GetPin(6) == hbs::TRUE)
	{
	  if (GetPin(4) == hbs::TRUE)
	    return (p->second = hbs::TRUE);
	  if (n == 1)
	    p->second = hbs::TRUE;
	  else
	    p->second = hbs::FALSE;
	  return (p->second);
	}

      /// RESET 1
      if (GetPin(4) == hbs::TRUE)
	{
	  if (n == 2)
	    p->second = hbs::TRUE;
	  else
	    p->second = hbs::FALSE;
	  return (p->second);
	}

      if (n == 1)
	p->second = val[0];
      else if (val[0] == hbs::UNDEFINED)
	p->second = hbs::UNDEFINED;
      else if (val[0] == hbs::TRUE)
	p->second = hbs::FALSE;
      else
	p->second = hbs::TRUE;
      return (p->second);
    }
  /// SECOND D LATCH
  else if (n == 13 || n == 12)
    {
      /// TICK ?
      if (last_tick[1] != timer.GetTime())
	if (GetPin(11) == hbs::TRUE && GetPrevious(11) == hbs::FALSE)
	  {
	    last_tick[1] = timer.GetTime();
	    val[1] = GetPin(5);
	  }

      /// SET 2
      if (GetPin(8) == hbs::TRUE)
	{
	  if (GetPin(10) == hbs::TRUE)
	    return (p->second = hbs::TRUE);
	  if (n == 13)
	    p->second = hbs::TRUE;
	  else
	    p->second = hbs::FALSE;
	  return (p->second);
	}

      /// RESET 2
      if (GetPin(10) == hbs::TRUE)
	{
	  if (n == 12)
	    p->second = hbs::TRUE;
	  else
	    p->second = hbs::FALSE;
	  return (p->second);
	}

      if (n == 13)
	p->second = val[1];
      else if (val[1] == hbs::UNDEFINED)
	p->second = hbs::UNDEFINED;
      else if (val[1] == hbs::TRUE)
	p->second = hbs::FALSE;
      else
	p->second = hbs::TRUE;
      return (p->second);
    }
  else if (n == 7 || n == 14)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4013::CMP_4013(const hbs::Timer		&time)
  : AComponent(time)
{
  val[0] = hbs::UNDEFINED;
  val[1] = hbs::UNDEFINED;
  last_tick[0] = 0;
  last_tick[1] = 0;
}

hbs::CMP_4013::~CMP_4013(void)
{}

