// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		<algorithm>
#include		<cmath>
#include		<ctime>
#include		<iostream>
#include		<sstream>
#include		<set>
#include		"Track.hpp"
#include		"Screen.hpp"

const size_t		hbs::Track::NoNode = std::numeric_limits<size_t>::max();

void			hbs::Track::CleanOld(void)
{
  if (timer.GetTime() > 4)
    timeline.erase(timeline.begin(), timeline.find(timer.GetTime() - 3));
}

bool			hbs::Track::AlreadyComputed(size_t n, hbs::Tristate &out)
{
  std::map<size_t, PinState>::iterator pin;
  std::map<size_t, Tristate>::iterator tri;

  CleanOld();
  if (n < 1 || n > attachments.size())
    throw hbs::BadPin(GetType() + ": Bad pin.");
  if ((pin = timeline.find(timer.GetTime())) != timeline.end())
    if ((tri = pin->second.find(n)) != pin->second.end())
      {
	out = tri->second;
	return (true);
      }
  return (false);
}

hbs::Track::Preset	hbs::Track::PresetOutput(size_t n)
{
  std::map<size_t, PinState>::iterator it;
  std::map<size_t, Tristate>::iterator itx;

  if ((it = timeline.find(timer.GetTime())) == timeline.end())
    timeline[timer.GetTime()][n] = hbs::UNDEFINED;
  else if ((itx = it->second.find(n)) == it->second.end())
    it->second[n] = hbs::UNDEFINED;
  else
    return (itx);
  return (timeline[timer.GetTime()].find(n));
}

void			hbs::Track::SetAllPins(hbs::Tristate value)
{
  for (size_t i = 1; i <= attachments.size(); ++i)
    timeline[timer.GetTime()][i] = value;
}

hbs::Positions::iterator hbs::Track::NodeIterator(size_t node)
{
  hbs::Positions::iterator it = path.begin();

  for (size_t i = 0; it != path.end() && i < node; ++i, ++it);
  return (it);
}

hbs::Positions::const_iterator hbs::Track::NodeIterator(size_t node) const
{
  hbs::Positions::const_iterator it = path.begin();

  for (size_t i = 0; it != path.end() && i < node; ++i, ++it);
  return (it);
}

size_t			hbs::Track::DefaultNodeForPin(size_t pin, const hbs::IComponent &, size_t) const
{
  if (path.empty())
    return (NoNode);
  if (pin <= path.size())
    return (pin - 1);
  return (path.size() - 1);
}

static bool		read_number_pair(const std::string &geometry, int &i, hbs::Position &pos)
{
  int			j;
  size_t		tmp = 0;

  ReadWhitespace(geometry, i);
  j = i;
  ReadChar(geometry, i);
  pos.x = std::stof(&geometry[j], &tmp);
  if (tmp == 0)
    return (false);
  ReadWhitespace(geometry, i);
  if (!ReadText(geometry, i, ","))
    return (false);
  ReadWhitespace(geometry, i);
  j = i;
  ReadChar(geometry, i);
  pos.y = std::stof(&geometry[j], &tmp);
  return (tmp != 0);
}

