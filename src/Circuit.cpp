// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
// EFRITS SAS 2022-2026
//
// RED Spice

#include		<algorithm>
#include		<cmath>
#include		<fstream>
#include		<cctype>
#include		<sstream>
#include		<iostream>
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
    if (!is_track(it->second) && it->second->IsUnder(screen, pos))
      return (it->second);
  return (NULL);
}

hbs::Packet		hbs::Circuit::EndLinkStep(void) const
{
  return (hbs::Packet());
}

hbs::Track		*hbs::Circuit::GetTrackAt(const hbs::Screen		&screen,
						     t_bunny_position		pos) const
{
  for (auto it = tracks.begin(); it != tracks.end(); ++it)
    if (it->second->IsUnder(screen, pos))
      return (it->second);
  return (NULL);
}

static bool		inside_rect(const hbs::Position &p, const hbs::Position &a, const hbs::Position &b)
{
  double		left = std::min(a.x, b.x);
  double		right = std::max(a.x, b.x);
  double		top = std::min(a.y, b.y);
  double		bottom = std::max(a.y, b.y);

  return (p.x >= left && p.x <= right && p.y >= top && p.y <= bottom);
}

void			hbs::Circuit::GetSelectionInRect(const hbs::Position	&from,
						   const hbs::Position	&to,
						   std::set<hbs::IComponent*> &components,
						   std::set<hbs::Track*> &selected_tracks) const
{
  components.clear();
  selected_tracks.clear();
  for (auto it = circuit.begin(); it != circuit.end(); ++it)
    if (!is_track(it->second))
      {
	bool selected = inside_rect(it->second->GetPosition(), from, to);

	for (size_t pin = 1; !selected && pin <= it->second->GetPinCount(); ++pin)
	  selected = inside_rect(it->second->GetPinPosition(pin), from, to);
	if (selected)
	  components.insert(it->second);
      }
  for (auto it = tracks.begin(); it != tracks.end(); ++it)
    {
      const hbs::Positions &path = it->second->GetPath();
      bool selected = false;

      for (hbs::Positions::const_iterator node = path.begin(); !selected && node != path.end(); ++node)
	selected = inside_rect(node->first, from, to);
      if (!selected)
	for (size_t pin = 1; !selected && pin <= it->second->GetPinCount(); ++pin)
	  selected = inside_rect(it->second->GetPinPosition(pin), from, to);
      if (selected)
	selected_tracks.insert(it->second);
    }
}

hbs::Packet		hbs::Circuit::GetLinkStep(const hbs::Screen		&screen,
						  t_bunny_position		pos) const
{
  for (auto it = tracks.begin(); it != tracks.end(); ++it)
    {
      hbs::Packet step = it->second->GetLinkStep(screen, pos);

      if (step != it->second->EndLinkStep())
	return (step);
    }
  return (EndLinkStep());
}

hbs::ComponentPin	hbs::Circuit::GetPinAt(const hbs::Screen		&screen,
						 t_bunny_position		pos) const
{
  hbs::Position		p = {(double)pos.x, (double)pos.y};

  for (auto it = circuit.begin(); it != circuit.end(); ++it)
    if (!is_track(it->second))
      for (size_t pin = 1; pin <= it->second->GetPinCount(); ++pin)
	{
	  hbs::Position pp = it->second->GetPinPosition(pin);
	  double dist = sqrt(pow(p.x - pp.x, 2) + pow(p.y - pp.y, 2));

	  if (dist < std::max(0.45, 8.0 / screen.pin_size))
	    return (hbs::ComponentPin(it->second, pin));
	}
  return (hbs::ComponentPin());
}

hbs::Track		*hbs::Circuit::CreateUserTrackAt(const hbs::Position	&pos)
{
  hbs::Track		*track = CreateImplicitTrack("");

  track->AddNode(pos, hbs::ILink::BOTTOM);
  return (track);
}



