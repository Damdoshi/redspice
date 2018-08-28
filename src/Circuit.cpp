// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		<fstream>
#include		"Circuit.hpp"

static bool		ReadWhitespace(const std::string		&code,
				       int				&i)
{
  int			j = i;

  while (code[i] == ' ' || code[i] == '\t' || code[i] == '\n')
    i = i + 1;
  return (j != i);
}

static bool		ReadText(const std::string			&code,
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

static bool		ReadChar(const std::string			&code,
				 int					&i)
{
  bool			ret;
  int			j;

  for (j = i; (code[j] >= 'a' && code[j] <= 'z') ||
	 (code[j] >= 'A' && code[j] <= 'Z') ||
	 (code[j] >= '0' && code[j] <= '9') ||
	 code[j] == '.' || code[j] == '/' || code[j] == '_';
       ++j);
  ret = (j != i);
  i = j;
  return (ret);
}

static bool		CheckChar(const std::string			&code,
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

bool			hbs::Circuit::ReadChipsetsInside(const std::string &code,
							 int		&i)
{
  std::string		type;
  std::string		name;
  std::string		value;
  int			j;

  j = i;
  while (code[j] != '.' && ReadChar(code, j))
    {
      type = code.substr(i, j - i);
      ReadWhitespace(code, j);
      i = j;
      if (ReadChar(code, j) == false)
       throw hbs::SyntaxError("Expected component name after type.");
      name = code.substr(i, j - i);
      ReadWhitespace(code, j);
      if (code[j] == '(')
	{
	  i = ++j;
	  if (ReadChar(code, j))
	    value = code.substr(i, j - i);
	  ReadWhitespace(code, j);
	  if (code[j] != ')')
	    throw hbs::SyntaxError("Missing ')'.");
	  j += 1;
	}
      circuit[name] = Create(type, value);
      if (type == "input" || type == "clock")
	inputs[name] = dynamic_cast<hbs::Input*>(circuit[name]);
      else if (type == "output" || type == "terminal")
	outputs[name] = dynamic_cast<hbs::Output*>(circuit[name]);
      ReadWhitespace(code, j);
      i = j;
    }
  return (true);
}

bool			hbs::Circuit::ReadChipsets(const std::string	&code,
						   int			&i)
{
  if (ReadText(code, i, ".chipsets:") == false)
    return (false);
  ReadWhitespace(code, i);
  return (ReadChipsetsInside(code, i));
}

bool			hbs::Circuit::ReadOneLink(const std::string	&code,
						  int			&i,
						  std::string		&cmp,
						  int			&pin)
{
  int			j;

  j = i;
  ReadChar(code, j);
  cmp = code.substr(i, j - i);
  if (code[j] != ':')
    throw hbs::SyntaxError("Expected ':' and pin specification.");
  if (code[++j] < '0' || code[j] > '9')
    throw hbs::SyntaxError("Expected pin specification after component.");
  pin = std::atoi(&code.c_str()[j]);
  ReadChar(code, j);
  ReadWhitespace(code, j);
  i = j;
  return (true);
}

bool			hbs::Circuit::ReadLinksInside(const std::string	&code,
						      int		&i)
{
  std::string		lcom;
  int			lpin;
  std::string		rcom;
  int			rpin;

  while (CheckChar(code, i))
    {
      ReadOneLink(code, i, lcom, lpin);
      ReadOneLink(code, i, rcom, rpin);
      if (circuit[lcom] == NULL)
	throw hbs::UnknownComponent(lcom);
      circuit[lcom]->SetLink(lpin, *circuit[rcom], rpin);
    }
  return (true);
}

bool			hbs::Circuit::ReadLinks(const std::string	&code,
						int			&i)
{
  if (ReadText(code, i, ".links:") == false)
    return (false);
  ReadWhitespace(code, i);
  return (ReadLinksInside(code, i));
}

hbs::Tristate		hbs::Circuit::Compute(size_t			output)
{
  std::map<std::string, Output*>::iterator	it;

  for (it = outputs.begin(); it != outputs.end() && output > 1; ++it, --output);
  return (it->second->Compute());
}

hbs::Tristate		hbs::Circuit::Compute(void)
{
  std::map<std::string, Output*>::iterator	it;

  for (it = outputs.begin(); it != outputs.end(); ++it)
    it->second->Compute();
  return (hbs::UNDEFINED);
}

void			hbs::Circuit::SetLink(size_t			pnthis,
					      hbs::IComponent		&com,
					      size_t			pntarg)
{
  (void)pnthis; (void)com; (void)pntarg;
}

void			hbs::Circuit::Dump(void) const
{
  std::map<std::string, IComponent*>::const_iterator	it;

  for (it = circuit.begin(); it != circuit.end(); ++it)
    {
      std::cout << it->first << std::endl;
      it->second->Dump();
    }
}

const std::string	&hbs::Circuit::GetOutputName(size_t		n) const
{
  std::map<std::string, Output*>::const_iterator	it;

  for (it = outputs.begin(); it != outputs.end() && n > 1; --n, ++it);
  if (it == outputs.end())
    throw hbs::MissingOutputs("Cannot found output.");
  return (it->first);
}

bool			hbs::Circuit::GetDisplayable(size_t		n) const
{
  std::map<std::string, Output*>::const_iterator	it;

  for (it = outputs.begin(); it != outputs.end() && n > 1; --n, ++it);
  if (it == outputs.end())
    throw hbs::MissingOutputs("Cannot found output.");
  return (it->second->Displayable());
}

size_t			hbs::Circuit::GetOutputNum(void) const
{
  return (outputs.size());
}

bool			hbs::Circuit::Load(const std::string		&file)
{
  std::ifstream		ss((char*)file.c_str(), std::ios::in | std::ios::binary);

  if (!ss)
    return (false);
  std::string		content;
  bool			erase;
  int			i;

  i = 0;
  ss.seekg(0,  std::ios::end);
  content.resize(ss.tellg());
  ss.seekg(0, std::ios::beg);
  ss.read(&content[0], content.size());
  for (i = 0, erase = false; i < (int)content.size(); ++i)
    if (content[i] == '#')
      {
	erase = true;
	content[i] = ' ';
      }
    else if (content[i] == '\n')
      erase = false;
    else if (erase)
      content[i] = '\n';
  i = 0;
  ReadWhitespace(content, i);
  if (ReadChipsets(content, i) == false)
    return (false);
  return (ReadLinks(content, i));
}

void			hbs::Circuit::SetValue(const std::string		&input,
					       hbs::Tristate			value)
{
  std::map<std::string, Input*>::iterator	it;

  if ((it = inputs.find(input)) == inputs.end())
    throw hbs::MissingInputs("Cannot found input " + input + ".");
  it->second->SetValue(value);
}

hbs::Circuit::Circuit(hbs::Timer		&tim)
  : timer(tim)
{}

hbs::Circuit::~Circuit(void)
{
  std::map<std::string, IComponent*>::iterator	it;

  for (it = circuit.begin(); it != circuit.end(); ++it)
    delete it->second;
}