void			hbs::Track::SetPath(const std::string &geometry)
{
  int			i = 0;
  hbs::Position		pos;
  hbs::ILink::Layer	layer = hbs::ILink::BOTTOM;

  path.clear();
  segments.clear();
  if (geometry == "")
    return ;
  ReadWhitespace(geometry, i);
  if (!ReadText(geometry, i, "["))
    throw hbs::SyntaxError(&geometry[i]);
  while (geometry[i] != '\0' && geometry[i] != ']')
    {
      ReadWhitespace(geometry, i);
      if (ReadText(geometry, i, "t"))
	layer = hbs::ILink::TOP;
      else if (ReadText(geometry, i, "b"))
	layer = hbs::ILink::BOTTOM;
      if (!read_number_pair(geometry, i, pos))
	throw hbs::SyntaxError(&geometry[i]);
      AddNode(pos, layer);
      ReadWhitespace(geometry, i);
    }
  if (geometry[i] != ']')
    throw hbs::SyntaxError("Missing ']'.");
  i += 1;
  ReadWhitespace(geometry, i);
  if (geometry[i] == '{')
    {
      int		j;
      int		from;
      int		to;

      i += 1;
      segments.clear();
      while (geometry[i] != '\0' && geometry[i] != '}')
	{
	  ReadWhitespace(geometry, i);
	  if (geometry[i] == '}')
	    break ;
	  j = i;
	  while (geometry[i] >= '0' && geometry[i] <= '9')
	    i += 1;
	  from = std::atoi(&geometry[j]);
	  if (from <= 0)
	    throw hbs::SyntaxError(&geometry[i]);
	  ReadWhitespace(geometry, i);
	  if (!ReadText(geometry, i, "-"))
	    throw hbs::SyntaxError(&geometry[i]);
	  ReadWhitespace(geometry, i);
	  j = i;
	  while (geometry[i] >= '0' && geometry[i] <= '9')
	    i += 1;
	  to = std::atoi(&geometry[j]);
	  if (to <= 0)
	    throw hbs::SyntaxError(&geometry[i]);
	  AddSegment((size_t)from - 1, (size_t)to - 1);
	  ReadWhitespace(geometry, i);
	}
      if (geometry[i] != '}')
	throw hbs::SyntaxError("Missing '}'.");
    }
}

const hbs::Positions	&hbs::Track::GetPath(void) const
{
  return (path);
}

hbs::Positions		&hbs::Track::GetPath(void)
{
  return (path);
}

const std::vector<hbs::Track::Segment> &hbs::Track::GetSegments(void) const
{
  return (segments);
}

const std::vector<hbs::Track::Attachment> &hbs::Track::GetAttachments(void) const
{
  return (attachments);
}

void			hbs::Track::SetAttachmentNode(size_t pin, size_t node)
{
  if (pin == 0 || pin > attachments.size())
    throw hbs::BadPin(GetType() + ": Bad pin.");
  if (node != NoNode && node >= path.size())
    throw hbs::BadPin(GetType() + ": Bad path node.");
  attachments[pin - 1].node = node;
}

size_t			hbs::Track::GetAttachmentNode(size_t pin) const
{
  if (pin == 0 || pin > attachments.size())
    throw hbs::BadPin(GetType() + ": Bad pin.");
  return (attachments[pin - 1].node);
}

size_t			hbs::Track::AddFreeNode(const hbs::Position &pos, hbs::ILink::Layer layer)
{
  size_t		idx = path.size();

  path.push_back(std::make_pair(pos, layer));
  return (idx);
}

size_t			hbs::Track::AddNode(const hbs::Position &pos, hbs::ILink::Layer layer)
{
  size_t		idx = AddFreeNode(pos, layer);

  if (idx > 0)
    AddSegment(idx - 1, idx);
  return (idx);
}

size_t			hbs::Track::AddNodeAfter(size_t from, const hbs::Position &pos, hbs::ILink::Layer layer)
{
  size_t		idx;

  if (from != NoNode && from >= path.size())
    throw hbs::BadPin(GetType() + ": Bad path node.");
  idx = path.size();
  path.push_back(std::make_pair(pos, layer));
  if (from != NoNode)
    AddSegment(from, idx);
  else if (idx > 0)
    AddSegment(idx - 1, idx);
  return (idx);
}

void			hbs::Track::AddSegment(size_t from, size_t to)
{
  if (from >= path.size() || to >= path.size() || from == to)
    throw hbs::BadPin(GetType() + ": Bad segment.");
  for (size_t i = 0; i < segments.size(); ++i)
    if ((segments[i].from == from && segments[i].to == to) ||
	(segments[i].from == to && segments[i].to == from))
      return ;
  segments.push_back({from, to});
}

static bool		node_has_attachment(const std::vector<hbs::Track::Attachment> &attachments,
					    size_t node)
{
  for (size_t i = 0; i < attachments.size(); ++i)
    if (attachments[i].node == node && attachments[i].component != NULL)
      return (true);
  return (false);
}

