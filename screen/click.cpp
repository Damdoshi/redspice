// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<cmath>
#include		<chrono>
#include		<algorithm>
#include		"Circuit.hpp"
#include		"Screen.hpp"
#include		"Track.hpp"

static bool		is_toolbar(t_bunny_position pos)
{
  return (pos.y >= 0 && pos.y < 75 && pos.x >= 0 && pos.x < 600);
}

static bool		is_inside_context_menu(const hbs::Screen &screen, t_bunny_position pos)
{
  int relx = pos.x - screen.context_pos.x;
  int rely = pos.y - screen.context_pos.y;

  return (relx >= 0 && relx <= 130 && rely >= 0 && rely <= 58);
}

static void		trigger_context_menu_action(LoopData &ld, t_bunny_position pos)
{
  int rely = pos.y - ld.screen.context_pos.y;

  if (rely < 29)
    {
      hbs::IComponent *unique = ld.screen.GetUniqueSelection();
      if (unique)
	{
	  ld.screen.rename_buffer = unique->GetName();
	  ld.screen.rename_mode = true;
	}
    }
  else
    {
      ld.circuit.DeleteSelected(ld.screen.selected_components, ld.screen.selected_tracks);
      ld.screen.active_track = NULL;
      ld.screen.active_node = hbs::Track::NoNode;
    }
}

static t_bunny_position	world_mouse(hbs::Screen &screen)
{
  hbs::Position		world = screen.ScreenToWorld(*bunny_get_mouse_position());

  return (t_bunny_position{(int)round(world.x), (int)round(world.y)});
}

static hbs::Position	drag_delta(hbs::Screen &screen)
{
  t_bunny_accurate_position npos =
    {
      (double)bunny_get_mouse_position()->x,
      (double)bunny_get_mouse_position()->y
    };

  npos.x -= screen.grab_pos.x;
  npos.y -= screen.grab_pos.y;
  npos.x /= screen.pin_size;
  npos.y /= screen.pin_size;
  npos.x = round(npos.x);
  npos.y = round(npos.y);
  return (npos);
}

static void		clear_selection(hbs::Screen &screen)
{
  screen.selected_components.clear();
  screen.selected_tracks.clear();
}

static void		select_component(hbs::Screen &screen, hbs::IComponent *component)
{
  clear_selection(screen);
  if (component)
    screen.selected_components.insert(component);
}

static void		select_track(hbs::Screen &screen, hbs::Track *track)
{
  clear_selection(screen);
  if (track)
    screen.selected_tracks.insert(track);
}

static hbs::Position	snap_world(const hbs::Position &p)
{
  return (hbs::Position{(double)round(p.x), (double)round(p.y)});
}

static hbs::Position	snapped_mouse(hbs::Screen &screen)
{
  return (snap_world(screen.ScreenToWorld(*bunny_get_mouse_position())));
}

static bool		shift_pressed(void)
{
  return (bunny_get_keyboard()[BKS_LSHIFT] || bunny_get_keyboard()[BKS_RSHIFT]);
}

static bool		is_inside_component_menu(const hbs::Screen &screen, t_bunny_position pos)
{
  int x = screen.win->buffer.width - 330;
  int y = 80;

  return (screen.search_panel && pos.x >= x && pos.x <= x + 320 && pos.y >= y && pos.y <= y + 500);
}

static bool		component_menu_choice_at(const LoopData &ld,
					 t_bunny_position pos,
					 std::string &type)
{
  int y = 80;
  int line = 24;
  int rel;
  int wanted;
  int seen = 0;
  const std::vector<std::string> &types = ld.circuit.GetCreatableTypes();

  type = "";
  if (!is_inside_component_menu(ld.screen, pos))
    return (false);
  if (pos.y < y + 58 || pos.y >= y + 58 + 16 * line)
    return (false);
  rel = pos.y - (y + 58);
  wanted = rel / line;
  for (size_t i = 0; i < types.size(); ++i)
    {
      if (!ld.screen.search_query.empty() && types[i].find(ld.screen.search_query) == std::string::npos)
	continue ;
      if (seen++ < ld.screen.search_offset)
	continue ;
      if (wanted-- == 0)
	{
	  type = types[i];
	  return (true);
	}
    }
  return (false);
}

