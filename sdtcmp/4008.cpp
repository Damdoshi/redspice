// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"4008.hpp"

const std::string	&nts::CMP_4008::GetType(void) const
{
  return (type = typeid(*this).name());
}

nts::Tristate		nts::CMP_4008::Compute(size_t		n)
{
  /// Check if it already computed (Maybe temporary...)
  nts::Tristate		tri;

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
      if (GetPin(7) == nts::TRUE)
	val0 |= 1 << 0;
      if (GetPin(5) == nts::TRUE)
	val0 |= 1 << 1;
      if (GetPin(3) == nts::TRUE)
	val0 |= 1 << 2;
      if (GetPin(1) == nts::TRUE)
	val0 |= 1 << 3;

      if (GetPin(6) == nts::TRUE)
	val1 |= 1 << 0;
      if (GetPin(4) == nts::TRUE)
	val1 |= 1 << 1;
      if (GetPin(2) == nts::TRUE)
	val1 |= 1 << 2;
      if (GetPin(15) == nts::TRUE)
	val1 |= 1 << 3;

      if (GetPin(9) == nts::TRUE)
	carry = 1;
      else
	carry = 0;

      if ((carry += val0 + val1) == 0)
	p->second = nts::FALSE;
      else if ((carry >> (n - 10)) & 1)
	p->second = nts::TRUE;
      else
	p->second = nts::FALSE;

      return (p->second);
    }
  else if (n == 8 || n == 16)
    return (nts::UNDEFINED);
  return (GetPin(n));
}

nts::CMP_4008::CMP_4008(const nts::Timer		&time)
  : AComponent(time)
{}

nts::CMP_4008::~CMP_4008(void)
{}

