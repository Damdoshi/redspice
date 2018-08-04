// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4013.hpp"
#define			XXX					0
#define			PIN					-1 + 

const std::string	&nts::CMP_4013::GetType(void) const
{
  return (type = typeid(*this).name());
}

nts::Tristate		nts::CMP_4013::Compute(size_t		n)
{
  /// Check if it already computed (Maybe temporary...)
  nts::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);

  /// Create a new state base on the previous one
  Preset		p = PresetOutput(n);

  /// FIRST D LATCH
  if (n == 1 || n == 2)
    {
      /// TICK ?
      if (last_tick[0] != timer.GetTime())
	if (GetPin(3) == nts::TRUE && GetPrevious(3) == nts::FALSE)
	  {
	    last_tick[0] = timer.GetTime();
	    val[0] = GetPin(5);
	  }

      /// SET 1
      if (GetPin(6) == nts::TRUE)
	{
	  if (GetPin(4) == nts::TRUE)
	    return (p->second = nts::TRUE);
	  if (n == 1)
	    p->second = nts::TRUE;
	  else
	    p->second = nts::FALSE;
	  return (p->second);
	}

      /// RESET 1
      if (GetPin(4) == nts::TRUE)
	{
	  if (n == 2)
	    p->second = nts::TRUE;
	  else
	    p->second = nts::FALSE;
	  return (p->second);
	}

      if (n == 1)
	p->second = val[0];
      else if (val[0] == nts::UNDEFINED)
	p->second = nts::UNDEFINED;
      else if (val[0] == nts::TRUE)
	p->second = nts::FALSE;
      else
	p->second = nts::TRUE;
      return (p->second);
    }
  /// SECOND D LATCH
  else if (n == 13 || n == 12)
    {
      /// TICK ?
      if (last_tick[1] != timer.GetTime())
	if (GetPin(11) == nts::TRUE && GetPrevious(11) == nts::FALSE)
	  {
	    last_tick[1] = timer.GetTime();
	    val[1] = GetPin(5);
	  }

      /// SET 2
      if (GetPin(8) == nts::TRUE)
	{
	  if (GetPin(10) == nts::TRUE)
	    return (p->second = nts::TRUE);
	  if (n == 13)
	    p->second = nts::TRUE;
	  else
	    p->second = nts::FALSE;
	  return (p->second);
	}

      /// RESET 2
      if (GetPin(10) == nts::TRUE)
	{
	  if (n == 12)
	    p->second = nts::TRUE;
	  else
	    p->second = nts::FALSE;
	  return (p->second);
	}

      if (n == 13)
	p->second = val[1];
      else if (val[1] == nts::UNDEFINED)
	p->second = nts::UNDEFINED;
      else if (val[1] == nts::TRUE)
	p->second = nts::FALSE;
      else
	p->second = nts::TRUE;
      return (p->second);
    }
  else if (n == 7 || n == 14)
    return (nts::UNDEFINED);
  return (GetPin(n));
}

nts::CMP_4013::CMP_4013(const nts::Timer		&time)
  : AComponent(time)
{
  val[0] = nts::UNDEFINED;
  val[1] = nts::UNDEFINED;
  last_tick[0] = 0;
  last_tick[1] = 0;
}

nts::CMP_4013::~CMP_4013(void)
{}