static size_t		node_degree(const std::vector<hbs::Track::Segment> &segments,
				    size_t node)
{
  size_t		degree = 0;

  for (size_t i = 0; i < segments.size(); ++i)
    if (segments[i].from == node || segments[i].to == node)
      degree += 1;
  return (degree);
}

static bool		find_single_neighbor(const std::vector<hbs::Track::Segment> &segments,
					     size_t node,
					     size_t &neighbor,
					     size_t &segment)
{
  for (size_t i = 0; i < segments.size(); ++i)
    if (segments[i].from == node || segments[i].to == node)
      {
	neighbor = (segments[i].from == node ? segments[i].to : segments[i].from);
	segment = i;
	return (true);
      }
  return (false);
}

static void		prune_from_node(std::vector<hbs::Track::Segment> &segments,
					std::vector<hbs::Track::Attachment> &attachments,
					std::set<size_t> &deleted_nodes,
					size_t node)
{
  while (true)
    {
      size_t degree = node_degree(segments, node);
      size_t neighbor;
      size_t segment;

      if (degree > 1 || node_has_attachment(attachments, node))
	return ;
      deleted_nodes.insert(node);
      if (degree == 0 || !find_single_neighbor(segments, node, neighbor, segment))
	return ;
      segments.erase(segments.begin() + segment);
      node = neighbor;
    }
}

bool			hbs::Track::DeleteBranchFromSegment(size_t segment)
{
  Segment		old;
  std::set<size_t>	deleted_nodes;
  std::vector<size_t>	remap(path.size(), NoNode);
  hbs::Positions	newpath;

  if (segment >= segments.size())
    return (false);
  old = segments[segment];
  segments.erase(segments.begin() + segment);
  prune_from_node(segments, attachments, deleted_nodes, old.from);
  prune_from_node(segments, attachments, deleted_nodes, old.to);
  if (deleted_nodes.empty())
    return (true);
  for (size_t i = 0; i < path.size(); ++i)
    if (deleted_nodes.find(i) == deleted_nodes.end())
      {
	remap[i] = newpath.size();
	hbs::Positions::const_iterator it = NodeIterator(i);
	newpath.push_back(*it);
      }
  for (size_t i = 0; i < segments.size();)
    {
      if (segments[i].from >= remap.size() || segments[i].to >= remap.size() ||
	  remap[segments[i].from] == NoNode || remap[segments[i].to] == NoNode)
	segments.erase(segments.begin() + i);
      else
	{
	  segments[i].from = remap[segments[i].from];
	  segments[i].to = remap[segments[i].to];
	  ++i;
	}
    }
  for (size_t i = 0; i < attachments.size(); ++i)
    if (attachments[i].node != NoNode)
      {
	if (attachments[i].node >= remap.size() || remap[attachments[i].node] == NoNode)
	  attachments[i].node = NoNode;
	else
	  attachments[i].node = remap[attachments[i].node];
      }
  path = newpath;
  return (true);
}


size_t			hbs::Track::InsertNodeOnSegment(size_t segment, const hbs::Position &pos)
{
  if (segment >= segments.size())
    throw hbs::BadPin(GetType() + ": Bad segment.");
  Segment		old = segments[segment];
  hbs::ILink::Layer	layer = GetNodeLayer(old.from);
  size_t		node;

  if (GetNodeLayer(old.to) != layer)
    layer = hbs::ILink::BOTTOM;
  segments.erase(segments.begin() + segment);
  node = AddFreeNode(pos, layer);
  AddSegment(old.from, node);
  AddSegment(node, old.to);
  return (node);
}

void			hbs::Track::MoveNode(size_t node, const hbs::Position &delta)
{
  hbs::Positions::iterator it = NodeIterator(node);

  if (it == path.end())
    throw hbs::BadPin(GetType() + ": Bad path node.");
  it->first.x += delta.x;
  it->first.y += delta.y;
}

hbs::Position		hbs::Track::GetNodePosition(size_t node) const
{
  hbs::Positions::const_iterator it = NodeIterator(node);

  if (it == path.end())
    throw hbs::BadPin(GetType() + ": Bad path node.");
  return (it->first);
}

