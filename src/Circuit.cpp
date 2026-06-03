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
  return (NormalizeTrack(dst, dst_node));
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

static hbs::Position	parse_position(const std::string &pos)
{
  hbs::Position		position = {0, 0};
  size_t		idx = 0;
  int			idy;

  position.x = std::stof(pos, &idx);
  if (idx == 0)
    throw hbs::SyntaxError(pos);
  idy = idx;
  ReadWhitespace(pos, idy);
  idy += pos[idy] == ',';
  ReadWhitespace(pos, idy);
  idx = idy;
  position.y = std::stof(&pos[idx], &idx);
  if (idx == 0)
    throw hbs::SyntaxError(pos);
  return (position);
}

void			hbs::Circuit::ConfigureAsComponent(const std::string	&type,
					       const std::string	&name,
					       const std::string	&position)
{
  component_instance = true;
  component_type = type;
  component_name = name;
  component_position = parse_position(position);
  BuildExternalPins();
  if (!package_defined)
    Move(component_position);
}

void			hbs::Circuit::BuildExternalPins(void)
{
  external_pins.clear();
  if (package_defined)
    return ;
  for (std::map<std::string, Input*>::iterator it = inputs.begin(); it != inputs.end(); ++it)
    external_pins.push_back({it->first, CIRCUIT_INPUT_PIN, it->second, 1, it->second->GetPinPosition(1)});
  for (std::map<std::string, Output*>::iterator it = outputs.begin(); it != outputs.end(); ++it)
    external_pins.push_back({it->first, CIRCUIT_OUTPUT_PIN, it->second, 1, it->second->GetPinPosition(1)});
}

void			hbs::Circuit::Move(const hbs::Position			&pos)
{
  if (package_defined)
    {
      component_position.x += pos.x;
      component_position.y += pos.y;
      return ;
    }
  for (std::map<std::string, IComponent*>::iterator it = circuit.begin(); it != circuit.end(); ++it)
    it->second->Move(pos);
  component_position.x += pos.x;
  component_position.y += pos.y;
}

bool			hbs::Circuit::IsUnder(const hbs::Screen			&screen,
					      const t_bunny_position		&pos) const
{
  hbs::Position		p = {(double)pos.x, (double)pos.y};

  if (!component_instance)
    return (false);
  for (size_t i = 1; i <= external_pins.size(); ++i)
    {
      hbs::Position pp = GetPinPosition(i);
      double dist = sqrt(pow(p.x - pp.x, 2) + pow(p.y - pp.y, 2));

      if (dist < std::max(0.45, 8.0 / screen.pin_size))
	return (true);
    }
  if (!package_defined)
    {
      for (std::map<std::string, IComponent*>::const_iterator it = circuit.begin(); it != circuit.end(); ++it)
	if (it->second->IsUnder(screen, pos))
	  return (true);
      return (false);
    }
  if (package_lines.empty() && external_pins.empty())
    return (false);
  double left = GetPinPosition(1).x;
  double right = left;
  double top = GetPinPosition(1).y;
  double bottom = top;
  for (size_t i = 1; i <= external_pins.size(); ++i)
    {
      hbs::Position pp = GetPinPosition(i);
      left = std::min(left, pp.x);
      right = std::max(right, pp.x);
      top = std::min(top, pp.y);
      bottom = std::max(bottom, pp.y);
    }
  for (size_t i = 0; i < package_lines.size(); ++i)
    {
      left = std::min(left, component_position.x + std::min(package_lines[i].from.x, package_lines[i].to.x));
      right = std::max(right, component_position.x + std::max(package_lines[i].from.x, package_lines[i].to.x));
      top = std::min(top, component_position.y + std::min(package_lines[i].from.y, package_lines[i].to.y));
      bottom = std::max(bottom, component_position.y + std::max(package_lines[i].from.y, package_lines[i].to.y));
    }
  return (p.x >= left && p.x <= right && p.y >= top && p.y <= bottom);
}

hbs::Position		hbs::Circuit::GetPosition(void) const
{
  return (component_position);
}

hbs::Position		hbs::Circuit::GetPinPosition(size_t		pin) const
{
  if (pin == 0 || pin > external_pins.size())
    throw hbs::BadPin(GetType() + ": Bad pin.");
  if (package_defined)
    return (component_position + external_pins[pin - 1].position);
  return (external_pins[pin - 1].component->GetPinPosition(external_pins[pin - 1].component_pin));
}

