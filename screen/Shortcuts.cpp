// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<algorithm>
#include		<cstdio>
#include		"Circuit.hpp"
#include		"Screen.hpp"
#include		"Shortcuts.hpp"
#include		"Track.hpp"
#include                "Integrated.hpp"
#include "ComponentDockUI.hpp"

static void		shortcut_escape(LoopData &ld)
{
  if(ld.screen.search_panel || !ld.pending_cell.empty() || ld.dock_pressed>=0){hbs::CloseComponentMenu(ld);ld.pending_cell.clear();ld.dock_pressed=-1;ld.dock_dragged=false;return;}
  if (ld.file_browser && ld.HasDocument())
    {
      ld.file_browser = false;
      ld.file_browser_mode = FILE_BROWSER_OPEN;
      ld.browser_target = "";
      ld.browser_error = "";
      return ;
    }
  ld.RequestQuit();
}

static void		shortcut_new(LoopData &ld)
{
  ld.NewDocument();
}
static void shortcut_new_ic(LoopData &ld){ld.NewDocument(BufferType::IntegratedCircuit);}
static void shortcut_insert_ic(LoopData &ld){if(!ld.HasDocument())return;ld.file_browser=true;ld.file_browser_mode=FILE_BROWSER_INSERT;ld.RefreshBrowser();}
static void shortcut_inspect_ic(LoopData &ld){ld.InspectSelected();}
static void shortcut_source_ic(LoopData &ld){ld.OpenSelectedDefinition();}

static void		shortcut_close(LoopData &ld)
{
  ld.CloseCurrentDocument();
}

static void		shortcut_save(LoopData &ld)
{
  ld.SaveCurrentDocument();
}

static void		shortcut_save_as(LoopData &ld)
{
  ld.BeginSaveAs(false);
}

static void		shortcut_open(LoopData &ld)
{
  ld.BeginOpenFileBrowser();
}

static void shortcut_save_copy(LoopData &ld){ld.BeginSaveAs(true);}

static void		shortcut_center_on_mouse(LoopData &ld)
{
  if (!ld.HasDocument())
    return ;
  t_bunny_position	center =
    {
      static_cast<int>(ld.screen.pic->buffer.width / 2),
      static_cast<int>(ld.screen.pic->buffer.height / 2)
    };

  if(ld.HasIntegratedCircuit()){auto &editor=ld.CurrentIntegratedCircuit();auto p=editor.World(bunny_get_mouse_position()->x-hbs::ComponentDockWidth(ld.screen),bunny_get_mouse_position()->y);editor.center_x=p.x;editor.center_y=p.y;bunny_set_mouse_position_window(ld.screen.win,hbs::ComponentDockWidth(ld.screen)+(180+editor.width-320)/2,(100+editor.height-190)/2);return;}
  ld.screen.context_menu = false;
  ld.screen.camera = ld.screen.ScreenToWorld(*bunny_get_mouse_position());
  bunny_set_mouse_position_window(ld.screen.win, center.x, center.y);
  ld.MarkDirty();
}

static void		shortcut_home(LoopData &ld)
{
  if (!ld.HasDocument())
    return ;
  ld.screen.context_menu = false;
  if(ld.HasIntegratedCircuit()){ld.CurrentIntegratedCircuit().Fit();return;}
  ld.screen.ResetZoom(*bunny_get_mouse_position());
  ld.MarkDirty();
}

static void		shortcut_zoom_in(LoopData &ld)
{
  if (!ld.HasDocument())
    return ;
  ld.screen.context_menu = false;
  if(ld.HasIntegratedCircuit()){ld.CurrentIntegratedCircuit().Wheel(bunny_get_mouse_position()->x-hbs::ComponentDockWidth(ld.screen),bunny_get_mouse_position()->y,+2);return;}
  ld.screen.ZoomAt(*bunny_get_mouse_position(), +2);
  ld.MarkDirty();
}