hbs::Track		*hbs::Circuit::MergeTracks(hbs::Track *dst,
					      hbs::Track *src,
					      size_t dst_node,
					      size_t src_node)
{
  std::vector<size_t>	nodemap;
  std::set<hbs::IComponent*> empty_components;
  std::set<hbs::Track*> victims;

  if (dst == NULL || src == NULL || dst == src)
    return (dst);
  const hbs::Positions &spath = src->GetPath();
  nodemap.reserve(spath.size());
  for (hbs::Positions::const_iterator it = spath.begin(); it != spath.end(); ++it)
    nodemap.push_back(dst->AddFreeNode(it->first, it->second));
  const std::vector<hbs::Track::Segment> &segments = src->GetSegments();
  for (size_t i = 0; i < segments.size(); ++i)
    if (segments[i].from < nodemap.size() && segments[i].to < nodemap.size())
      dst->AddSegment(nodemap[segments[i].from], nodemap[segments[i].to]);
  const std::vector<hbs::Track::Attachment> &attachments = src->GetAttachments();
  for (size_t i = 0; i < attachments.size(); ++i)
    if (attachments[i].component != NULL && attachments[i].pin != 0)
      {
	size_t pin = dst->GetPinCount() + 1;
	dst->SetLink(pin, *attachments[i].component, attachments[i].pin, "");
	if (attachments[i].node != hbs::Track::NoNode && attachments[i].node < nodemap.size())
	  dst->SetAttachmentNode(pin, nodemap[attachments[i].node]);
      }
  if (dst_node != hbs::Track::NoNode && src_node != hbs::Track::NoNode && src_node < nodemap.size())
    dst->AddSegment(dst_node, nodemap[src_node]);
  victims.insert(src);
  DeleteSelected(empty_components, victims);
  return (dst);
}


hbs::Track		*hbs::Circuit::FindTrackAttachedTo(hbs::IComponent *component,
						      size_t pin,
						      size_t *node) const
{
  for (std::map<std::string, hbs::Track*>::const_iterator it = tracks.begin(); it != tracks.end(); ++it)
    {
      const std::vector<hbs::Track::Attachment> &attachments = it->second->GetAttachments();

      for (size_t i = 0; i < attachments.size(); ++i)
	if (attachments[i].component == component && attachments[i].pin == pin)
	  {
	    if (node != NULL)
	      *node = attachments[i].node;
	    return (it->second);
	  }
    }
  if (node != NULL)
    *node = hbs::Track::NoNode;
  return (NULL);
}

const std::vector<std::string> &hbs::Circuit::GetCreatableTypes(void) const
{
  return (creatable_types);
}

static std::string	make_component_name(const std::string &type, size_t idx)
{
  std::stringstream	ss;

  ss << type << "_" << idx;
  return (ss.str());
}

hbs::IComponent		*hbs::Circuit::CreateUserComponent(const std::string &type,
						     const hbs::Position &pos)
{
  std::stringstream	ss;
  std::string		name;
  size_t		idx = 1;

  do
    name = make_component_name(type, idx++);
  while (circuit.find(name) != circuit.end());
  ss << pos.x << "," << pos.y;
  circuit[name] = Create(type, name, "", ss.str());
  if (type == "input" || type == "clock")
    inputs[name] = dynamic_cast<hbs::Input*>(circuit[name]);
  else if (type == "output" || type == "terminal")
    outputs[name] = dynamic_cast<hbs::Output*>(circuit[name]);
  else if (type == "wire" || type == "track")
    tracks[name] = dynamic_cast<hbs::Track*>(circuit[name]);
  return (circuit[name]);
}

bool			hbs::Circuit::RenameComponent(hbs::IComponent *component,
						 const std::string &name)
{
  std::string		old;

  if (component == NULL || name.empty() || circuit.find(name) != circuit.end())
    return (false);
  old = component->GetName();
  if (circuit.find(old) == circuit.end())
    return (false);
  circuit.erase(old);
  circuit[name] = component;
  if (inputs.find(old) != inputs.end())
    {
      inputs[name] = inputs[old];
      inputs.erase(old);
    }
  if (outputs.find(old) != outputs.end())
    {
      outputs[name] = outputs[old];
      outputs.erase(old);
    }
  if (tracks.find(old) != tracks.end())
    {
      tracks[name] = tracks[old];
      tracks.erase(old);
    }
  component->SetName(name);
  return (true);
}

