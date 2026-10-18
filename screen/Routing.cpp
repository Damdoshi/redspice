// RED Spice - temporary routing mode and non-persistent route preview.
#include "Circuit.hpp"
#include "Screen.hpp"
#include "Track.hpp"
#include <cmath>

bool hbs::Screen::IsRoutingMode(void) const
{
  return drawing_mode || bunny_get_keyboard()[BKS_LCONTROL] || bunny_get_keyboard()[BKS_RCONTROL];
}

bool hbs::Screen::GetRoutePreview(const Circuit &c, Position &from,
                                  Position &to, unsigned int &color) const
{
  if (!active_track || active_node == Track::NoNode ||
      active_node >= active_track->GetNbrSteps() || panning || placing_component ||
      search_panel || rename_mode || context_menu)
    return false;
  from = active_track->GetNodePosition(active_node);
  Position mouse = ScreenToWorld(*bunny_get_mouse_position());
  t_bunny_position snapped = {int(std::round(mouse.x)), int(std::round(mouse.y))};
  to = {double(snapped.x), double(snapped.y)};
  // Match the click handler's priorities and snapping exactly.
  ComponentPin pin = c.GetPinAt(*this, snapped);
  if (pin.IsValid()) to = pin.component->GetPinPosition(pin.pin);
  else
    {
      Packet step = c.GetLinkStep(*this, snapped);
      if (step != c.EndLinkStep()) to = step.track->GetNodePosition(step.node);
    }
  color = active_track->GetNodeLayer(active_node) == ILink::TOP ? Red : Blue;
  return true;
}

void hbs::Screen::DrawRoutePreview(const Circuit &c)
{
  Position from, to;
  unsigned int color;
  if (!GetRoutePreview(c, from, to, color)) return;
  double dx = to.x - from.x, dy = to.y - from.y, length = std::hypot(dx, dy);
  if (length > 1e-9)
    {
      Position offset{-dy / length / pin_size, dx / length / pin_size};
      Line(from - offset, to - offset, color);
      Line(from, to, color);
      Line(from + offset, to + offset, color);
    }
  Circle(to, {.16, .16}, White, false);
}
