// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		"74181.hpp"
#include		"Logic.hpp"

const std::string	&hbs::CMP_74181::GetType(void) const
{
  return (type = "74181");
}

unsigned char		hbs::CMP_74181::ReadA(void)
{
  unsigned char		out = 0;

  if (GetPin(2) == hbs::TRUE) out |= 1 << 0;
  if (GetPin(23) == hbs::TRUE) out |= 1 << 1;
  if (GetPin(21) == hbs::TRUE) out |= 1 << 2;
  if (GetPin(19) == hbs::TRUE) out |= 1 << 3;
  return (out);
}

unsigned char		hbs::CMP_74181::ReadB(void)
{
  unsigned char		out = 0;

  if (GetPin(1) == hbs::TRUE) out |= 1 << 0;
  if (GetPin(22) == hbs::TRUE) out |= 1 << 1;
  if (GetPin(20) == hbs::TRUE) out |= 1 << 2;
  if (GetPin(18) == hbs::TRUE) out |= 1 << 3;
  return (out);
}

unsigned char		hbs::CMP_74181::ReadS(void)
{
  unsigned char		out = 0;

  if (GetPin(6) == hbs::TRUE) out |= 1 << 0;
  if (GetPin(5) == hbs::TRUE) out |= 1 << 1;
  if (GetPin(4) == hbs::TRUE) out |= 1 << 2;
  if (GetPin(3) == hbs::TRUE) out |= 1 << 3;
  return (out);
}

unsigned char		hbs::CMP_74181::Function(void)
{
  unsigned char		a = ReadA();
  unsigned char		b = ReadB();
  unsigned char		s = ReadS();
  unsigned int		res;

  if (GetPin(8) == hbs::TRUE)
    {
      switch (s & 15)
	{
	case 0: return (~a) & 15;
	case 1: return (~(a | b)) & 15;
	case 2: return ((~a) & b) & 15;
	case 3: return (0);
	case 4: return (~(a & b)) & 15;
	case 5: return (~b) & 15;
	case 6: return (a ^ b) & 15;
	case 7: return (a & (~b)) & 15;
	case 8: return ((~a) | b) & 15;
	case 9: return (~(a ^ b)) & 15;
	case 10: return (b) & 15;
	case 11: return (a & b) & 15;
	case 12: return (15);
	case 13: return (a | (~b)) & 15;
	case 14: return (a | b) & 15;
	default: return (a) & 15;
	}
    }
  switch (s & 3)
    {
    case 0: res = a + b; break;
    case 1: res = a + (b ^ 15); break;
    case 2: res = (a ^ 15) + b; break;
    default: res = a + 1; break;
    }
  if (GetPin(7) == hbs::TRUE)
    ++res;
  return (res & 15);
}

bool			hbs::CMP_74181::DrivesPin(size_t n) const
{
  return (n == 9 || n == 10 || n == 11 || n == 13 || n == 14 || n == 15 || n == 16 || n == 17);
}

hbs::Tristate		hbs::CMP_74181::Compute(size_t n)
{
  hbs::Tristate		tri;
  Preset		p;
  unsigned char		f;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      f = Function();
      if (n == 9)  return (p->second = (f & (1 << 0)) ? hbs::TRUE : hbs::FALSE);
      if (n == 10) return (p->second = (f & (1 << 1)) ? hbs::TRUE : hbs::FALSE);
      if (n == 11) return (p->second = (f & (1 << 2)) ? hbs::TRUE : hbs::FALSE);
      if (n == 13) return (p->second = (f & (1 << 3)) ? hbs::TRUE : hbs::FALSE);
      if (n == 14) return (p->second = (f == 0) ? hbs::TRUE : hbs::FALSE);
      if (n == 15) return (p->second = (ReadA() | ReadB()) == 15 ? hbs::TRUE : hbs::FALSE);
      if (n == 16) return (p->second = ((ReadA() + ReadB() + (GetPin(7) == hbs::TRUE ? 1 : 0)) & 16) ? hbs::TRUE : hbs::FALSE);
      if (n == 17) return (p->second = (ReadA() & ReadB()) != 0 ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 12 || n == 24)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_74181::CMP_74181(const hbs::Timer	&time,
				  const std::string	&name,
				  const std::string	&pos)
  : AComponent(time, name, pos)
{}

hbs::CMP_74181::~CMP_74181(void)
{}

bool			hbs::CMP_74181::TypeMatches(const std::string &type)
{
  if (type == "74181" || type == "74HC181" || type == "74HCT181" || type == "74LS181")
    return (true);
  return (false);
}

hbs::IComponent	*hbs::CMP_74181::Create(hbs::Timer		&timer,
				       const std::string	&type,
				       const std::string	&name,
				       const std::string	&value,
				       const std::string	&position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_74181(timer, name, position));
}

std::string		hbs::CMP_74181::GetDisplayType(void) const
{
  return ("74181 4-bit ALU / function generator");
}
