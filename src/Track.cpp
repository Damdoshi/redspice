// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include		<cmath>
#include		<iostream>
#include		<sstream>
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

hbs::Track::Preset		hbs::Track::PresetOutput(size_t n)
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

size_t			hbs::Track::DefaultNodeForPin(size_t pin, const hbs::IComponent &, size_t) const
{
  if (path.empty())
    return (NoNode);
  if (pin <= path.size())
    return (pin - 1);
  return (path.size() - 1);
}

void			hbs::Track::SetPath(const std::string &geometry)
{
  int			i = 0;
  int			j = 0;
  hbs::Position		pos;
  size_t		tmp = 0;
  hbs::ILink::Layer	layer = hbs::ILink::BOTTOM;

  path.clear();
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
      ReadWhitespace(geometry, i);
      j = i;
      ReadChar(geometry, i);
      pos.x = std::stof(&geometry[j], &tmp);
      if (tmp == 0)
	throw hbs::SyntaxError(&geometry[i]);
      ReadWhitespace(geometry, i);
      if (!ReadText(geometry, i, ","))
	throw hbs::SyntaxError(&geometry[i]);
      ReadWhitespace(geometry, i);
      j = i;
      ReadChar(geometry, i);
      pos.y = std::stof(&geometry[j], &tmp);
      if (tmp == 0)
	throw hbs::SyntaxError(&geometry[i]);
      ReadWhitespace(geometry, i);
      path.push_back({pos, layer});
    }
  if (geometry[i] != ']')
    throw hbs::SyntaxError("Missing ']'.");
}

const hbs::Positions	&hbs::Track::GetPath(void) const
{
  return (path);
}

hbs::Positions		&hbs::Track::GetPath(void)
{
  return (path);
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

hbs::Positions::iterator hbs::Track::EndLinkStep(void)
{
  return (path.end());
}

hbs::Positions::iterator hbs::Track::GetLinkStep(const hbs::Screen &screen, t_bunny_position pos)
{
  for (hbs::Positions::iterator it = path.begin(); it != path.end(); ++it)
    {
      int dist = sqrt(pow(pos.x - it->first.x, 2) + pow(pos.y - it->first.y, 2));
      if (dist < screen.pin_size)
	return (it);
    }
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

size_t			hbs::Track::GetPinCount(void) const
{
  return (attachments.size());
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
    if (attachments[i].component)
      {
	tmp = attachments[i].component->Compute(attachments[i].pin);
	if (out == hbs::UNDEFINED)
	  out = tmp;
	else if (tmp != hbs::UNDEFINED && tmp != out)
	  {
	    SetAllPins(hbs::BROKEN);
	    computing = false;
	    return (hbs::BROKEN);
	  }
      }
  SetAllPins(out);
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
  for (hbs::Positions::const_iterator it = path.begin(); it != path.end(); ++it)
    {
      int dist = sqrt(pow(pos.x - it->first.x, 2) + pow(pos.y - it->first.y, 2));
      if (dist < screen.pin_size)
	return (true);
    }
  return (false);
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
    {
      hbs::Positions::const_iterator it = path.begin();
      for (size_t i = 0; i < att.node; ++i, ++it);
      return (it->first);
    }
  if (att.component)
    return (att.component->GetPinPosition(att.pin));
  return (hbs::Position{0, 0});
}

void			hbs::Track::Draw(hbs::Screen &screen) const
{
  hbs::Positions::const_iterator prev;

  if (path.empty())
    {
      if (attachments.size() >= 2 && attachments[0].component && attachments[1].component)
	screen.Line(attachments[0].component->GetPinPosition(attachments[0].pin),
		    attachments[1].component->GetPinPosition(attachments[1].pin),
		    hbs::Screen::White);
      return ;
    }
  prev = path.begin();
  for (hbs::Positions::const_iterator it = path.begin(); it != path.end(); ++it)
    {
      screen.Line(prev->first, it->first, prev->second == hbs::ILink::TOP ? hbs::Screen::Red : hbs::Screen::Blue);
      if (prev != it && prev->second != it->second)
	{
	  screen.Circle(it->first, {1, 1}, hbs::Screen::Yellow, true);
	  screen.Circle(it->first, {1, 1}, hbs::Screen::White, false);
	}
      prev = it;
    }
  for (size_t i = 0; i < attachments.size(); ++i)
    if (attachments[i].component)
      {
	hbs::Position a = attachments[i].component->GetPinPosition(attachments[i].pin);
	hbs::Position b = GetPinPosition(i + 1);
	screen.Line(a, b, hbs::Screen::White);
	screen.Circle(b, {0.35, 0.35}, hbs::Screen::Yellow, true);
      }
}

void			hbs::Track::Dump(void) const
{
  std::cerr << "Track " << name << " pins=" << attachments.size() << " nodes=" << path.size() << std::endl;
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
  hbs::Positions::const_iterator it = path.begin();

  for (; it != path.end() && i > 0; ++it, --i);
  if (it == path.end())
    throw 0;
  return (std::pair<int, int>(round(it->first.x), round(it->first.y)));
}

hbs::Track::Track(const hbs::Timer &tim, const std::string &nam, const std::string &geometry)
  : timer(tim),
    name(nam),
    computing(false)
{
  SetPath(geometry);
}

hbs::Track::~Track(void)
{}
