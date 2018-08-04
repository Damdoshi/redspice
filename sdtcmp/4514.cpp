// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4514.hpp"

const std::string	&nts::CMP_4514::GetType(void) const
{
  return (type = typeid(*this).name());
}


static const size_t	pinstrenght[24] =
  {
    0, 0, 0, 7, 6, 5, 4, 3, 1, 2, 0, 0,
    13, 12, 15, 14, 9, 8, 11, 10, 0, 0, 0, 0
  };

nts::Tristate		nts::CMP_4514::Compute(size_t		n)
{
  /// Check if it already computed (Maybe temporary...)
  nts::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);

  /// Create a new state base on the previous one
  Preset		p = PresetOutput(n);
  unsigned char		val;
  size_t		active;

  if ((n >= 4 && n <= 11) || (n >= 13 && n <= 20))
    {
      /// STROBE
      if (GetPin(1) == nts::FALSE)
	{
	  KeepOld(n);
	  if (GetPin(23) == nts::TRUE)
	    return (nts::FALSE);
	  return (GetPin(n));
	}

      /// INHIBIT
      if (GetPin(23) == nts::TRUE)
	{
	  p->second = nts::FALSE;
	  return (nts::FALSE);
	}

      val = 0;
      if (GetPin(2) == nts::TRUE)
	val |= 1 << 0;
      if (GetPin(3) == nts::TRUE)
	val |= 1 << 1;
      if (GetPin(21) == nts::TRUE)
       	val |= 1 << 2;
      if (GetPin(22) == nts::TRUE)
	val |= 1 << 3;
      if (val == 0)
	active = 10;
      else
	for (active = 0; pinstrenght[active] != val; ++active);
      
      if (active + 1 == p->first)
	p->second = nts::TRUE;
      else
	p->second = nts::FALSE;

      return (p->second);
    }
  else if (n == 12 || n == 24)
    return (nts::UNDEFINED);
  return (GetPin(n));
}

nts::CMP_4514::CMP_4514(const nts::Timer	&time)
  : AComponent(time)
{}

nts::CMP_4514::~CMP_4514(void)
{}