bool			hbs::Circuit::DeleteSelected(std::set<hbs::IComponent*> &components,
						std::set<hbs::Track*> &selected_tracks)
{
  std::set<hbs::IComponent*> victims;

  for (std::set<hbs::IComponent*>::iterator it = components.begin(); it != components.end(); ++it)
    victims.insert(*it);
  for (std::set<hbs::Track*>::iterator it = selected_tracks.begin(); it != selected_tracks.end(); ++it)
    victims.insert(*it);
  if (victims.empty())
    return (false);
  for (std::set<hbs::IComponent*>::iterator v = victims.begin(); v != victims.end(); ++v)
    for (std::map<std::string, IComponent*>::iterator it = circuit.begin(); it != circuit.end(); ++it)
      if (victims.find(it->second) == victims.end())
	it->second->DisconnectFrom(*v);
  for (std::set<hbs::IComponent*>::iterator v = victims.begin(); v != victims.end(); ++v)
    {
      std::string name = (*v)->GetName();

      inputs.erase(name);
      outputs.erase(name);
      tracks.erase(name);
      circuit.erase(name);
      delete *v;
    }
  components.clear();
  selected_tracks.clear();
  return (true);
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

void			hbs::Circuit::SetName(const std::string &name)
{
  (void)name;
}

void			hbs::Circuit::DisconnectFrom(const hbs::IComponent *component)
{
  for (std::map<std::string, IComponent*>::iterator it = circuit.begin(); it != circuit.end(); ++it)
    if (it->second != component)
      it->second->DisconnectFrom(component);
}

size_t			hbs::Circuit::GetPinCount(void) const
{
  return (0);
}

static double		orientation(const hbs::Position &a, const hbs::Position &b, const hbs::Position &c)
{
  return ((b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y));
}

static bool		on_segment(const hbs::Position &a, const hbs::Position &b, const hbs::Position &c)
{
  return (b.x <= std::max(a.x, c.x) + 0.0001 && b.x + 0.0001 >= std::min(a.x, c.x) &&
	  b.y <= std::max(a.y, c.y) + 0.0001 && b.y + 0.0001 >= std::min(a.y, c.y));
}

static bool		segments_touch(const hbs::Position &a, const hbs::Position &b,
			       const hbs::Position &c, const hbs::Position &d)
{
  double		o1 = orientation(a, b, c);
  double		o2 = orientation(a, b, d);
  double		o3 = orientation(c, d, a);
  double		o4 = orientation(c, d, b);

  if (((o1 > 0 && o2 < 0) || (o1 < 0 && o2 > 0)) &&
      ((o3 > 0 && o4 < 0) || (o3 < 0 && o4 > 0)))
    return (true);
  if (fabs(o1) < 0.0001 && on_segment(a, c, b))
    return (true);
  if (fabs(o2) < 0.0001 && on_segment(a, d, b))
    return (true);
  if (fabs(o3) < 0.0001 && on_segment(c, a, d))
    return (true);
  if (fabs(o4) < 0.0001 && on_segment(c, b, d))
    return (true);
  return (false);
}

void			hbs::Circuit::ValidateRouting(void) const
{
  for (auto it = tracks.begin(); it != tracks.end(); ++it)
    it->second->SetRoutingError(false);
  for (auto a = tracks.begin(); a != tracks.end(); ++a)
    {
      auto b = a;

      for (++b; b != tracks.end(); ++b)
	{
	  const std::vector<hbs::Track::Segment> &aseg = a->second->GetSegments();
	  const std::vector<hbs::Track::Segment> &bseg = b->second->GetSegments();

	  for (size_t i = 0; i < aseg.size(); ++i)
	    for (size_t j = 0; j < bseg.size(); ++j)
	      {
		hbs::Position aa = a->second->GetNodePosition(aseg[i].from);
		hbs::Position ab = a->second->GetNodePosition(aseg[i].to);
		hbs::Position ba = b->second->GetNodePosition(bseg[j].from);
		hbs::Position bb = b->second->GetNodePosition(bseg[j].to);

		if (a->second->GetNodeLayer(aseg[i].from) == b->second->GetNodeLayer(bseg[j].from) &&
		    segments_touch(aa, ab, ba, bb))
		  {
		    a->second->SetRoutingError(true);
		    b->second->SetRoutingError(true);
		  }
	      }
	}
    }
}

void			hbs::Circuit::Draw(hbs::Screen			&screen) const
{
  ValidateRouting();
  for (auto it = tracks.begin(); it != tracks.end(); ++it)
    if (!screen.dragging_selection || screen.selected_tracks.find(it->second) == screen.selected_tracks.end())
      it->second->Draw(screen);
  for (auto it = circuit.begin(); it != circuit.end(); ++it)
    if (!is_track(it->second) &&
	(!screen.dragging_selection || screen.selected_components.find(it->second) == screen.selected_components.end()) &&
	it->second != screen.grabbed)
      it->second->Draw(screen);
}

static void		skip_spaces(const std::string &code, int &i)
{
  while (i < (int)code.size() && std::isspace((unsigned char)code[i]))
    i += 1;
}

static std::string	read_token(const std::string &code, int &i, const std::string &what)
{
  int			start;

  skip_spaces(code, i);
  start = i;
  while (i < (int)code.size() && !std::isspace((unsigned char)code[i]) &&
	 code[i] != '(' && code[i] != '[' && code[i] != '.')
    i += 1;
  if (start == i)
    throw hbs::SyntaxError(std::string("Expected ") + what + ".");
  return (code.substr(start, i - start));
}

static std::string	read_between(const std::string &code, int &i, char open, char close, const std::string &what)
{
  int			start;

  skip_spaces(code, i);
  if (i >= (int)code.size() || code[i] != open)
    return ("");
  i += 1;
  start = i;
  while (i < (int)code.size() && code[i] != close)
    i += 1;
  if (i >= (int)code.size())
    throw hbs::SyntaxError(std::string("Missing '") + close + "' while reading " + what + ".");
  std::string ret = code.substr(start, i - start);
  i += 1;
  return (ret);
}

bool			hbs::Circuit::ReadChipsetsInside(const std::string &code,
							 int		&i)
{
  std::string		type;
  std::string		name;
  std::string		value;
  std::string		position;

  while (true)
    {
      skip_spaces(code, i);
      if (i >= (int)code.size() || code[i] == '.')
	break;
      type = read_token(code, i, "component type");
      name = read_token(code, i, "component name");
      value = read_between(code, i, '(', ')', "component value");
      position = read_between(code, i, '[', ']', "component position");
      if (circuit.find(name) != circuit.end())
	throw hbs::BadComponent(std::string("Duplicated component ") + name);
      circuit[name] = Create(type, name, value, position);
      if (type == "input" || type == "clock")
	inputs[name] = dynamic_cast<hbs::Input*>(circuit[name]);
      else if (type == "output" || type == "terminal")
	outputs[name] = dynamic_cast<hbs::Output*>(circuit[name]);
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
      ReadWhitespace(code, j);
      if (code[j] == '{')
	{
	  while (code[j] != '}' && code[j])
	    j += 1;
	  if (!code[j])
	    throw hbs::SyntaxError("Missing '}'.");
	  j += 1;
	}
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
						  LinkEnd		&end)
{
  int			j;

  end.node = hbs::Track::NoNode;
  j = i;
  ReadChar(code, j);
  end.cmp = code.substr(i, j - i);
  if (code[j] != ':')
    throw hbs::SyntaxError("Expected ':' and pin specification.");
  if (code[++j] < '0' || code[j] > '9')
    throw hbs::SyntaxError("Expected pin specification after component.");
  end.pin = std::atoi(&code.c_str()[j]);
  ReadChar(code, j);
  if (code[j] == '@')
    {
      int k = ++j;

      if (code[j] < '0' || code[j] > '9')
	throw hbs::SyntaxError("Expected node specification after '@'.");
      end.node = (size_t)std::atoi(&code.c_str()[j]);
      if (end.node == 0)
	throw hbs::SyntaxError("Track node numbers start at 1.");
      end.node -= 1;
      ReadChar(code, j);
      (void)k;
    }
  ReadWhitespace(code, j);
  i = j;
  return (true);
}

bool			hbs::Circuit::ReadLinksInside(const std::string	&code,
						      int		&i)
{
  LinkEnd		left;
  LinkEnd		right;

  while (CheckChar(code, i))
    {
      std::string	poslink;

      ReadWhitespace(code, i);
      ReadOneLink(code, i, left);
      ReadOneLink(code, i, right);
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
      if (circuit[left.cmp] == NULL)
	throw hbs::UnknownComponent(left.cmp);
      if (circuit[right.cmp] == NULL)
	throw hbs::UnknownComponent(right.cmp);
      if (!is_track(circuit[left.cmp]) && !is_track(circuit[right.cmp]))
	{
	  hbs::Track *track = CreateImplicitTrack(poslink);

	  track->SetLink(1, *circuit[left.cmp], left.pin, "");
	  track->SetLink(2, *circuit[right.cmp], right.pin, "");
	  if (!track->GetPath().empty())
	    {
	      track->SetAttachmentNode(1, 0);
	      track->SetAttachmentNode(2, track->GetPath().size() - 1);
	    }
	}
      else
	{
	  circuit[left.cmp]->SetLink(left.pin, *circuit[right.cmp], right.pin, "");
	  if (is_track(circuit[left.cmp]) && left.node != hbs::Track::NoNode)
	    dynamic_cast<hbs::Track*>(circuit[left.cmp])->SetAttachmentNode(left.pin, left.node);
	  if (is_track(circuit[right.cmp]) && right.node != hbs::Track::NoNode)
	    dynamic_cast<hbs::Track*>(circuit[right.cmp])->SetAttachmentNode(right.pin, right.node);
	}
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
      const std::vector<hbs::Track::Segment> &segments = it->second->GetSegments();

      os << it->first << " [";
      for (hbs::Positions::const_iterator p = path.begin(); p != path.end(); ++p)
	{
	  if (p != path.begin())
	    os << " ";
	  os << (p->second == hbs::ILink::TOP ? "t " : "b ") << p->first.x << "," << p->first.y;
	}
      os << "]";
      if (!segments.empty())
	{
	  os << " {";
	  for (size_t i = 0; i < segments.size(); ++i)
	    {
	      if (i != 0)
		os << " ";
	      os << (segments[i].from + 1) << "-" << (segments[i].to + 1);
	    }
	  os << "}";
	}
      os << "\n";
    }
  os << "\n.links:\n";
  for (auto it = tracks.begin(); it != tracks.end(); ++it)
    {
      const std::vector<hbs::Track::Attachment> &attachments = it->second->GetAttachments();

      for (size_t i = 0; i < attachments.size(); ++i)
	if (attachments[i].component && !is_track(attachments[i].component))
	  {
	    os << attachments[i].component->GetName() << ":" << attachments[i].pin
	       << " " << it->first << ":" << (i + 1);
	    if (attachments[i].node != hbs::Track::NoNode)
	      os << "@" << (attachments[i].node + 1);
	    os << "\n";
	  }
    }
  return (true);
}

static std::string	load_context(const std::string &content, int pos)
{
  int		line = 1;
  int		col = 1;
  int		start;
  int		end;
  std::stringstream ss;

  if (pos < 0)
    pos = 0;
  if (pos > (int)content.size())
    pos = content.size();
  for (int i = 0; i < pos; ++i)
    if (content[i] == '\n')
      line += 1, col = 1;
    else
      col += 1;
  for (start = pos; start > 0 && content[start - 1] != '\n'; --start);
  for (end = pos; end < (int)content.size() && content[end] != '\n'; ++end);
  ss << "line " << line << ", column " << col << ": "
     << content.substr(start, end - start);
  return (ss.str());
}

bool			hbs::Circuit::Load(const std::string		&file)
{
  std::ifstream		ss((char*)file.c_str(), std::ios::in | std::ios::binary);

  if (!ss)
    throw hbs::CannotOpenFile(std::string("Cannot open circuit file '") + file + "'.");
  std::string		content;
  bool			erase;
  int			i;

  i = 0;
  ss.seekg(0,  std::ios::end);
  content.resize(ss.tellg());
  ss.seekg(0, std::ios::beg);
  if (!content.empty())
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
      content[i] = ' ';
  i = 0;
  ReadWhitespace(content, i);
  if (ReadChipsets(content, i) == false)
    throw hbs::SyntaxError(std::string("Expected .chipsets: section at ") + load_context(content, i));
  ReadWhitespace(content, i);
  if (ReadTracks(content, i))
    ReadWhitespace(content, i);
  if (ReadLinks(content, i) == false)
    throw hbs::SyntaxError(std::string("Expected .links: section at ") + load_context(content, i));
  ReadWhitespace(content, i);
  if (i < (int)content.size())
    throw hbs::SyntaxError(std::string("Unexpected content after .links: at ") + load_context(content, i));
  return (true);
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
{
  const char *types[] = {
    "input", "output", "clock", "true", "false", "terminal",
    "4001", "4008", "4011", "4013", "4017", "4030", "4040",
    "4069", "4071", "4081", "4094", "4514", "2716", "wire"
  };

  creatable_types.assign(types, types + sizeof(types) / sizeof(types[0]));
}

hbs::Circuit::~Circuit(void)
{
  std::map<std::string, IComponent*>::iterator	it;

  for (it = circuit.begin(); it != circuit.end(); ++it)
    delete it->second;
}
