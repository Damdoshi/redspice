// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4514.hpp"

const std::string	&hbs::CMP_4514::GetType(void) const
{
  return (type = "4514");
  return (type = typeid(*this).name());
}


static const size_t	pinstrenght[24] =
  {
    0, 0, 0, 7, 6, 5, 4, 3, 1, 2, 0, 0,
    13, 12, 15, 14, 9, 8, 11, 10, 0, 0, 0, 0
  };

hbs::Tristate		hbs::CMP_4514::Compute(size_t		n)
{
  /// Check if it already computed (Maybe temporary...)
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);

  /// Create a new state base on the previous one
  Preset		p = PresetOutput(n);
  unsigned char		val;
  size_t		active;

  if ((n >= 4 && n <= 11) || (n >= 13 && n <= 20))
    {
      /// STROBE
      if (GetPin(1) == hbs::FALSE)
	{
	  KeepOld(n);
	  if (GetPin(23) == hbs::TRUE)
	    return (hbs::FALSE);
	  return (GetPin(n));
	}

      /// INHIBIT
      if (GetPin(23) == hbs::TRUE)
	{
	  p->second = hbs::FALSE;
	  return (hbs::FALSE);
	}

      val = 0;
      if (GetPin(2) == hbs::TRUE)
	val |= 1 << 0;
      if (GetPin(3) == hbs::TRUE)
	val |= 1 << 1;
      if (GetPin(21) == hbs::TRUE)
       	val |= 1 << 2;
      if (GetPin(22) == hbs::TRUE)
	val |= 1 << 3;
      if (val == 0)
	active = 10;
      else
	for (active = 0; pinstrenght[active] != val; ++active);
      
      if (active + 1 == p->first)
	p->second = hbs::TRUE;
      else
	p->second = hbs::FALSE;

      return (p->second);
    }
  else if (n == 12 || n == 24)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4514::CMP_4514(const hbs::Timer	&time,
			const std::string	&name,
			const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_4514::~CMP_4514(void)
{}