hbs::ILink::Layer	hbs::Track::GetNodeLayer(size_t node) const
{
  hbs::Positions::const_iterator it = NodeIterator(node);

  if (it == path.end())
    throw hbs::BadPin(GetType() + ": Bad path node.");
  return (it->second);
}

void			hbs::Track::SetNodeLayer(size_t node, hbs::ILink::Layer layer)
{
  hbs::Positions::iterator it = NodeIterator(node);

  if (it == path.end())
    throw hbs::BadPin(GetType() + ": Bad path node.");
  it->second = layer;
}

void			hbs::Track::ToggleNodeLayer(size_t node)
{
  hbs::Positions::iterator it = NodeIterator(node);

  if (it == path.end())
    throw hbs::BadPin(GetType() + ": Bad path node.");
  it->second = (it->second == hbs::ILink::TOP ? hbs::ILink::BOTTOM : hbs::ILink::TOP);
}

double			hbs::Track::DistanceToSegment(const hbs::Position &p, const hbs::Position &a, const hbs::Position &b) const
{
  double		dx = b.x - a.x;
  double		dy = b.y - a.y;
  double		len2 = dx * dx + dy * dy;
  double		t;
  hbs::Position		proj;

  if (len2 <= 0.000001)
    return (sqrt(pow(p.x - a.x, 2) + pow(p.y - a.y, 2)));
  t = ((p.x - a.x) * dx + (p.y - a.y) * dy) / len2;
  if (t < 0.0)
    t = 0.0;
  if (t > 1.0)
    t = 1.0;
  proj.x = a.x + t * dx;
  proj.y = a.y + t * dy;
  return (sqrt(pow(p.x - proj.x, 2) + pow(p.y - proj.y, 2)));
}

size_t			hbs::Track::FindNode(const hbs::Screen &screen, t_bunny_position pos) const
{
  size_t		i = 0;

  for (hbs::Positions::const_iterator it = path.begin(); it != path.end(); ++it, ++i)
    {
      double dist = sqrt(pow(pos.x - it->first.x, 2) + pow(pos.y - it->first.y, 2));
      if (dist < std::max(0.45, 8.0 / screen.pin_size))
	return (i);
    }
  return (NoNode);
}

size_t			hbs::Track::FindSegment(const hbs::Screen &screen, t_bunny_position pos) const
{
  hbs::Position		p = {(double)pos.x, (double)pos.y};

  for (size_t i = 0; i < segments.size(); ++i)
    if (DistanceToSegment(p, GetNodePosition(segments[i].from), GetNodePosition(segments[i].to)) < std::max(0.30, 6.0 / screen.pin_size))
      return (i);
  return (NoNode);
}

void			hbs::Track::SetRoutingError(bool error)
{
  routing_error = error;
}

bool			hbs::Track::HasRoutingError(void) const
{
  return (routing_error);
}

hbs::Packet		hbs::Track::EndLinkStep(void)
{
  return (hbs::Packet());
}

hbs::Packet		hbs::Track::GetLinkStep(const hbs::Screen &screen, t_bunny_position pos)
{
  size_t		node = FindNode(screen, pos);

  if (node != NoNode)
    return (hbs::Packet(this, node));
  return (EndLinkStep());
}

const std::string	&hbs::Track::GetType(void) const
{
  return (type = "wire");
}

const std::string	&hbs::Track::GetName(void) const
{
  return (name);
}

void			hbs::Track::SetName(const std::string &n)
{
  name = n;
}

void			hbs::Track::DisconnectFrom(const hbs::IComponent *component)
{
  for (size_t i = 0; i < attachments.size(); ++i)
    if (attachments[i].component == component)
      {
	attachments[i].component = NULL;
	attachments[i].pin = 0;
      }
}

void			hbs::Track::RemoveAttachment(size_t pin)
{
  if (pin == 0 || pin > attachments.size())
    return ;
  attachments.erase(attachments.begin() + pin - 1);
}

size_t			hbs::Track::GetPinCount(void) const
{
  return (attachments.size());
}

