// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
// EFRITS SAS 2022-2026
//
// RED Spice

#include		<fstream>
#include		<sstream>
#include		"Circuit.hpp"

static bool		is_track(const hbs::IComponent *cmp)
{
  return (dynamic_cast<const hbs::Track*>(cmp) != NULL);
}

static std::string	file_type(const hbs::IComponent &cmp)
{
  const std::string	&type = cmp.GetType();

  if (type == "In")
    return ("input");
  if (type == "Out")
    return ("output");
  if (type == "Clock")
    return ("clock");
  if (type == "True")
    return ("true");
  if (type == "False")
    return ("false");
  if (type == "Terminal")
    return ("terminal");
  return (type);
}

hbs::Input		*hbs::Circuit::GetInput(const hbs::Screen		&screen,
						t_bunny_position		pos) const
{
  for (auto it = inputs.begin(); it != inputs.end(); ++it)
    if (it->second->IsUnder(screen, pos))
      return (it->second);
  return (NULL);
}

hbs::IComponent		*hbs::Circuit::GetComponent(const hbs::Screen		&screen,
						    t_bunny_position		pos) const
{
  for (auto it = circuit.begin(); it != circuit.end(); ++it)
    if (it->second->IsUnder(screen, pos))
      return (it->second);
  return (NULL);
}

hbs::Packet		hbs::Circuit::EndLinkStep(void) const
{
  hbs::Packet		ps;

  return (ps);
}

hbs::Packet		hbs::Circuit::GetLinkStep(const hbs::Screen		&screen,
						  t_bunny_position		pos) const
{
  for (auto it = tracks.begin(); it != tracks.end(); ++it)
    {
      hbs::Positions::iterator step = it->second->GetLinkStep(screen, pos);

      if (step != it->second->EndLinkStep())
	{
	  hbs::Packet packet;

	  packet.link = it->second;
	  packet.ori_pin = 0;
	  packet.pos = step;
	  return (packet);
	}
    }
  return (EndLinkStep());
}

void			hbs::Circuit::Move(const hbs::Position			&pos)
{
  (void)pos;
}

bool			hbs::Circuit::IsUnder(const hbs::Screen			&screen,
					      const t_bunny_position		&pos) const
{
  (void)screen;
  (void)pos;
  return (false);
}

hbs::Position		hbs::Circuit::GetPosition(void) const
{
  return (hbs::Position{0, 0});
}

hbs::Position		hbs::Circuit::GetPinPosition(size_t		pin) const
{
  (void)pin;
  return (hbs::Position{0, 0});
}

const std::string	&hbs::Circuit::GetType(void) const
{
  static std::string	type = "circuit";

  return (type);
}

const std::string	&hbs::Circuit::GetName(void) const
{
  static std::string	name = "circuit";

  return (name);
}

size_t			hbs::Circuit::GetPinCount(void) const
{
  return (0);
}

void			hbs::Circuit::Draw(hbs::Screen			&screen) const
{
  for (auto it = tracks.begin(); it != tracks.end(); ++it)
    if (it->second != screen.grabbed)
      it->second->Draw(screen);
  for (auto it = circuit.begin(); it != circuit.end(); ++it)
    if (!is_track(it->second) && it->second != screen.grabbed)
      it->second->Draw(screen);
}

