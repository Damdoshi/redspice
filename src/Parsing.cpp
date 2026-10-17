// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"Parsing.hpp"
#include                <cctype>
#include                <charconv>
#include                <cmath>

bool			ReadWhitespace(const std::string		&code,
				       int				&i)
{
  int			j = i;

  while (i >= 0 && (size_t)i < code.size() && std::isspace((unsigned char)code[i]))
    i = i + 1;
  return (j != i);
}

bool			ReadText(const std::string			&code,
				 int					&i,
				 const std::string			&token)
{
  if (i < 0 || (size_t)i > code.size() || token.size() > code.size() - (size_t)i ||
      code.compare((size_t)i, token.size(), token) != 0)
    return (false);
  i += (int)token.size();
  return (true);
}

bool			ReadChar(const std::string			&code,
				 int					&i)
{
  bool			ret;
  int			j;

  if (i < 0 || (size_t)i > code.size())
    return (false);
  for (j = i; (size_t)j < code.size() && ((code[j] >= 'a' && code[j] <= 'z') ||
	 (code[j] >= 'A' && code[j] <= 'Z') ||
	 (code[j] >= '0' && code[j] <= '9') ||
	 code[j] == '.' || code[j] == '/' ||
	 code[j] == '_' || code[j] == '-' || code[j] == '+');
       ++j);
  ret = (j != i);
  i = j;
  return (ret);
}

bool			CheckChar(const std::string			&code,
				  int					&i)
{
  int j = i;
  return (ReadChar(code, j));
}

bool ReadNumber(const std::string &code, int &i, double &value)
{
  ReadWhitespace(code, i);
  if (i < 0 || (size_t)i >= code.size())
    return (false);
  const char *begin = code.data() + i;
  if (*begin == '+')
    ++begin;
  double parsed;
  auto result = std::from_chars(begin, code.data() + code.size(), parsed);
  // Bound world coordinates as well as rejecting NaN/infinity, before rendering.
  if (result.ec != std::errc() || result.ptr == begin ||
      !std::isfinite(parsed) || std::abs(parsed) > 1000000.0)
    return (false);
  value = parsed;
  i = (int)(result.ptr - code.data());
  return (true);
}

bool ReadUnsigned(const std::string &code, int &i, size_t &value)
{
  if (i < 0 || (size_t)i >= code.size())
    return (false);
  size_t parsed;
  auto result = std::from_chars(code.data() + i, code.data() + code.size(), parsed);
  if (result.ec != std::errc() || result.ptr == code.data() + i)
    return (false);
  value = parsed;
  i = (int)(result.ptr - code.data());
  return (true);
}

bool ValidIdentifier(const std::string &name)
{
  if (name.empty())
    return (false);
  for (unsigned char c : name)
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '+'))
      return (false);
  return (true);
}