static void		collect_connected_nodes(const std::vector<hbs::Track::Segment> &segments,
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

static bool		attachment_is_on_same_net(const std::vector<hbs::Track::Attachment> &attachments,
					      const std::vector<hbs::Track::Segment> &segments,
					      size_t pin_num_this,
					      size_t attachment)
{
  if (pin_num_this == 0 || pin_num_this > attachments.size())
    return (false);
  const hbs::Track::Attachment &origin = attachments[pin_num_this - 1];
  const hbs::Track::Attachment &other = attachments[attachment];

  if (origin.node == hbs::Track::NoNode || other.node == hbs::Track::NoNode)
    return (origin.node == other.node);
  if (origin.node == other.node)
    return (true);
  std::set<size_t> nodes;

  collect_connected_nodes(segments, origin.node, nodes);
  return (nodes.find(other.node) != nodes.end());
}

void			hbs::Track::SetNetPins(size_t pin_num_this, hbs::Tristate value)
{
  for (size_t i = 0; i < attachments.size(); ++i)
    if (attachment_is_on_same_net(attachments, segments, pin_num_this, i))
      timeline[timer.GetTime()][i + 1] = value;
}

hbs::Tristate		hbs::Track::Compute(size_t pin_num_this)
{
  hbs::Tristate		out;
  hbs::Tristate		tmp;

  if (AlreadyComputed(pin_num_this, out) && !computing)
    return (out);
  if (computing)
    return (hbs::UNDEFINED);
  computing = true;
  PresetOutput(pin_num_this);
  out = hbs::UNDEFINED;
  for (size_t i = 0; i < attachments.size(); ++i)
    if (attachments[i].component &&
	attachments[i].component->DrivesPin(attachments[i].pin) &&
	attachment_is_on_same_net(attachments, segments, pin_num_this, i))
      {
	tmp = attachments[i].component->Compute(attachments[i].pin);
	if (tmp != hbs::UNDEFINED)
	  {
	    if (out != hbs::UNDEFINED)
	      {
		SetNetPins(pin_num_this, hbs::BROKEN);
		computing = false;
		return (hbs::BROKEN);
	      }
	    out = tmp;
	  }
      }
  SetNetPins(pin_num_this, out);
  computing = false;
  return (out);
}

void			hbs::Track::SetLink(size_t pin_num_this,
					 hbs::IComponent &component,
					 size_t pin_num_target,
					 const std::string &pos)
{
  if (pin_num_this == 0)
    throw hbs::BadPin(GetType() + ": Bad pin.");
  if (attachments.size() < pin_num_this)
    attachments.resize(pin_num_this, {NULL, 0, NoNode});
  Attachment &att = attachments[pin_num_this - 1];
  if (att.component == &component && att.pin == pin_num_target)
    return ;
  att.component = &component;
  att.pin = pin_num_target;
  if (att.node == NoNode)
    att.node = DefaultNodeForPin(pin_num_this, component, pin_num_target);
  if (pos.empty() || pos[0] != '!')
    component.SetLink(pin_num_target, *this, pin_num_this, "!" + pos);
}

bool			hbs::Track::IsUnder(const hbs::Screen &screen, const t_bunny_position &pos) const
{
  return (FindNode(screen, pos) != NoNode || FindSegment(screen, pos) != NoNode);
}

void			hbs::Track::Move(const hbs::Position &pos)
{
  for (hbs::Positions::iterator it = path.begin(); it != path.end(); ++it)
    {
      it->first.x += pos.x;
      it->first.y += pos.y;
    }
}

hbs::Position		hbs::Track::GetPosition(void) const
{
  if (!path.empty())
    return (path.begin()->first);
  if (!attachments.empty() && attachments[0].component)
    return (attachments[0].component->GetPinPosition(attachments[0].pin));
  return (hbs::Position{0, 0});
}

hbs::Position		hbs::Track::GetPinPosition(size_t pin) const
{
  if (pin == 0 || pin > attachments.size())
    throw hbs::BadPin(GetType() + ": Bad pin.");
  const Attachment &att = attachments[pin - 1];
  if (att.node != NoNode && att.node < path.size())
    return (GetNodePosition(att.node));
  if (att.component)
    return (att.component->GetPinPosition(att.pin));
  return (hbs::Position{0, 0});
}

static void		draw_thick_line(hbs::Screen &screen,
					const hbs::Position &a,
					const hbs::Position &b,
					unsigned int col,
					double pixels)
{
  double		dx = b.x - a.x;
  double		dy = b.y - a.y;
  double		len = sqrt(dx * dx + dy * dy);

  if (len < 0.000001)
    {
      screen.Circle(a, {pixels / screen.pin_size, pixels / screen.pin_size}, col, true);
      return ;
    }
  double ox = -dy / len / screen.pin_size;
  double oy = dx / len / screen.pin_size;
  int radius = std::max(1, (int)ceil(pixels / 2.0));

  for (int i = -radius; i <= radius; ++i)
    screen.Line({a.x + ox * i, a.y + oy * i}, {b.x + ox * i, b.y + oy * i}, col);
}

void			hbs::Track::Draw(hbs::Screen &screen) const
{
  unsigned int		error_color;
  double		pulse;
  const bool		physical = (!segments.empty());

  if (path.empty())
    {
      if (attachments.size() >= 2 && attachments[0].component && attachments[1].component)
	screen.Line(attachments[0].component->GetPinPosition(attachments[0].pin),
		    attachments[1].component->GetPinPosition(attachments[1].pin),
		    hbs::Screen::Green);
      return ;
    }
  pulse = (sin(clock() / (double)CLOCKS_PER_SEC * 3.0) + 1.0) / 2.0;
  error_color = pulse > 0.45 ? hbs::Screen::Yellow : hbs::Screen::Red;
  for (size_t i = 0; i < segments.size(); ++i)
    {
      hbs::Positions::const_iterator a = NodeIterator(segments[i].from);
      hbs::Positions::const_iterator b = NodeIterator(segments[i].to);
      unsigned int col = a->second == hbs::ILink::TOP ? hbs::Screen::Red : hbs::Screen::Blue;

      if (routing_error)
	col = error_color;
      draw_thick_line(screen, a->first, b->first, col, 4.0);
      if (a->second != b->second)
	{
	  screen.Circle(b->first, {0.42, 0.42}, hbs::Screen::Yellow, true);
	  screen.Circle(b->first, {0.30, 0.30}, hbs::Screen::White, false);
	}
    }
  for (hbs::Positions::const_iterator node = path.begin(); node != path.end(); ++node)
    screen.Circle(node->first, {0.22, 0.22}, node->second == hbs::ILink::TOP ? hbs::Screen::Red : hbs::Screen::Blue, true);
  for (size_t i = 0; i < attachments.size(); ++i)
    if (attachments[i].component)
      {
	hbs::Position a = attachments[i].component->GetPinPosition(attachments[i].pin);
	hbs::Position b = GetPinPosition(i + 1);
	unsigned int col = routing_error ? error_color : (physical ? hbs::Screen::Yellow : hbs::Screen::Green);

	screen.Line(a, b, col);
	screen.Circle(b, {0.35, 0.35}, col, true);
      }
}

void			hbs::Track::Dump(void) const
{
  std::cerr << "Track " << name << " pins=" << attachments.size()
	    << " nodes=" << path.size() << " segments=" << segments.size();
  if (routing_error)
    std::cerr << " ROUTING-ERROR";
  std::cerr << std::endl;
}

const hbs::IComponent	*hbs::Track::GetTarget(void) const
{
  return (NULL);
}

size_t			hbs::Track::GetTargetPin(void) const
{
  return (0);
}

size_t			hbs::Track::GetNbrSteps(void) const
{
  return (path.size());
}

std::pair<int, int>	hbs::Track::GetStep(size_t i) const
{
  hbs::Positions::const_iterator it = NodeIterator(i);

  if (it == path.end())
    throw 0;
  return (std::pair<int, int>(round(it->first.x), round(it->first.y)));
}

hbs::Track::Track(const hbs::Timer &tim, const std::string &nam, const std::string &geometry)
  : timer(tim),
    name(nam),
    routing_error(false),
    computing(false)
{
  SetPath(geometry);
}

hbs::Track::~Track(void)
{}
