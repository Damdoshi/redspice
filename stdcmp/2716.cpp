// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		<fstream>
#include		"2716.hpp"
#define			XXX					0
#define			PIN					-1 + 

const std::string	&hbs::CMP_2716::GetType(void) const
{
  return (type = "2716");
  return (type = typeid(*this).name());
}

static const size_t	addresspin[11] =
  {
    8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19
  };

static const size_t	pinstrenght[24] =
  {
    [PIN  1] = XXX,
    [PIN  2] = XXX,
    [PIN  3] = XXX,
    [PIN  4] = XXX,
    [PIN  5] = XXX,
    [PIN  6] = XXX,
    [PIN  7] = XXX,
    [PIN  8] = XXX,
    [PIN  9] = 0,
    [PIN 10] = 1,
    [PIN 11] = 2,
    [PIN 12] = XXX,
    [PIN 13] = 3,
    [PIN 14] = 4,
    [PIN 15] = 5,
    [PIN 16] = 6,
    [PIN 17] = 7,
    [PIN 18] = XXX,
    [PIN 19] = XXX,
    [PIN 20] = XXX,
    [PIN 21] = XXX,
    [PIN 22] = XXX,
    [PIN 23] = XXX,
    [PIN 24] = XXX
  };

hbs::Tristate		hbs::CMP_2716::Compute(size_t		n)
{
  /// Check if it already computed (Maybe temporary...)
  hbs::Tristate		tri;

  if (AlreadyComputed(n, tri))
    return (tri);

  /// Create a new state base on the previous one
  Preset		p = PresetOutput(n);
  uint32_t		address;
  uint8_t		value;
  size_t		i;

  if ((n >= 9 && n <= 11) || (n >= 13 && n <= 17))
    {
      /// /CE
      if (GetPin(18) != hbs::FALSE)
	return (hbs::UNDEFINED);

      /// /OE
      if (GetPin(20) != hbs::FALSE)
	return (hbs::UNDEFINED);

      address = 0;
      for (i = 0; i < sizeof(addresspin) / sizeof(addresspin[0]); ++i)
	address |= (GetPin(addresspin[i]) ? 1 : 0) << i;
      value = GetData(address);
      p->second = (value >> (pinstrenght[n - 1])) & 1 ? hbs::TRUE : hbs::FALSE;
      return (p->second);
    }
  return (hbs::UNDEFINED);
}

hbs::CMP_2716::CMP_2716(const hbs::Timer		&time,
			const std::string		&file,
			const std::string		&pos)
  : Memory(time, pos)
{
  std::ifstream		ss((char*)file.c_str(), std::ios::in | std::ios::binary);

  if (!ss)
    throw hbs::CannotOpenFile(file);
  std::string		content;
  size_t		i;

  ss.seekg(0,  std::ios::end);
  if (ss.tellg() > (int)GetMemorySize())
    throw hbs::FileIsTooBig(file);
  content.resize(ss.tellg());
  ss.seekg(0, std::ios::beg);
  ss.read(&content[0], content.size());
  for (i = 0; i < content.size(); ++i)
    SetData(i, content[i]);
  while (i < GetMemorySize())
    SetData(i++, 0);
}

hbs::CMP_2716::~CMP_2716(void)
{}

