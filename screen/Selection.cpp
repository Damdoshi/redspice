// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<algorithm>
#include		<cmath>
#include		"IComponent.hpp"
#include		"Screen.hpp"
#include		"Track.hpp"

static void		add_point(bool &valid,
				  hbs::Position &from,
				  hbs::Position &to,
				  const hbs::Position &p)
{
  if (!valid)
    {
      from = p;
      to = p;
      valid = true;
      return ;
    }
  from.x = std::min(from.x, p.x);
  from.y = std::min(from.y, p.y);
  to.x = std::max(to.x, p.x);
  to.y = std::max(to.y, p.y);
}

bool			hbs::Screen::HasSelection(void) const
{
  return (!selected_components.empty() || !selected_tracks.empty());
}

bool			hbs::Screen::GetSelectionBounds(hbs::Position &from,
							 hbs::Position &to) const
{
  bool			valid = false;

  for (std::set<hbs::IComponent*>::const_iterator it = selected_components.begin();
       it != selected_components.end(); ++it)
    {
      add_point(valid, from, to, (*it)->GetPosition());
      for (size_t pin = 1; pin <= (*it)->GetPinCount(); ++pin)
	add_point(valid, from, to, (*it)->GetPinPosition(pin));
    }
  for (std::set<hbs::Track*>::const_iterator it = selected_tracks.begin();
       it != selected_tracks.end(); ++it)
    {
      const hbs::Positions &path = (*it)->GetPath();

      for (hbs::Positions::const_iterator node = path.begin(); node != path.end(); ++node)
	add_point(valid, from, to, node->first);
      if (path.empty())
	add_point(valid, from, to, (*it)->GetPosition());
      for (size_t pin = 1; pin <= (*it)->GetPinCount(); ++pin)
	add_point(valid, from, to, (*it)->GetPinPosition(pin));
    }
  if (!valid)
    return (false);
  double padding = std::max(0.5, 10.0 / std::max(1, pin_size));
  from.x -= padding;
  from.y -= padding;
  to.x += padding;
  to.y += padding;
  return (true);
}

bool			hbs::Screen::IsInsideSelectionBox(const hbs::Position &pos) const
{
  hbs::Position		from;
  hbs::Position		to;

  if (!GetSelectionBounds(from, to))
    return (false);
  return (pos.x >= from.x && pos.x <= to.x && pos.y >= from.y && pos.y <= to.y);
}

void			hbs::Screen::DrawSelectionBox(void)
{
  hbs::Position		from;
  hbs::Position		to;

  if (!GetSelectionBounds(from, to))
    return ;
  hbs::Position corner = {std::min(from.x, to.x), std::min(from.y, to.y)};
  hbs::Size size = {fabs(to.x - from.x), fabs(to.y - from.y)};

  Square(corner, size, ALPHA(48, WHITE), true);
  Square(corner, size, WHITE, false);
}

hbs::IComponent	*hbs::Screen::GetUniqueSelection(void) const
{
  if (selected_components.size() + selected_tracks.size() != 1)
    return (NULL);
  if (!selected_components.empty())
    return (*selected_components.begin());
  return (*selected_tracks.begin());
}