static void		move_placing_component_to_mouse(LoopData &ld)
{
  if (!ld.screen.placing_component || ld.screen.component_to_place == NULL)
    return ;
  hbs::Position target = snapped_mouse(ld.screen);
  hbs::Position current = ld.screen.component_to_place->GetPosition();

  ld.screen.component_to_place->Move({target.x - current.x, target.y - current.y});
}

static void		begin_component_placement(LoopData &ld, const std::string &type)
{
  if (type.empty())
    return ;
  hbs::Position pos = snapped_mouse(ld.screen);
  hbs::IComponent *cmp = ld.circuit.CreateUserComponent(type, pos);

  clear_selection(ld.screen);
  if (hbs::Track *track = dynamic_cast<hbs::Track*>(cmp))
    ld.screen.selected_tracks.insert(track);
  else
    ld.screen.selected_components.insert(cmp);
  ld.screen.placing_component = true;
  ld.screen.component_to_place = cmp;
  ld.screen.placing_type = type;
  ld.screen.search_panel = false;
  ld.screen.context_menu = false;
  move_placing_component_to_mouse(ld);
}

static void		cancel_component_placement(LoopData &ld)
{
  if (!ld.screen.placing_component || ld.screen.component_to_place == NULL)
    return ;
  std::set<hbs::IComponent*> components;
  std::set<hbs::Track*> tracks;

  if (hbs::Track *track = dynamic_cast<hbs::Track*>(ld.screen.component_to_place))
    tracks.insert(track);
  else
    components.insert(ld.screen.component_to_place);
  ld.circuit.DeleteSelected(components, tracks);
  ld.screen.placing_component = false;
  ld.screen.component_to_place = NULL;
  ld.screen.placing_type = "";
}

static void		finish_component_placement(LoopData &ld)
{
  std::string type = ld.screen.placing_type;

  move_placing_component_to_mouse(ld);
  if (!shift_pressed())
    {
      ld.screen.placing_component = false;
      ld.screen.component_to_place = NULL;
      ld.screen.placing_type = "";
      return ;
    }
  ld.screen.placing_component = false;
  ld.screen.component_to_place = NULL;
  ld.screen.placing_type = "";
  begin_component_placement(ld, type);
}

static bool		place_via(LoopData &ld)
{
  if (ld.screen.active_track == NULL || ld.screen.active_node == hbs::Track::NoNode)
    return (false);
  hbs::ILink::Layer oldlayer = ld.screen.active_track->GetNodeLayer(ld.screen.active_node);
  hbs::ILink::Layer newlayer = oldlayer == hbs::ILink::TOP ? hbs::ILink::BOTTOM : hbs::ILink::TOP;
  hbs::Position pos = ld.screen.active_track->GetNodePosition(ld.screen.active_node);

  ld.screen.active_node = ld.screen.active_track->AddNodeAfter(ld.screen.active_node, pos, newlayer);
  return (true);
}

static hbs::ILink::Layer current_layer(const hbs::Screen &screen)
{
  if (screen.active_track != NULL && screen.active_node != hbs::Track::NoNode)
    return (screen.active_track->GetNodeLayer(screen.active_node));
  return (hbs::ILink::BOTTOM);
}

static void		detach_draw_cursor(hbs::Screen &screen)
{
  screen.active_track = NULL;
  screen.active_node = hbs::Track::NoNode;
}

static size_t		find_track_attachment(hbs::Track *track,
				      const hbs::ComponentPin &pin)
{
  if (track == NULL || !pin.IsValid())
    return (hbs::Track::NoNode);
  const std::vector<hbs::Track::Attachment> &attachments = track->GetAttachments();

  for (size_t i = 0; i < attachments.size(); ++i)
    if (attachments[i].component == pin.component && attachments[i].pin == pin.pin)
      return (i + 1);
  return (hbs::Track::NoNode);
}