static void		shortcut_zoom_out(LoopData &ld)
{
  if (!ld.HasDocument())
    return ;
  ld.screen.context_menu = false;
  if(ld.HasIntegratedCircuit()){ld.CurrentIntegratedCircuit().Wheel(bunny_get_mouse_position()->x-hbs::ComponentDockWidth(ld.screen),bunny_get_mouse_position()->y,-2);return;}
  ld.screen.ZoomAt(*bunny_get_mouse_position(), -2);
  ld.MarkDirty();
}

static void		shortcut_add(LoopData &ld)
{
  if (!ld.HasDocument())
    {
      ld.BeginFileMenu();
      return ;
    }
  hbs::ToggleComponentMenu(ld);
}

static void		shortcut_select_pin(LoopData &ld)
{
  if (!ld.HasDocument())
    return ;
  ld.screen.drawing_mode = !ld.screen.drawing_mode;
  ld.screen.context_menu = false;
  ld.screen.selecting = false;
  ld.screen.dragging_selection = false;
  if (!ld.screen.drawing_mode)
    {
      ld.screen.active_track = NULL;
      ld.screen.active_node = hbs::Track::NoNode;
    }
}

static void		shortcut_step(LoopData &ld)
{
  if (!ld.HasDocument())
    return ;
  hbs::Command("simulate", ld.CurrentCircuit(), ld.CurrentTimer());
}

static void shortcut_auto_route(LoopData &ld)
{
  ld.screen.auto_routing = !ld.screen.auto_routing;
}

static void		shortcut_run_stop(LoopData &ld)
{
  if (!ld.HasDocument())
    return ;
  ld.screen.loopsim = !ld.screen.loopsim;
}

static const hbs::ShortcutBinding g_escape = {"QUIT", shortcut_escape};

static const hbs::ShortcutBinding g_shortcuts[hbs::SHORTCUT_MODE_COUNT][hbs::ShortcutFunctionKeyCount] =
  {
    {
      {"NEW CIRCUIT", shortcut_new},
      {"NEW CHIPSET", shortcut_new_ic},
      {"SAVE", shortcut_save},
      {"OPEN", shortcut_open},
      {"CENTER", shortcut_center_on_mouse},
      {"HOME", shortcut_home},
      {"ZOOM IN", shortcut_zoom_in},
      {"ZOOM OUT", shortcut_zoom_out},
      {"COMPONENT", shortcut_add},
      {"SELECT/PIN", shortcut_select_pin},
      {"STEP", shortcut_step},
      {"RUN/STOP", shortcut_run_stop}
    },
    {
      {"NEW IC", shortcut_new_ic}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL},
      {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL},
      {"INSERT IC", shortcut_insert_ic}, {"AUTO", shortcut_auto_route}, {NULL, NULL}, {NULL, NULL}
    },
    {
      {"CLOSE", shortcut_close},
      {NULL, NULL},
      {"SAVE AS", shortcut_save_as}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL},
      {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL},
      {NULL, NULL}, {NULL, NULL}
    },
    {
      {NULL, NULL}, {NULL, NULL}, {"SAVE COPY", shortcut_save_copy}, {NULL, NULL},
      {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL},
      {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL}
    }
  };

static bool		control_pressed(void)
{
  return (bunny_get_keyboard()[BKS_LCONTROL] || bunny_get_keyboard()[BKS_RCONTROL]);
}

static bool		shift_pressed(void)
{
  return (bunny_get_keyboard()[BKS_LSHIFT] || bunny_get_keyboard()[BKS_RSHIFT]);
}

hbs::ShortcutMode	hbs::CurrentShortcutMode(void)
{
  bool			ctrl = control_pressed();
  bool			shift = shift_pressed();

  if (ctrl && shift)
    return (SHORTCUT_CONTROL_SHIFT);
  if (ctrl)
    return (SHORTCUT_CONTROL);
  if (shift)
    return (SHORTCUT_SHIFT);
  return (SHORTCUT_NORMAL);
}