bool			hbs::Circuit::ReadChipsetsInside(const std::string &code,
							 int		&i)
{
  std::string		type;
  std::string		name;
  std::string		value;
  std::string		position;
  int			j;

  j = i;
  while (code[j] != '.' && ReadChar(code, j))
    {
      value = "";
      position = "";
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
      if (code[j] == '[')
	{
	  i = ++j;
	  ReadWhitespace(code, j);
	  if (ReadChar(code, j))
	    {
	      if (code[j] != ',')
		throw SyntaxError(code.substr(i, 20));
	      else
		j += 1;
	      ReadWhitespace(code, j);
	      ReadChar(code, ++j);
	      position = code.substr(i, j - i);
	      ReadWhitespace(code, j);
	    }
	  ReadWhitespace(code, j);
	  if (code[j] != ']')
	    throw hbs::SyntaxError("Missing ']'.");
	  ReadWhitespace(code, j);
	  j += 1;
	}
      circuit[name] = Create(type, name, value, position);
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

hbs::Track		*hbs::Circuit::CreateTrack(const std::string		&name,
						 const std::string		&geometry)
{
  hbs::Track		*track;

  if (circuit.find(name) != circuit.end())
    throw hbs::BadComponent(std::string("Duplicated component ") + name);
  track = new hbs::Track(timer, name, geometry);
  circuit[name] = track;
  tracks[name] = track;
  return (track);
}

hbs::Track		*hbs::Circuit::CreateImplicitTrack(const std::string	&geometry)
{
  std::stringstream	ss;

  do
    ss.str(std::string()), ss.clear(), ss << "__wire_" << implicit_track_count++;
  while (circuit.find(ss.str()) != circuit.end());
  return (CreateTrack(ss.str(), geometry));
}

bool			hbs::Circuit::ReadTracksInside(const std::string &code,
						       int		&i)
{
  std::string		name;
  std::string		geometry;
  int			j;

  j = i;
  while (code[j] != '.' && ReadChar(code, j))
    {
      name = code.substr(i, j - i);
      ReadWhitespace(code, j);
      if (code[j] != '[')
	throw hbs::SyntaxError("Expected track geometry.");
      i = j;
      while (code[j] != ']' && code[j])
	j += 1;
      if (!code[j])
	throw hbs::SyntaxError("Missing ']'.");
      j += 1;
      geometry = code.substr(i, j - i);
      CreateTrack(name, geometry);
      ReadWhitespace(code, j);
      i = j;
    }
  return (true);
}

bool			hbs::Circuit::ReadTracks(const std::string	&code,
					       int			&i)
{
  if (ReadText(code, i, ".tracks:") == false)
    return (false);
  ReadWhitespace(code, i);
  return (ReadTracksInside(code, i));
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
      std::string	poslink;

      ReadWhitespace(code, i);
      ReadOneLink(code, i, lcom, lpin);
      ReadOneLink(code, i, rcom, rpin);
      ReadWhitespace(code, i);
      if (code[i] == '[')
	{
	  poslink = code.substr(i);
	  while (code[i] != ']' && code[i])
	    i = i + 1;
	  if (!code[i])
	    throw hbs::SyntaxError("Missing ']'");
	  i = i + 1;
	  poslink.resize(poslink.find(']') + 1);
	  ReadWhitespace(code, i);
	}
      else
	poslink = "";
      if (circuit[lcom] == NULL)
	throw hbs::UnknownComponent(lcom);
      if (circuit[rcom] == NULL)
	throw hbs::UnknownComponent(rcom);
      if (!is_track(circuit[lcom]) && !is_track(circuit[rcom]))
	{
	  hbs::Track *track = CreateImplicitTrack(poslink);

	  track->SetLink(1, *circuit[lcom], lpin, "");
	  track->SetLink(2, *circuit[rcom], rpin, "");
	  if (!track->GetPath().empty())
	    {
	      track->SetAttachmentNode(1, 0);
	      track->SetAttachmentNode(2, track->GetPath().size() - 1);
	    }
	}
      else
	circuit[lcom]->SetLink(lpin, *circuit[rcom], rpin, "");
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
  std::map<std::string, Output*>::iterator it;

  for (it = outputs.begin(); it != outputs.end() && output > 1; ++it, --output);
  return (it->second->Compute());
}

hbs::Tristate		hbs::Circuit::Compute(void)
{
  std::map<std::string, Output*>::iterator it;

  for (it = outputs.begin(); it != outputs.end(); ++it)
    it->second->Compute();
  return (hbs::UNDEFINED);
}

void			hbs::Circuit::SetLink(size_t			pnthis,
					      hbs::IComponent		&com,
					      size_t			pntarg,
					      const std::string		&pos)
{
  (void)pnthis; (void)com; (void)pntarg; (void)pos;
}

void			hbs::Circuit::Dump(void) const
{
  std::map<std::string, IComponent*>::const_iterator it;

  for (it = circuit.begin(); it != circuit.end(); ++it)
    {
      std::cerr << it->first << std::endl;
      it->second->Dump();
    }
}

void			hbs::Circuit::Map(void) const
{
  std::map<std::string, Input*>::const_iterator iti;
  std::map<std::string, Output*>::const_iterator ito;

  iti = inputs.begin();
  ito = outputs.begin();
  fprintf(stderr, "%21s[        ]\n", " ");
  while (iti != inputs.end() || ito != outputs.end())
    {
      char		c;

      if (iti != inputs.end())
	{
	  c << iti->second->Compute(1);
	  fprintf(stderr, "%16s(%c)->[        ]", iti->first.c_str(), c);
	  ++iti;
	}
      else
	fprintf(stderr, "%21s[        ]", " ");
      if (ito != outputs.end())
	{
	  if (ito->second->Displayable())
	    {
	      c << ito->second->Compute(1);
	      fprintf(stderr, "->%s(%c)", ito->first.c_str(), c);
	    }
	  else
	    fprintf(stderr, "->%s", ito->first.c_str());
	  ++ito;
	}
      fprintf(stderr, "\n");
    }
  fprintf(stderr, "%21s[        ]\n", " ");
}

const std::string	&hbs::Circuit::GetOutputName(size_t		n) const
{
  std::map<std::string, Output*>::const_iterator it;

  for (it = outputs.begin(); it != outputs.end() && n > 1; --n, ++it);
  if (it == outputs.end())
    throw hbs::MissingOutputs("Cannot found output.");
  return (it->first);
}

bool			hbs::Circuit::GetDisplayable(size_t		n) const
{
  std::map<std::string, Output*>::const_iterator it;

  for (it = outputs.begin(); it != outputs.end() && n > 1; --n, ++it);
  if (it == outputs.end())
    throw hbs::MissingOutputs("Cannot found output.");
  return (it->second->Displayable());
}

size_t			hbs::Circuit::GetOutputNum(void) const
{
  return (outputs.size());
}

bool			hbs::Circuit::Save(const std::string		&file) const
{
  std::ofstream		os((char*)file.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

  if (!os)
    return (false);
  os << ".chipsets:\n";
  for (auto it = circuit.begin(); it != circuit.end(); ++it)
    if (!is_track(it->second))
      {
	std::string value = it->second->GetDefinitionValue();
	hbs::Position pos = it->second->GetPosition();

	os << file_type(*it->second) << " " << it->first;
	if (value != "")
	  os << "(" << value << ")";
	os << " [" << pos.x << "," << pos.y << "]\n";
      }
  os << "\n.tracks:\n";
  for (auto it = tracks.begin(); it != tracks.end(); ++it)
    {
      const hbs::Positions &path = it->second->GetPath();

      os << it->first << " [";
      for (hbs::Positions::const_iterator p = path.begin(); p != path.end(); ++p)
	{
	  if (p != path.begin())
	    os << " ";
	  os << (p->second == hbs::ILink::TOP ? "t " : "b ") << p->first.x << "," << p->first.y;
	}
      os << "]\n";
    }
  os << "\n.links:\n";
  for (auto it = tracks.begin(); it != tracks.end(); ++it)
    {
      const std::vector<hbs::Track::Attachment> &attachments = it->second->GetAttachments();

      for (size_t i = 0; i < attachments.size(); ++i)
	if (attachments[i].component && !is_track(attachments[i].component))
	  os << attachments[i].component->GetName() << ":" << attachments[i].pin
	     << " " << it->first << ":" << (i + 1) << "\n";
    }
  return (true);
}

bool			hbs::Circuit::Load(const std::string		&file)
{
  std::ifstream		ss((char*)file.c_str(), std::ios::in | std::ios::binary);

  if (!ss)
    return (true);
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
  ReadWhitespace(content, i);
  ReadTracks(content, i);
  ReadWhitespace(content, i);
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

size_t			hbs::Circuit::GetTime(void) const
{
  return (timer.GetTime());
}

hbs::Circuit::Circuit(hbs::Timer		&tim)
  : timer(tim),
    implicit_track_count(0)
{}

hbs::Circuit::~Circuit(void)
{
  std::map<std::string, IComponent*>::iterator	it;

  for (it = circuit.begin(); it != circuit.end(); ++it)
    delete it->second;
}
