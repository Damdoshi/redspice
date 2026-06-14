// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<algorithm>
#include		<cmath>
#include		<set>
#include		"Circuit.hpp"
#include		"Screen.hpp"
#include		"Shortcuts.hpp"
#include		"ComponentMenu.hpp"
#include		"Track.hpp"

static bool		ctrl_pressed(void)
{
  return (bunny_get_keyboard()[BKS_LCONTROL] || bunny_get_keyboard()[BKS_RCONTROL]);
}

static std::string	current_menu_choice(const LoopData &ld)
{
  const std::vector<std::string> &types = ld.CurrentCircuit().GetCreatableTypes();
  int			seen = 0;

  for (size_t i = 0; i < types.size(); ++i)
    {
      if (!hbs::ComponentMatchesQuery(types[i], ld.screen.search_query))
	continue ;
      if (seen++ < ld.screen.search_offset)
	continue ;
      return (types[i]);
    }
  return ("");
}

static hbs::Position	snap_world(const hbs::Position &p)
{
  return (hbs::Position{(double)round(p.x), (double)round(p.y)});
}

static hbs::Position	snapped_mouse(hbs::Screen &screen)
{
  return (snap_world(screen.ScreenToWorld(*bunny_get_mouse_position())));
}

static void		clear_selection(hbs::Screen &screen)
{
  screen.selected_components.clear();
  screen.selected_tracks.clear();
}

static void		begin_component_placement(LoopData &ld, const std::string &type)
{
  if (type.empty())
    return ;
  hbs::Position pos = snapped_mouse(ld.screen);
  hbs::IComponent *cmp = ld.CurrentCircuit().CreateUserComponent(type, pos);

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
  ld.CurrentCircuit().DeleteSelected(components, tracks);
  ld.screen.placing_component = false;
  ld.screen.component_to_place = NULL;
  ld.screen.placing_type = "";
}

static void		place_via(LoopData &ld)
{
  if (ld.screen.active_track == NULL || ld.screen.active_node == hbs::Track::NoNode)
    return ;
  hbs::ILink::Layer oldlayer = ld.screen.active_track->GetNodeLayer(ld.screen.active_node);
  hbs::ILink::Layer newlayer = oldlayer == hbs::ILink::TOP ? hbs::ILink::BOTTOM : hbs::ILink::TOP;
  hbs::Position pos = ld.screen.active_track->GetNodePosition(ld.screen.active_node);

  ld.screen.active_node = ld.screen.active_track->AddNodeAfter(ld.screen.active_node, pos, newlayer);
}

extern "C"
t_bunny_response	screen_key(t_bunny_event_state	state,
				   t_bunny_keysym	sym,
				   LoopData		&ld)
{
  if (sym == BKS_ESCAPE && state == GO_UP)
    {
      if (ld.screen.search_panel)
	{
	  ld.screen.search_panel = false;
	  ld.screen.search_query = "";
	  ld.screen.search_offset = 0;
	  return (GO_ON);
	}
      hbs::TriggerEscapeShortcut(ld);
      return (ld.quit_requested ? EXIT_ON_SUCCESS : GO_ON);
    }
  if (hbs::FunctionKeyIndex(sym) >= 0 && state == GO_UP)
    {
      hbs::TriggerShortcut(sym, ld);
      return (ld.quit_requested ? EXIT_ON_SUCCESS : GO_ON);
    }
  if (ld.file_browser)
    {
      if (state == GO_DOWN)
	hbs::FileBrowserKey(sym, ld);
      return (ld.quit_requested ? EXIT_ON_SUCCESS : GO_ON);
    }
  if (!ld.HasDocument())
    {
      ld.BeginFileMenu();
      return (GO_ON);
    }
  if (state == GO_UP)
    return (GO_ON);

  if (ld.screen.rename_mode)
    {
      if (sym == BKS_RETURN)
	{
	  hbs::IComponent *unique = ld.screen.GetUniqueSelection();
	  if (unique)
	    {
	      ld.CurrentCircuit().RenameComponent(unique, ld.screen.rename_buffer);
	      ld.MarkDirty();
	    }
	  ld.screen.rename_mode = false;
	}
      if (sym == BKS_BACKSPACE && !ld.screen.rename_buffer.empty())
	ld.screen.rename_buffer.erase(ld.screen.rename_buffer.size() - 1);
      return (GO_ON);
    }

  if (sym == BKS_DELETE)
    {
      if (ld.screen.placing_component)
	cancel_component_placement(ld);
      else
	{
	  ld.CurrentCircuit().DeleteSelected(ld.screen.selected_components, ld.screen.selected_tracks);
	  ld.MarkDirty();
	}
      ld.screen.active_track = NULL;
      ld.screen.active_node = hbs::Track::NoNode;
      return (GO_ON);
    }
  if (ctrl_pressed() && sym == BKS_S)
    {
      ld.SaveCurrentDocument();
      return (GO_ON);
    }
  if ((sym == BKS_ADD || sym == BKS_EQUAL))
    {
      ld.screen.context_menu = false;
      ld.screen.ZoomAt(*bunny_get_mouse_position(), +2);
      ld.MarkDirty();
    }
  if ((sym == BKS_SUBTRACT || sym == BKS_DASH))
    {
      ld.screen.context_menu = false;
      ld.screen.ZoomAt(*bunny_get_mouse_position(), -2);
      ld.MarkDirty();
    }
  if (sym == BKS_HOME)
    {
      ld.screen.context_menu = false;
      ld.screen.ResetZoom(*bunny_get_mouse_position());
      ld.MarkDirty();
    }
  if (sym == BKS_P)
    {
      ld.screen.drawing_mode = !ld.screen.drawing_mode;
      ld.screen.context_menu = false;
      ld.screen.selecting = false;
      ld.screen.dragging_selection = false;
      if (!ld.screen.drawing_mode)
	{
	  ld.screen.active_track = NULL;
	  ld.screen.active_node = hbs::Track::NoNode;
	}
      return (GO_ON);
    }

  if (sym == BKS_V && ld.screen.active_track != NULL && ld.screen.active_node != hbs::Track::NoNode)
    {
      place_via(ld);
      ld.MarkDirty();
      return (GO_ON);
    }

  if (ld.screen.search_panel)
    {
      if (sym == BKS_BACKSPACE && !ld.screen.search_query.empty())
	{
	  ld.screen.search_query.erase(ld.screen.search_query.size() - 1);
	  ld.screen.search_offset = 0;
	}
      if (sym == BKS_PAGEUP)
	ld.screen.search_offset = std::max(0, ld.screen.search_offset - 10);
      if (sym == BKS_PAGEDOWN)
	ld.screen.search_offset += 10;
      if (sym == BKS_UP)
	ld.screen.search_offset = std::max(0, ld.screen.search_offset - 1);
      if (sym == BKS_DOWN)
	ld.screen.search_offset += 1;
      if (sym == BKS_RETURN)
	{
	  std::string type = current_menu_choice(ld);
	  if (!type.empty())
	    {
	      begin_component_placement(ld, type);
	      ld.MarkDirty();
	    }
	}
      return (GO_ON);
    }

  if (sym == BKS_T)
    ld.screen.loopsim = false;
  if (sym == BKS_N)
    hbs::Command("simulate", ld.CurrentCircuit(), ld.CurrentTimer());
  if (sym == BKS_R)
    ld.screen.loopsim = true;

  if (sym == BKS_TAB || sym == BKS_C)
    {
      ld.screen.search_panel = !ld.screen.search_panel;
      ld.screen.search_offset = 0;
      return (GO_ON);
    }
  return (GO_ON);
}
