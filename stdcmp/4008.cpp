// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4008.hpp"

const std::string	&hbs::CMP_4008::GetType(void) const
{
  return (type = "4008");
  return (type = typeid(*this).name());
}

hbs::Tristate		hbs::CMP_4008::Compute(size_t		n)
{
  /// Check if it already computed (Maybe temporary...)
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);

  /// Create a new state base on the previous one
  Preset		p = PresetOutput(n);
  unsigned char		val0;
  unsigned char		val1;
  unsigned char		carry;

  if (n >= 10 && n <= 14)
    {
      val0 = 0;
      val1 = 0;
      carry = 0;
      if (GetPin(7) == hbs::TRUE)
	val0 |= 1 << 0;
      if (GetPin(5) == hbs::TRUE)
	val0 |= 1 << 1;
      if (GetPin(3) == hbs::TRUE)
	val0 |= 1 << 2;
      if (GetPin(1) == hbs::TRUE)
	val0 |= 1 << 3;

      if (GetPin(6) == hbs::TRUE)
	val1 |= 1 << 0;
      if (GetPin(4) == hbs::TRUE)
	val1 |= 1 << 1;
      if (GetPin(2) == hbs::TRUE)
	val1 |= 1 << 2;
      if (GetPin(15) == hbs::TRUE)
	val1 |= 1 << 3;

      if (GetPin(9) == hbs::TRUE)
	carry = 1;
      else
	carry = 0;

      if ((carry += val0 + val1) == 0)
	p->second = hbs::FALSE;
      else if ((carry >> (n - 10)) & 1)
	p->second = hbs::TRUE;
      else
	p->second = hbs::FALSE;

      return (p->second);
    }
  else if (n == 8 || n == 16)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_4008::CMP_4008(const hbs::Timer		&time,
			const std::string		&name,
			const std::string		&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_4008::~CMP_4008(void)
{}

