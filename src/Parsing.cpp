// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		"Parsing.hpp"

bool			ReadWhitespace(const std::string		&code,
				       int				&i)
{
  int			j = i;

  while (code[i] == ' ' || code[i] == '\t' || code[i] == '\n')
    i = i + 1;
  return (j != i);
}

bool			ReadText(const std::string			&code,
				 int					&i,
				 const std::string			&token)
{
  int			j;

  for (j = 0; token[j]; ++j)
    if (code[i + j] != token[j])
      return (false);
  i += j;
  return (true);
}

bool			ReadChar(const std::string			&code,
				 int					&i)
{
  bool			ret;
  int			j;

  for (j = i; (code[j] >= 'a' && code[j] <= 'z') ||
	 (code[j] >= 'A' && code[j] <= 'Z') ||
	 (code[j] >= '0' && code[j] <= '9') ||
	 code[j] == '.' || code[j] == '/' ||
	 code[j] == '_' || code[j] == '-' || code[j] == '+';
       ++j);
  ret = (j != i);
  i = j;
  return (ret);
}

bool			CheckChar(const std::string			&code,
				  int					&i)
{
  int			j;

  for (j = i; (code[j] >= 'a' && code[j] <= 'z') ||
	 (code[j] >= 'A' && code[j] <= 'Z') ||
	 (code[j] >= '0' && code[j] <= '9') ||
	 code[j] == '.' || code[j] == '/' || code[j] == '_';
       ++j);
  return (j != i);
}