const std::string	&hbs::Circuit::GetType(void) const
{
  static std::string	default_type = "circuit";

  if (component_instance)
    return (component_type);
  return (default_type);
}

const std::string	&hbs::Circuit::GetName(void) const
{
  static std::string	default_name = "circuit";

  if (component_instance)
    return (component_name);
  return (default_name);
}

void			hbs::Circuit::SetName(const std::string &name)
{
  component_name = name;
  component_instance = true;
}

void			hbs::Circuit::DisconnectFrom(const hbs::IComponent *component)
{
  for (std::map<size_t, std::list<CircuitConnection> >::iterator it = external_links.begin();
       it != external_links.end(); ++it)
    for (std::list<CircuitConnection>::iterator jt = it->second.begin(); jt != it->second.end();)
      if (jt->component == component)
	jt = it->second.erase(jt);
      else
	++jt;
  for (std::map<std::string, IComponent*>::iterator it = circuit.begin(); it != circuit.end(); ++it)
    if (it->second != component)
      it->second->DisconnectFrom(component);
}

size_t			hbs::Circuit::GetPinCount(void) const
{
  return (external_pins.size());
}

bool			hbs::Circuit::DrivesPin(size_t pin) const
{
  if (pin == 0 || pin > external_pins.size())
    return (false);
  return (external_pins[pin - 1].kind == CIRCUIT_OUTPUT_PIN);
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
  if (component_instance && package_defined)
    {
      for (size_t i = 0; i < package_lines.size(); ++i)
	screen.Line(component_position + package_lines[i].from,
		    component_position + package_lines[i].to,
		    hbs::Screen::Teal);
      for (size_t i = 1; i <= external_pins.size(); ++i)
	{
	  hbs::Position pp = GetPinPosition(i);
	  hbs::Size sz;

	  screen.Circle(pp, {0.4, 0.4}, hbs::Screen::Purple, true);
	  screen.Circle(pp, {0.25, 0.25}, hbs::Screen::White, false);
	  sz = screen.TextSize({10, 10}, external_pins[i - 1].name);
	  sz.x = (-sz.x / 2.0) / screen.PinSize();
	  sz.y = -1.0;
	  screen.Text(pp + sz, {10, 10}, hbs::Screen::White, external_pins[i - 1].name);
	}
      hbs::Size sz = screen.TextSize({10, 10}, GetName());

      sz.x = (-sz.x / 2.0) / screen.PinSize();
      sz.y = -1.5;
      screen.Text(component_position + sz, {10, 10}, hbs::Screen::White, GetName());
      return ;
    }
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


static void		collect_track_component(const std::vector<hbs::Track::Segment> &segments,
						 size_t node,
						 std::set<size_t> &nodes)
{
  bool			changed;

  nodes.insert(node);
  do
    {
      changed = false;
      for (size_t i = 0; i < segments.size(); ++i)
	{
	  if (nodes.find(segments[i].from) != nodes.end() &&
	      nodes.find(segments[i].to) == nodes.end())
	    {
	      nodes.insert(segments[i].to);
	      changed = true;
	    }
	  if (nodes.find(segments[i].to) != nodes.end() &&
	      nodes.find(segments[i].from) == nodes.end())
	    {
	      nodes.insert(segments[i].from);
	      changed = true;
	    }
	}
    }
  while (changed);
}

static std::vector<std::set<size_t> > track_components(const hbs::Track *track)
{
  std::vector<std::set<size_t> > components;
  std::set<size_t>		seen;
  size_t			nodes = track->GetPath().size();

  for (size_t i = 0; i < nodes; ++i)
    if (seen.find(i) == seen.end())
      {
	std::set<size_t> component;

	collect_track_component(track->GetSegments(), i, component);
	seen.insert(component.begin(), component.end());
	components.push_back(component);
      }
  return (components);
}

static size_t		component_containing(const std::vector<std::set<size_t> > &components,
					     size_t node)
{
  if (node == hbs::Track::NoNode)
    return (0);
  for (size_t i = 0; i < components.size(); ++i)
    if (components[i].find(node) != components[i].end())
      return (i);
  return (0);
}

hbs::Track		*hbs::Circuit::NormalizeTrack(hbs::Track *track,
					     size_t preferred_node)
{
  std::vector<std::set<size_t> > components;
  std::vector<hbs::Track*> new_tracks;
  std::string		old_name;
  size_t		preferred_component;

  if (track == NULL || track->GetPath().empty())
    return (track);
  components = track_components(track);
  if (components.size() <= 1)
    return (track);

  old_name = track->GetName();
  preferred_component = component_containing(components, preferred_node);
  const std::vector<hbs::Track::Segment> &segments = track->GetSegments();
  const std::vector<hbs::Track::Attachment> &attachments = track->GetAttachments();

  for (size_t c = 0; c < components.size(); ++c)
    {
      hbs::Track *out = CreateImplicitTrack("");
      std::vector<size_t> remap(track->GetPath().size(), hbs::Track::NoNode);

      for (size_t n = 0; n < track->GetPath().size(); ++n)
	if (components[c].find(n) != components[c].end())
	  remap[n] = out->AddFreeNode(track->GetNodePosition(n), track->GetNodeLayer(n));
      for (size_t i = 0; i < segments.size(); ++i)
	if (segments[i].from < remap.size() && segments[i].to < remap.size() &&
	    remap[segments[i].from] != hbs::Track::NoNode &&
	    remap[segments[i].to] != hbs::Track::NoNode)
	  out->AddSegment(remap[segments[i].from], remap[segments[i].to]);
      for (size_t i = 0; i < attachments.size(); ++i)
	{
	  bool keep = false;

	  if (attachments[i].component == NULL || attachments[i].pin == 0)
	    continue ;
	  if (attachments[i].node == hbs::Track::NoNode)
	    keep = (c == preferred_component);
	  else if (attachments[i].node < remap.size() &&
		   remap[attachments[i].node] != hbs::Track::NoNode)
	    keep = true;
	  if (keep)
	    {
	      size_t pin = out->GetPinCount() + 1;

	      out->SetLink(pin, *attachments[i].component, attachments[i].pin, "");
	      if (attachments[i].node != hbs::Track::NoNode)
		out->SetAttachmentNode(pin, remap[attachments[i].node]);
	      else
		out->SetAttachmentNode(pin, hbs::Track::NoNode);
	    }
	}
      new_tracks.push_back(out);
    }

  std::set<hbs::IComponent*> empty_components;
  std::set<hbs::Track*> old_track;
  old_track.insert(track);
  DeleteSelected(empty_components, old_track);
  if (preferred_component < new_tracks.size())
    {
      RenameComponent(new_tracks[preferred_component], old_name);
      return (new_tracks[preferred_component]);
    }
  return (NULL);
}

void			hbs::Circuit::NormalizeTracks(void)
{
  std::vector<std::string> names;

  for (std::map<std::string, hbs::Track*>::const_iterator it = tracks.begin(); it != tracks.end(); ++it)
    names.push_back(it->first);
  for (size_t i = 0; i < names.size(); ++i)
    {
      std::map<std::string, hbs::Track*>::iterator it = tracks.find(names[i]);

      if (it != tracks.end())
	NormalizeTrack(it->second, hbs::Track::NoNode);
    }
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
bool			hbs::Circuit::ReadPackageInside(const std::string &code,
							int		&i)
{
  std::string		word;

  package_defined = true;
  package_lines.clear();
  external_pins.clear();
  while (true)
    {
      skip_spaces(code, i);
      if (i >= (int)code.size() || code[i] == '.')
	break;
      word = read_token(code, i, "package directive");
      if (word == "line")
	{
	  std::string from = read_between(code, i, '[', ']', "package line start");
	  std::string to = read_between(code, i, '[', ']', "package line end");

	  package_lines.push_back({parse_position(from), parse_position(to)});
	}
      else
	{
	  std::string name;
	  std::string pos;
	  CircuitPin pin;

	  if (word == "pin")
	    name = read_token(code, i, "package pin name");
	  else
	    name = word;
	  pos = read_between(code, i, '[', ']', "package pin position");
	  if (inputs.find(name) != inputs.end())
	    pin = {name, CIRCUIT_INPUT_PIN, inputs[name], 1, parse_position(pos)};
	  else if (outputs.find(name) != outputs.end())
	    pin = {name, CIRCUIT_OUTPUT_PIN, outputs[name], 1, parse_position(pos)};
	  else
	    throw hbs::UnknownComponent(std::string("Unknown package pin ") + name);
	  external_pins.push_back(pin);
	}
    }
  return (true);
}

bool			hbs::Circuit::ReadPackage(const std::string	&code,
						 int			&i)
{
  if (ReadText(code, i, ".package:") == false)
    return (false);
  ReadWhitespace(code, i);
  return (ReadPackageInside(code, i));
}
void			hbs::Circuit::UpdateExternalInputs(void)
{
  for (size_t i = 0; i < external_pins.size(); ++i)
    if (external_pins[i].kind == CIRCUIT_INPUT_PIN)
      {
	hbs::Tristate value = hbs::UNDEFINED;
	std::map<size_t, std::list<CircuitConnection> >::iterator lnk = external_links.find(i + 1);

	if (lnk != external_links.end())
	  for (std::list<CircuitConnection>::iterator it = lnk->second.begin(); it != lnk->second.end(); ++it)
	    if (it->component != NULL)
	      {
		hbs::Tristate tmp = it->component->Compute(it->pin);

		if (tmp != hbs::UNDEFINED)
		  {
		    if (value != hbs::UNDEFINED)
		      {
			value = hbs::BROKEN;
			break;
		      }
		    value = tmp;
		  }
	      }
	if (external_pins[i].component != NULL)
	  {
	    hbs::Input *input = dynamic_cast<hbs::Input*>(external_pins[i].component);

	    if (input != NULL)
	      input->SetValue(value);
	  }
      }
}

hbs::Tristate		hbs::Circuit::Compute(size_t			output)
{
  std::map<std::string, Output*>::iterator it;

  if (component_instance)
    {
      if (output == 0 || output > external_pins.size())
	throw hbs::BadPin(GetType() + ": Bad pin.");
      UpdateExternalInputs();
      return (external_pins[output - 1].component->Compute(external_pins[output - 1].component_pin));
    }
  for (it = outputs.begin(); it != outputs.end() && output > 1; ++it, --output);
  return (it->second->Compute());
}

hbs::Tristate		hbs::Circuit::Compute(void)
{
  std::map<std::string, Output*>::iterator it;

  if (component_instance)
    {
      UpdateExternalInputs();
      for (size_t i = 1; i <= external_pins.size(); ++i)
	if (external_pins[i - 1].kind == CIRCUIT_OUTPUT_PIN)
	  external_pins[i - 1].component->Compute(external_pins[i - 1].component_pin);
      return (hbs::UNDEFINED);
    }
  for (it = outputs.begin(); it != outputs.end(); ++it)
    it->second->Compute();
  return (hbs::UNDEFINED);
}

void			hbs::Circuit::SetLink(size_t			pnthis,
					      hbs::IComponent		&com,
					      size_t			pntarg,
					      const std::string		&pos)
{
  std::list<CircuitConnection>::iterator it;

  if (pnthis == 0 || pnthis > external_pins.size())
    throw hbs::BadPin(GetType() + ": Bad pin.");
  for (it = external_links[pnthis].begin(); it != external_links[pnthis].end(); ++it)
    if (it->component == &com && it->pin == pntarg)
      return ;
  external_links[pnthis].push_back({&com, pntarg});
  if (pos.empty() || pos[0] != '!')
    com.SetLink(pntarg, *this, pnthis, "!" + pos);
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
  NormalizeTracks();
  ReadWhitespace(content, i);
  if (ReadPackage(content, i))
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

bool			hbs::Circuit::HasInput(const std::string		&name) const
{
  return (inputs.find(name) != inputs.end());
}

bool			hbs::Circuit::HasOutput(const std::string	&name) const
{
  return (outputs.find(name) != outputs.end());
}

bool			hbs::Circuit::HasComponent(const std::string	&name) const
{
  return (circuit.find(name) != circuit.end());
}

bool			hbs::Circuit::HasComponentType(const std::string	&type) const
{
  for (std::map<std::string, IComponent*>::const_iterator it = circuit.begin(); it != circuit.end(); ++it)
    if (file_type(*it->second) == type)
      return (true);
  return (false);
}

hbs::Tristate		hbs::Circuit::GetOutputValue(const std::string	&output)
{
  std::map<std::string, Output*>::iterator it;

  if ((it = outputs.find(output)) == outputs.end())
    throw hbs::MissingOutputs("Cannot found output " + output + ".");
  return (it->second->Compute());
}

std::vector<std::string>	hbs::Circuit::GetInputNames(void) const
{
  std::vector<std::string>	ret;

  for (std::map<std::string, Input*>::const_iterator it = inputs.begin(); it != inputs.end(); ++it)
    ret.push_back(it->first);
  return (ret);
}

std::vector<std::string>	hbs::Circuit::GetOutputNames(void) const
{
  std::vector<std::string>	ret;

  for (std::map<std::string, Output*>::const_iterator it = outputs.begin(); it != outputs.end(); ++it)
    ret.push_back(it->first);
  return (ret);
}

std::vector<std::string>	hbs::Circuit::GetComponentNames(void) const
{
  std::vector<std::string>	ret;

  for (std::map<std::string, IComponent*>::const_iterator it = circuit.begin(); it != circuit.end(); ++it)
    ret.push_back(it->first);
  return (ret);
}

std::string		hbs::Circuit::GetComponentType(const std::string	&component) const
{
  std::map<std::string, IComponent*>::const_iterator it;

  if ((it = circuit.find(component)) == circuit.end())
    throw hbs::UnknownComponent(component);
  return (file_type(*it->second));
}

std::map<std::string, size_t> hbs::Circuit::GetComponentTypeCount(void) const
{
  std::map<std::string, size_t>	ret;

  for (std::map<std::string, IComponent*>::const_iterator it = circuit.begin(); it != circuit.end(); ++it)
    ret[file_type(*it->second)] += 1;
  return (ret);
}

size_t			hbs::Circuit::GetComponentCount(bool			include_tracks) const
{
  size_t		count = 0;

  for (std::map<std::string, IComponent*>::const_iterator it = circuit.begin(); it != circuit.end(); ++it)
    if (include_tracks || !is_track(it->second))
      count += 1;
  return (count);
}

size_t			hbs::Circuit::GetLinkCount(void) const
{
  size_t		count = 0;

  for (std::map<std::string, hbs::Track*>::const_iterator it = tracks.begin(); it != tracks.end(); ++it)
    count += it->second->GetAttachments().size();
  return (count);
}

size_t			hbs::Circuit::GetTime(void) const
{
  return (timer.GetTime());
}

hbs::Circuit::Circuit(hbs::Timer		&tim)
  : timer(tim),
    component_type("circuit"),
    component_name("circuit"),
    component_position({0, 0}),
    component_instance(false),
    package_defined(false),
    implicit_track_count(0)
{
  const char *types[] = {
    "input", "output", "clock", "true", "false", "terminal",
    "4000", "4001", "4002", "4006", "4008", "4009", "4010", "4011",
    "4012", "4013", "4014", "4015", "4017", "4018", "4020", "4021", "4022", "4023", "4024", "4025",
    "4026", "4027", "4028", "4029", "4030", "4031", "4033", "4034", "4035", "4040", "4041", "4042", "4043", "4044", "4049",
    "4050", "4051", "4052", "4053", "4063", "4066", "4067", "4068", "4069", "4071", "4072", "4073",
    "4075", "4076", "4077", "4078", "4081", "4082", "4085",
    "4086", "4089", "4093", "4094", "4095", "4096", "4099", "4502", "4503", "4510", "4511",
    "4514", "4515", "4516", "4518", "4520", "4522", "4526", "4527", "4555", "4556", "4584",
    "74137", "74138", "74237", "74238", "74147", "74148", "74151", "74251", "74153", "74253", "74154", "74155", "74156",
    "74160", "74161", "74162", "74163", "74190", "74191", "74192", "74193",
    "74390", "74393",
    "74125", "74126", "74240", "74241", "74244", "74245",
    "74540", "74541", "74157", "74158", "74257", "74258", "74181", "74182", "74280", "74688",
    "74164", "74165", "74166", "74194", "74195", "74299", "74595", "74597",
    "7473", "7474", "7475", "7476", "7490", "7493", "74107", "74109", "74112",
    "74174", "74175", "74273", "74373", "74374", "74377", "74573", "74574",
    "2716", "wire"
  };

  creatable_types.assign(types, types + sizeof(types) / sizeof(types[0]));
  LoadExternalComponents();
}

hbs::Circuit::~Circuit(void)
{
  std::map<std::string, IComponent*>::iterator	it;

  for (it = circuit.begin(); it != circuit.end(); ++it)
    delete it->second;
}