static size_t		ensure_pin_attachment_node(hbs::Track *track,
					   const hbs::ComponentPin &pin,
					   const hbs::Position &pos,
					   hbs::ILink::Layer layer)
{
  size_t		track_pin = find_track_attachment(track, pin);
  size_t		node;

  if (track_pin == hbs::Track::NoNode)
    return (hbs::Track::NoNode);
  node = track->GetAttachmentNode(track_pin);
  if (node != hbs::Track::NoNode)
    return (node);
  node = track->AddFreeNode(pos, layer);
  track->SetAttachmentNode(track_pin, node);
  return (node);
}

static void		attach_pin_to_active_track(LoopData &ld,
					       const hbs::ComponentPin &pin)
{
  if (!pin.IsValid() || ld.screen.active_track == NULL)
    return ;
  size_t existing_node = hbs::Track::NoNode;
  hbs::Track *existing = ld.circuit.FindTrackAttachedTo(pin.component, pin.pin, &existing_node);

  if (existing != NULL)
    existing_node = ensure_pin_attachment_node(existing, pin,
					      pin.component->GetPinPosition(pin.pin),
					      current_layer(ld.screen));
  if (existing != NULL && existing != ld.screen.active_track)
    {
      ld.screen.active_track = ld.circuit.MergeTracks(ld.screen.active_track, existing,
						       ld.screen.active_node, existing_node);
      detach_draw_cursor(ld.screen);
      return ;
    }
  if (existing == ld.screen.active_track)
    {
      if (existing_node != hbs::Track::NoNode &&
	  ld.screen.active_node != hbs::Track::NoNode &&
	  existing_node != ld.screen.active_node)
	ld.screen.active_track->AddSegment(ld.screen.active_node, existing_node);
      return ;
    }
  size_t track_pin = ld.screen.active_track->GetPinCount() + 1;
  ld.screen.active_track->SetLink(track_pin, *pin.component, pin.pin, "");
  ld.screen.active_track->SetAttachmentNode(track_pin, ld.screen.active_node);
}

static void		connect_pin_in_draw_mode(LoopData &ld,
					 const hbs::ComponentPin &pin)
{
  if (!pin.IsValid())
    return ;
  hbs::Position pp = pin.component->GetPinPosition(pin.pin);
  size_t existing_node = hbs::Track::NoNode;
  hbs::Track *existing = ld.circuit.FindTrackAttachedTo(pin.component, pin.pin, &existing_node);

  if (existing != NULL)
    existing_node = ensure_pin_attachment_node(existing, pin, pp, current_layer(ld.screen));

  if (ld.screen.active_track == NULL)
    {
      if (existing != NULL)
	{
	  ld.screen.active_track = existing;
	  ld.screen.active_node = existing_node;
	  return ;
	}
      ld.screen.active_track = ld.circuit.CreateUserTrackAt(pp);
      ld.screen.active_node = 0;
      attach_pin_to_active_track(ld, pin);
      return ;
    }
  if (existing != NULL && existing != ld.screen.active_track)
    {
      ld.circuit.MergeTracks(ld.screen.active_track, existing, ld.screen.active_node, existing_node);
      detach_draw_cursor(ld.screen);
      return ;
    }
  if (existing == ld.screen.active_track)
    {
      if (existing_node != hbs::Track::NoNode &&
	  ld.screen.active_node != hbs::Track::NoNode &&
	  existing_node != ld.screen.active_node)
	ld.screen.active_track->AddSegment(ld.screen.active_node, existing_node);
      detach_draw_cursor(ld.screen);
      return ;
    }
  ld.screen.active_node = ld.screen.active_track->AddNodeAfter(ld.screen.active_node, pp, current_layer(ld.screen));
  attach_pin_to_active_track(ld, pin);
  detach_draw_cursor(ld.screen);
}

