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
#include                "Integrated.hpp"
#include "ComponentDockUI.hpp"

static bool		ctrl_pressed(void)
{
  return (bunny_get_keyboard()[BKS_LCONTROL] || bunny_get_keyboard()[BKS_RCONTROL]);
}

static void		clear_selection(hbs::Screen &screen)
{
  screen.selected_components.clear();
  screen.selected_tracks.clear();
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
  clear_selection(ld.screen);
  ld.screen.grabbed = NULL;
  ld.screen.grabbed_step = hbs::Packet();
  ld.screen.selecting = ld.screen.dragging_selection = false;
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
try
{
  if(hbs::ComponentInterfaceKey(state,sym,ld))return GO_ON;
  if(hbs::IntegratedKey(state,sym,ld))return ld.quit_requested?EXIT_ON_SUCCESS:GO_ON;
  if(!ld.file_browser && state==GO_DOWN && ctrl_pressed() && sym==BKS_TAB && ld.HasDocument()){
    bool shift=bunny_get_keyboard()[BKS_LSHIFT] || bunny_get_keyboard()[BKS_RSHIFT];
    ld.SelectDocument((ld.active_document+(shift?ld.documents.size()-1:1))%ld.documents.size());return GO_ON;
  }
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
      ld.screen.grabbed = NULL;
      ld.screen.grabbed_step = hbs::Packet();
      ld.screen.dragging_selection = ld.screen.selecting = false;
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

  if (sym == BKS_PAGEUP || sym == BKS_PAGEDOWN)
    {
      int turns = sym == BKS_PAGEUP ? -1 : 1;
      bool changed = false;
      hbs::RoutingSnapshot routing;
      if (ld.screen.auto_routing && !ld.screen.placing_component)
        {
          auto excluded = ld.screen.selected_tracks;
          if (ld.screen.active_track) excluded.insert(ld.screen.active_track);
          routing = ld.CurrentCircuit().CaptureRouting(ld.screen.selected_components, excluded);
        }
      if (ld.screen.placing_component && ld.screen.component_to_place)
        {
          ld.screen.component_to_place->Rotate(turns);
          changed = true;
        }
      else
        for (hbs::IComponent *component : ld.screen.selected_components)
          {
            component->Rotate(turns);
            changed = true;
          }
      if (changed)
        {
          ld.screen.context_menu = false;
          ld.MarkDirty();
          ld.CurrentCircuit().Reroute(routing);
        }
      return GO_ON;
    }

  if (sym == BKS_T)
    ld.screen.loopsim = false;
  if (sym == BKS_N)
    hbs::Command("simulate", ld.CurrentCircuit(), ld.CurrentTimer());
  if (sym == BKS_R)
    ld.screen.loopsim = true;

  if (sym == BKS_TAB || sym == BKS_C)
    {
      hbs::ToggleComponentMenu(ld);
      return (GO_ON);
    }
  return (GO_ON);
}
catch (const std::exception &error)
{
  hbs::ReportScreenError(ld, error);
  return (GO_ON);
}