unsigned int		hbs::ShortcutModeColor(ShortcutMode mode)
{
  if (mode == SHORTCUT_CONTROL)
    return (hbs::Screen::Blue);
  if (mode == SHORTCUT_SHIFT)
    return (hbs::Screen::Green);
  if (mode == SHORTCUT_CONTROL_SHIFT)
    return (hbs::Screen::White);
  return (hbs::Screen::Red);
}

int			hbs::FunctionKeyIndex(t_bunny_keysym sym)
{
  if (sym == BKS_F1)
    return (0);
  if (sym == BKS_F2)
    return (1);
  if (sym == BKS_F3)
    return (2);
  if (sym == BKS_F4)
    return (3);
  if (sym == BKS_F5)
    return (4);
  if (sym == BKS_F6)
    return (5);
  if (sym == BKS_F7)
    return (6);
  if (sym == BKS_F8)
    return (7);
  if (sym == BKS_F9)
    return (8);
  if (sym == BKS_F10)
    return (9);
  if (sym == BKS_F11)
    return (10);
  if (sym == BKS_F12)
    return (11);
  return (-1);
}

const hbs::ShortcutBinding	&hbs::GetShortcutBinding(ShortcutMode mode,
					      int key_index)
{
  static const hbs::ShortcutBinding empty = {NULL, NULL};

  if (mode < 0 || mode >= SHORTCUT_MODE_COUNT)
    return (empty);
  if (key_index < 0 || key_index >= ShortcutFunctionKeyCount)
    return (empty);
  return (g_shortcuts[mode][key_index]);
}

bool			hbs::TriggerShortcut(t_bunny_keysym sym,
				     LoopData &ld)
{
  int			idx = hbs::FunctionKeyIndex(sym);
  auto mode=hbs::CurrentShortcutMode();
  if(!ld.file_browser && ld.HasIntegratedCircuit() && (mode==SHORTCUT_NORMAL || mode==SHORTCUT_CONTROL) && hbs::IntegratedShortcut(idx,mode==SHORTCUT_CONTROL,ld))return true;
  if(!ld.file_browser && !ld.HasIntegratedCircuit() && mode==SHORTCUT_CONTROL_SHIFT && idx==9){shortcut_inspect_ic(ld);return true;}
  if(!ld.file_browser && !ld.HasIntegratedCircuit() && mode==SHORTCUT_CONTROL_SHIFT && idx==10){shortcut_source_ic(ld);return true;}
  const hbs::ShortcutBinding &binding = hbs::GetShortcutBinding(hbs::CurrentShortcutMode(), idx);

  if (idx < 0 || binding.action == NULL)
    return (false);
  binding.action(ld);
  return (true);
}

bool			hbs::TriggerEscapeShortcut(LoopData &ld)
{
  if (g_escape.action == NULL)
    return (false);
  g_escape.action(ld);
  return (true);
}

bool			hbs::HandleShortcutBarClick(t_bunny_event_state state,
					     t_bunny_mouse_button sym,
					     t_bunny_position pos,
					     LoopData &ld)
{
  int			width = ld.screen.pic->buffer.width;
  int			idx;

  if (pos.y < 0 || pos.y >= hbs::ShortcutBarHeight || pos.x < 0 || pos.x >= width)
    return (false);
  if (state == GO_UP && sym == BMB_LEFT)
    {
      idx = std::min(hbs::ShortcutBarCellCount - 1, pos.x * hbs::ShortcutBarCellCount / width);
      if (idx == 0)
	hbs::TriggerEscapeShortcut(ld);
      else
	{
          const t_bunny_keysym keys[]={BKS_F1,BKS_F2,BKS_F3,BKS_F4,BKS_F5,BKS_F6,BKS_F7,BKS_F8,BKS_F9,BKS_F10,BKS_F11,BKS_F12};
          hbs::TriggerShortcut(keys[idx-1],ld);
	}
    }
  return (true);
}