static void		connect_track_node_in_draw_mode(LoopData &ld,
						 hbs::Track *track,
						 size_t node)
{
  if (track == NULL || node == hbs::Track::NoNode)
    return ;
  if (ld.screen.active_track == NULL)
    {
      ld.screen.active_track = track;
      ld.screen.active_node = node;
      return ;
    }
  if (ld.screen.active_track == track)
    {
      if (ld.screen.active_node != hbs::Track::NoNode && ld.screen.active_node != node)
	ld.screen.active_track->AddSegment(ld.screen.active_node, node);
      detach_draw_cursor(ld.screen);
      return ;
    }
  ld.circuit.MergeTracks(ld.screen.active_track, track, ld.screen.active_node, node);
  detach_draw_cursor(ld.screen);
}

static void		draw_mode_left_click(LoopData &ld, t_bunny_position pos)
{
  hbs::ComponentPin pin = ld.circuit.GetPinAt(ld.screen, pos);

  if (pin.IsValid())
    {
      connect_pin_in_draw_mode(ld, pin);
      return ;
    }

  hbs::Packet step = ld.circuit.GetLinkStep(ld.screen, pos);
  if (step != ld.circuit.EndLinkStep())
    {
      connect_track_node_in_draw_mode(ld, step.track, step.node);
      return ;
    }

  hbs::Track *track = ld.circuit.GetTrackAt(ld.screen, pos);
  if (track != NULL)
    {
      size_t segment = track->FindSegment(ld.screen, pos);
      if (segment != hbs::Track::NoNode)
	{
	  size_t node = track->InsertNodeOnSegment(segment, {(double)pos.x, (double)pos.y});
	  connect_track_node_in_draw_mode(ld, track, node);
	  return ;
	}
    }

  if (ld.screen.active_track == NULL)
    {
      ld.screen.active_track = ld.circuit.CreateUserTrackAt({(double)pos.x, (double)pos.y});
      ld.screen.active_node = 0;
      return ;
    }
  ld.screen.active_node = ld.screen.active_track->AddNodeAfter(ld.screen.active_node, {(double)pos.x, (double)pos.y}, current_layer(ld.screen));
}

static void		draw_mode_right_click(LoopData &ld, t_bunny_position pos)
{
  if (ld.screen.active_track != NULL)
    {
      ld.screen.active_track = NULL;
      ld.screen.active_node = hbs::Track::NoNode;
      return ;
    }
  hbs::Track *track = ld.circuit.GetTrackAt(ld.screen, pos);
  if (track != NULL)
    {
      size_t segment = track->FindSegment(ld.screen, pos);

      if (segment != hbs::Track::NoNode)
	{
	  track->DeleteBranchFromSegment(segment);
	  ld.circuit.NormalizeTracks();
	}
      else
	{
	  std::set<hbs::IComponent*> components;
	  std::set<hbs::Track*> tracks;

	  tracks.insert(track);
	  ld.circuit.DeleteSelected(components, tracks);
	  clear_selection(ld.screen);
	}
    }
}