static void		draw_fixed_text(hbs::Screen &screen,
					double x,
					double y,
					hbs::Size size,
					unsigned int color,
					const std::string &text)
{
  screen.Text({x - screen.pic->buffer.width / 2.0,
	       y - screen.pic->buffer.height / 2.0}, size, color, text);
}

static void		draw_centered_text(hbs::Screen &screen,
					   double x,
					   double y,
					   double width,
					   hbs::Size size,
					   unsigned int color,
					   const std::string &text)
{
  double		text_width = screen.TextSize(size, text).x;

  draw_fixed_text(screen, x + std::max(0.0, (width - text_width) / 2.0), y, size, color, text);
}

static void		draw_one_shortcut(hbs::Screen &screen,
					  double x,
					  double cell,
					  const hbs::ShortcutBinding &binding,
					  const std::string &keyname,
					  unsigned int color)
{
  unsigned int		text_color = binding.action != NULL ? color : GRAY(96);

  if (binding.action != NULL)
    screen.Square({x - screen.pic->buffer.width / 2.0,
		   -screen.pic->buffer.height / 2.0},
		  {cell, (double)hbs::ShortcutBarHeight}, ALPHA(38, color), true);
  screen.Square({x - screen.pic->buffer.width / 2.0,
		 -screen.pic->buffer.height / 2.0},
		{cell, (double)hbs::ShortcutBarHeight}, text_color, false);
  const std::string label=binding.label?binding.label:"...";
  hbs::Size label_size={8,13};double label_width=screen.TextSize(label_size,label).x;
  if(label_width>cell-8 && label_width>0)label_size.x=std::max(3.,label_size.x*std::max(1.,cell-8)/label_width);
  draw_centered_text(screen,x,7,cell,label_size,text_color,label);
  draw_centered_text(screen, x, 29, cell, {6, 10}, text_color, keyname);
}

void			hbs::DrawShortcutBar(Screen &screen, const LoopData *ld)
{
  hbs::ShortcutMode	mode = hbs::CurrentShortcutMode();
  unsigned int		color = hbs::ShortcutModeColor(mode);
  int			width = screen.pic->buffer.width;
  double		cell = width / (double)hbs::ShortcutBarCellCount;
  hbs::ShortcutBinding escape = g_escape;

  if (ld != NULL && ld->HasDocument() && (ld->file_browser || ld->screen.search_panel || !ld->pending_cell.empty() || ld->dock_pressed>=0))
    escape.label = "BACK";

  screen.Square({-screen.pic->buffer.width / 2.0,
		 -screen.pic->buffer.height / 2.0},
		{(double)width, (double)hbs::ShortcutBarHeight}, ALPHA(210, GRAY(16)), true);
  draw_one_shortcut(screen, 0, cell, escape, "ECHAP", hbs::Screen::Yellow);
  for (int i = 0; i < hbs::ShortcutFunctionKeyCount; ++i)
    {
      hbs::ShortcutBinding binding = hbs::GetShortcutBinding(mode, i);
      if(ld && !ld->file_browser && ld->HasIntegratedCircuit() && (mode==SHORTCUT_NORMAL || mode==SHORTCUT_CONTROL)){
        const char *label=hbs::IntegratedShortcutLabel(i,mode==SHORTCUT_CONTROL);
        if(i>=4 && label){binding.label=label;binding.action=shortcut_inspect_ic;}
      }
      if(ld && !ld->file_browser && !ld->HasIntegratedCircuit() && mode==SHORTCUT_CONTROL_SHIFT && (i==9 || i==10)){binding.label=i==9?"INSPECT IC":"SOURCE IC";binding.action=shortcut_inspect_ic;}
      if (binding.action == shortcut_auto_route)
        binding.label = screen.auto_routing ? "AUTO ON" : "AUTO OFF";
      char		keyname[8];

      snprintf(keyname, sizeof(keyname), "F%d", i + 1);
      draw_one_shortcut(screen, (i + 1) * cell, cell, binding, keyname, color);
    }
}