extern "C"
t_bunny_response	screen_click(t_bunny_event_state	state,
			     t_bunny_mouse_button	sym,
			     LoopData			&ld)
{
  t_bunny_position	pos = *bunny_get_mouse_position();

  if (is_toolbar(pos))
    {
      if (state == GO_UP && sym == BMB_LEFT)
	{
	  int but = pos.x / 100;

	  if (but == 0)
	    ld.circuit.Save(ld.screen.file_name);
	  else if (but == 1)
	    ld.screen.loopsim = false;
	  else if (but == 2)
	    hbs::Command("simulate", ld.circuit, ld.timer);
	  else if (but == 3)
	    ld.screen.loopsim = true;
	  else if (but == 4)
	    {
	      ld.screen.search_panel = !ld.screen.search_panel;
	      ld.screen.search_offset = 0;
	    }
	  else if (but == 5)
	    {
	      ld.screen.drawing_mode = !ld.screen.drawing_mode;
	      ld.screen.context_menu = false;
	      ld.screen.selecting = false;
	      ld.screen.dragging_selection = false;
	    }
	}
      return (GO_ON);
    }

  if (ld.screen.search_panel && sym == BMB_LEFT && state == GO_DOWN)
    {
      std::string type;

      if (component_menu_choice_at(ld, pos, type))
	begin_component_placement(ld, type);
      else if (!is_inside_component_menu(ld.screen, pos))
	ld.screen.search_panel = false;
      return (GO_ON);
    }

  if (ld.screen.context_menu && sym == BMB_LEFT && state == GO_DOWN)
    {
      if (is_inside_context_menu(ld.screen, pos))
	trigger_context_menu_action(ld, pos);
      ld.screen.context_menu = false;
      return (GO_ON);
    }
  if (ld.screen.context_menu && sym == BMB_LEFT && state == GO_UP)
    return (GO_ON);

  pos = world_mouse(ld.screen);
  hbs::Input *in = ld.circuit.GetInput(ld.screen, pos);

  if (in && sym == BMB_MIDDLE && state == GO_UP)
    {
      if (in->GetPin(1) != hbs::TRUE)
	in->SetValue(hbs::TRUE);
      else
	in->SetValue(hbs::FALSE);
    }

  if (ld.screen.placing_component)
    {
      move_placing_component_to_mouse(ld);
      if (sym == BMB_LEFT && state == GO_DOWN)
	finish_component_placement(ld);
      else if (sym == BMB_RIGHT && state == GO_UP)
	cancel_component_placement(ld);
      return (GO_ON);
    }

  if (ld.screen.drawing_mode)
    {
      if (sym == BMB_LEFT && state == GO_DOWN && bunny_get_keyboard()[BKS_SPACE])
	{
	  ld.screen.panning = true;
	  ld.screen.pan_last = *bunny_get_mouse_position();
	  return (GO_ON);
	}
      if (sym == BMB_LEFT && state == GO_UP && ld.screen.panning)
	{
	  ld.screen.panning = false;
	  return (GO_ON);
	}
      if (sym == BMB_LEFT && state == GO_DOWN)
	{
	  static std::chrono::steady_clock::time_point last_click;
	  static t_bunny_position last_pos = {0, 0};
	  std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	  long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_click).count();
	  double dist = sqrt(pow(pos.x - last_pos.x, 2) + pow(pos.y - last_pos.y, 2));

	  if (elapsed < 360 && dist < std::max(0.75, 10.0 / ld.screen.pin_size))
	    {
	      hbs::Packet step = ld.circuit.GetLinkStep(ld.screen, pos);
	      bool via_done = false;

	      // Double-clicking the current drawing endpoint is the mouse equivalent
	      // of pressing V: insert a via at this exact point and continue routing
	      // on the other side of the board. This must also work when the point
	      // is under a component pin, so that a route can start from either side
	      // of a through-hole pad.
	      if (ld.screen.active_track != NULL && ld.screen.active_node != hbs::Track::NoNode)
		{
		  hbs::Position apos = ld.screen.active_track->GetNodePosition(ld.screen.active_node);
		  double adist = sqrt(pow(pos.x - apos.x, 2) + pow(pos.y - apos.y, 2));

		  if (adist < std::max(0.75, 10.0 / ld.screen.pin_size))
		    via_done = place_via(ld);
		}
	      if (!via_done && !ld.circuit.GetPinAt(ld.screen, pos).IsValid() &&
		  step != ld.circuit.EndLinkStep())
		{
		  ld.screen.active_track = step.track;
		  ld.screen.active_node = step.node;
		  via_done = place_via(ld);
		}
	      if (via_done)
		{
		  last_click = std::chrono::steady_clock::time_point();
		  return (GO_ON);
		}
	    }
	  last_click = now;
	  last_pos = pos;
	  draw_mode_left_click(ld, pos);
	}
      else if (sym == BMB_RIGHT && state == GO_UP)
	draw_mode_right_click(ld, pos);
      return (GO_ON);
    }

  hbs::IComponent *ic = ld.circuit.GetComponent(ld.screen, pos);
  hbs::Packet step = ld.circuit.GetLinkStep(ld.screen, pos);
  hbs::Track *track = ld.circuit.GetTrackAt(ld.screen, pos);

  if (sym == BMB_LEFT && state == GO_DOWN)
    {
      ld.screen.grab_pos = *bunny_get_mouse_position();
      ld.screen.selecting = false;
      ld.screen.dragging_selection = false;
      ld.screen.grabbed = NULL;
      ld.screen.grabbed_step = ld.circuit.EndLinkStep();
      if (bunny_get_keyboard()[BKS_SPACE])
	{
	  ld.screen.panning = true;
	  ld.screen.pan_last = ld.screen.grab_pos;
	  return (GO_ON);
	}
      if (ld.screen.HasSelection() && ld.screen.IsInsideSelectionBox({(double)pos.x, (double)pos.y}))
	{
	  ld.screen.dragging_selection = true;
	  return (GO_ON);
	}
      if (ic)
	{
	  if (ld.screen.selected_components.find(ic) == ld.screen.selected_components.end())
	    select_component(ld.screen, ic);
	  ld.screen.dragging_selection = true;
	  return (GO_ON);
	}
      if (track && ld.screen.selected_tracks.find(track) != ld.screen.selected_tracks.end() &&
	  (ld.screen.selected_tracks.size() + ld.screen.selected_components.size()) > 1)
	{
	  ld.screen.dragging_selection = true;
	  return (GO_ON);
	}
      if (track && step == ld.circuit.EndLinkStep())
	{
	  if (ld.screen.selected_tracks.find(track) == ld.screen.selected_tracks.end())
	    select_track(ld.screen, track);
	  ld.screen.dragging_selection = true;
	  return (GO_ON);
	}
      if (step != ld.circuit.EndLinkStep())
	{
	  ld.screen.grabbed_step = step;
	  return (GO_ON);
	}
      clear_selection(ld.screen);
      ld.screen.selecting = true;
      return (GO_ON);
    }

  if (sym == BMB_LEFT && state == GO_UP && ld.screen.panning)
    {
      ld.screen.panning = false;
      return (GO_ON);
    }

  if (sym == BMB_LEFT && state == GO_UP)
    {
      hbs::Position npos = drag_delta(ld.screen);

      if (ld.screen.selecting)
	{
	  hbs::Position from = ld.screen.ScreenToWorld(ld.screen.grab_pos);
	  hbs::Position to = ld.screen.ScreenToWorld(*bunny_get_mouse_position());

	  ld.circuit.GetSelectionInRect(from, to, ld.screen.selected_components, ld.screen.selected_tracks);
	  ld.screen.selecting = false;
	}
      else if (ld.screen.dragging_selection)
	{
	  for (auto it = ld.screen.selected_tracks.begin(); it != ld.screen.selected_tracks.end(); ++it)
	    (*it)->Move(npos);
	  for (auto it = ld.screen.selected_components.begin(); it != ld.screen.selected_components.end(); ++it)
	    (*it)->Move(npos);
	  ld.screen.dragging_selection = false;
	}
      else if (ld.screen.grabbed)
	ld.screen.grabbed->Move(npos);
      ld.screen.grabbed = NULL;

      if (ld.screen.grabbed_step != ld.circuit.EndLinkStep())
	ld.screen.grabbed_step.track->MoveNode(ld.screen.grabbed_step.node, npos);
      ld.screen.grabbed_step = ld.circuit.EndLinkStep();
    }

  if (sym == BMB_RIGHT && state == GO_UP)
    {
      hbs::IComponent *unique;

      if (ic && ld.screen.selected_components.find(ic) == ld.screen.selected_components.end())
	select_component(ld.screen, ic);
      else if (track && ld.screen.selected_tracks.find(track) == ld.screen.selected_tracks.end())
	select_track(ld.screen, track);
      unique = ld.screen.GetUniqueSelection();
      if (unique)
	{
	  ld.screen.context_menu = true;
	  ld.screen.context_pos = *bunny_get_mouse_position();
	}
    }

  return (GO_ON);
}
