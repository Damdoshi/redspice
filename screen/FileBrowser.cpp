// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<algorithm>
#include		<cctype>
#include		<chrono>
#include		<filesystem>
#include		<sstream>
#include		"Screen.hpp"
#include		"Shortcuts.hpp"

static std::string	basename(const std::string &path)
{
  std::filesystem::path p(path);
  std::string		name = p.filename().string();

  if (name.empty())
    return (path);
  return (name);
}

static std::string	shorten(const std::string &text, size_t max)
{
  if (text.size() <= max)
    return (text);
  if (max <= 3)
    return (text.substr(0, max));
  return (text.substr(0, max - 3) + "...");
}

static bool		save_mode(FileBrowserMode mode)
{
  return (mode == FILE_BROWSER_SAVE_ACTIVE || mode == FILE_BROWSER_SAVE_COPY ||
	  mode == FILE_BROWSER_SAVE_THEN_CLOSE || mode == FILE_BROWSER_SAVE_THEN_QUIT);
}

static const char	*mode_title(FileBrowserMode mode)
{
  if (mode == FILE_BROWSER_SAVE_ACTIVE)
    return ("SAVE");
  if (mode == FILE_BROWSER_SAVE_COPY)
    return ("SAVE COPY");
  if (mode == FILE_BROWSER_SAVE_THEN_CLOSE)
    return ("SAVE BEFORE CLOSE");
  if (mode == FILE_BROWSER_SAVE_THEN_QUIT)
    return ("SAVE BEFORE QUIT");
  return ("OPEN");
}

static int		visible_rows(const hbs::Screen &screen)
{
  int rows = (screen.pic->buffer.height - 142) / 24;

  return (std::max(1, rows));
}

static void		clamp_offset(int &offset, int count, int rows)
{
  int maxoff = std::max(0, count - rows);

  if (offset < 0)
    offset = 0;
  if (offset > maxoff)
    offset = maxoff;
}

static hbs::Position	fixed_pos(const hbs::Screen &screen, int x, int y)
{
  return (hbs::Position{(double)x - screen.pic->buffer.width / 2.0,
			(double)y - screen.pic->buffer.height / 2.0});
}

static void		fixed_text(hbs::Screen &screen,
				   int x,
				   int y,
				   hbs::Size size,
				   unsigned int color,
				   const std::string &text)
{
  screen.Text(fixed_pos(screen, x, y), size, color, text);
}

static void		fixed_square(hbs::Screen &screen,
				     int x,
				     int y,
				     int w,
				     int h,
				     unsigned int color,
				     bool full)
{
  screen.Square(fixed_pos(screen, x, y), {(double)w, (double)h}, color, full);
}

static void		draw_entry(hbs::Screen &screen,
				   int x,
				   int y,
				   int w,
				   const std::string &text,
				   bool current,
				   bool directory)
{
  if (current)
    {
      fixed_square(screen, x - 4, y - 2, w - 8, 23, ALPHA(90, GRAY(96)), true);
      fixed_square(screen, x - 4, y - 2, w - 8, 23, hbs::Screen::Yellow, false);
    }
  fixed_text(screen, x + 8, y, {10, 16}, directory ? hbs::Screen::Teal : hbs::Screen::White,
	     shorten(text, std::max(4, w / 9)));
}

static void		draw_left_panel(LoopData &ld, int x, int y, int w, int h)
{
  hbs::Screen &screen = ld.screen;
  int rows = visible_rows(screen);

  clamp_offset(ld.browser_offset, (int)ld.browser_entries.size(), rows);
  fixed_square(screen, x, y, w, h, ALPHA(190, GRAY(20)), true);
  fixed_square(screen, x, y, w, h, hbs::Screen::Red, false);
  fixed_text(screen, x + 12, y + 10, {15, 22}, hbs::Screen::Red, "FILESYSTEM");
  fixed_text(screen, x + 12, y + 44, {9, 14}, hbs::Screen::White,
	     shorten(ld.browser_directory, std::max(10, w / 8)));
  for (int row = 0; row < rows; ++row)
    {
      int idx = ld.browser_offset + row;

      if (idx >= (int)ld.browser_entries.size())
	break ;
      draw_entry(screen, x + 10, y + 84 + row * 24, w - 20,
		 ld.browser_entries[idx].name, false, ld.browser_entries[idx].directory);
    }
}

static void		draw_right_panel(LoopData &ld, int x, int y, int w, int h)
{
  hbs::Screen &screen = ld.screen;
  int rows = visible_rows(screen);

  clamp_offset(ld.opened_offset, (int)ld.documents.size(), rows);
  fixed_square(screen, x, y, w, h, ALPHA(190, GRAY(20)), true);
  fixed_square(screen, x, y, w, h, hbs::Screen::Red, false);
  fixed_text(screen, x + 12, y + 10, {15, 22}, hbs::Screen::Red, "OPEN FILES");
  fixed_text(screen, x + 12, y + 44, {9, 14}, hbs::Screen::White,
	     save_mode(ld.file_browser_mode) ?
	     "clic: choisir ce chemin   double-clic: ecraser avec ce chemin" :
	     "clic: afficher   double-clic: afficher et fermer ce menu");
  if (ld.documents.empty())
    fixed_text(screen, x + 20, y + 86, {10, 16}, hbs::Screen::Yellow, "Aucun fichier ouvert.");
  for (int row = 0; row < rows; ++row)
    {
      int idx = ld.opened_offset + row;

      if (idx >= (int)ld.documents.size())
	break ;
      std::string name = basename(ld.documents[idx]->file_name);
      if (ld.documents[idx]->temporary)
	name = "*" + name;
      else if (ld.documents[idx]->dirty)
	name = name + " *";
      draw_entry(screen, x + 10, y + 84 + row * 24, w - 20,
		 name, idx == (int)ld.active_document, false);
      if (idx == (int)ld.active_document)
	fixed_text(screen, x + w - 80, y + 84 + row * 24, {8, 13}, hbs::Screen::Yellow, "VISIBLE");
    }
}

static void		draw_save_prompt(LoopData &ld, int margin, int y, int w)
{
  if (!save_mode(ld.file_browser_mode))
    return ;
  hbs::Screen &screen = ld.screen;
  std::string prompt = std::string("Nom: ") + ld.browser_target + "_";

  fixed_square(screen, margin, y, w, 54, ALPHA(205, GRAY(30)), true);
  fixed_square(screen, margin, y, w, 54, hbs::Screen::Yellow, false);
  fixed_text(screen, margin + 10, y + 7, {10, 16}, hbs::Screen::Yellow,
	     "Entree: sauvegarder   Shift+Entree: ne pas sauvegarder   clic fichier: nom   double-clic: ecraser");
  fixed_text(screen, margin + 10, y + 30, {12, 18}, hbs::Screen::White,
	     shorten(prompt, std::max(20, w / 9)));
}

void			hbs::DrawFileBrowser(LoopData &ld)
{
  hbs::Screen &screen = ld.screen;
  hbs::Position oldcam = screen.camera;
  int oldpinsize = screen.pin_size;
  int w = screen.pic->buffer.width;
  int h = screen.pic->buffer.height;
  int margin = 24;
  int gap = 18;
  int top = hbs::ShortcutBarHeight + 10;
  int save_h = save_mode(ld.file_browser_mode) ? 66 : 0;
  int panel_w = (w - margin * 2 - gap) / 2;

  bunny_clear(&screen.pic->buffer, BLACK);
  screen.pin_size = 1;
  screen.camera = {0, 0};
  hbs::DrawShortcutBar(screen, &ld);
  fixed_text(screen, margin, top, {18, 28}, hbs::Screen::Red,
	     std::string("BUNNY CAD FILE SELECTOR - ") + mode_title(ld.file_browser_mode));
  fixed_text(screen, w - 610, top + 6, {9, 14}, hbs::Screen::White,
	     "F1:new  F3:open  F4:menu  Shift+F1:close  F2:save  Shift+F2:save copy");
  draw_save_prompt(ld, margin, top + 38, w - margin * 2);
  draw_left_panel(ld, margin, top + 58 + save_h, panel_w, h - top - 82 - save_h);
  draw_right_panel(ld, margin + panel_w + gap, top + 58 + save_h, panel_w, h - top - 82 - save_h);
  if (!ld.browser_error.empty())
    fixed_text(screen, margin, h - 20, {9, 14}, hbs::Screen::Yellow, shorten(ld.browser_error, 120));
  screen.camera = oldcam;
  screen.pin_size = oldpinsize;
  bunny_blit(&screen.win->buffer, screen.pic, NULL);
  bunny_display(screen.win);
}

static bool		shift_pressed(void)
{
  return (bunny_get_keyboard()[BKS_LSHIFT] || bunny_get_keyboard()[BKS_RSHIFT]);
}

bool			hbs::FileBrowserKey(t_bunny_keysym sym, LoopData &ld)
{
  int rows = visible_rows(ld.screen);

  if (save_mode(ld.file_browser_mode))
    {
      if (sym == BKS_RETURN)
	return (ld.CompleteSavePrompt(shift_pressed()), true);
      if (sym == BKS_BACKSPACE && !ld.browser_target.empty())
	ld.browser_target.erase(ld.browser_target.size() - 1);
    }
  if (sym == BKS_UP)
    ld.browser_offset -= 1;
  if (sym == BKS_DOWN)
    ld.browser_offset += 1;
  if (sym == BKS_PAGEUP)
    ld.browser_offset -= rows;
  if (sym == BKS_PAGEDOWN)
    ld.browser_offset += rows;
  clamp_offset(ld.browser_offset, (int)ld.browser_entries.size(), rows);
  return (true);
}

bool			hbs::FileBrowserWheel(int delta, LoopData &ld)
{
  const t_bunny_position *mouse = bunny_get_mouse_position();
  int rows = visible_rows(ld.screen);

  if (mouse->x < ld.screen.pic->buffer.width / 2)
    {
      ld.browser_offset -= delta * 3;
      clamp_offset(ld.browser_offset, (int)ld.browser_entries.size(), rows);
    }
  else
    {
      ld.opened_offset -= delta * 3;
      clamp_offset(ld.opened_offset, (int)ld.documents.size(), rows);
    }
  return (true);
}

bool			hbs::FileBrowserClick(t_bunny_event_state state,
					      t_bunny_mouse_button sym,
					      LoopData &ld)
{
  static int		last_side = -1;
  static int		last_index = -1;
  static std::chrono::steady_clock::time_point last_click;
  t_bunny_position	pos = *bunny_get_mouse_position();
  int			w = ld.screen.pic->buffer.width;
  int			margin = 24;
  int			gap = 18;
  int			top = hbs::ShortcutBarHeight + 10;
  int			save_h = save_mode(ld.file_browser_mode) ? 66 : 0;
  int			panel_w = (w - margin * 2 - gap) / 2;
  int			left_x = margin;
  int			right_x = margin + panel_w + gap;
  int			panel_y = top + 58 + save_h;
  int			row;
  int			idx;

  if (state != GO_DOWN || sym != BMB_LEFT)
    return (true);
  row = (pos.y - (panel_y + 84)) / 24;
  if (pos.y < panel_y + 84 || row < 0 || row >= visible_rows(ld.screen))
    return (true);

  std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
  long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_click).count();
  bool dbl;

  if (pos.x >= left_x && pos.x < left_x + panel_w)
    {
      idx = ld.browser_offset + row;
      dbl = last_side == 0 && last_index == idx && elapsed < 420;
      last_side = 0;
      last_index = idx;
      last_click = now;
      if (idx < 0 || idx >= (int)ld.browser_entries.size())
	return (true);
      if (ld.browser_entries[idx].directory)
	{
	  if (!dbl)
	    return (true);
	  ld.browser_directory = ld.browser_entries[idx].path;
	  ld.browser_offset = 0;
	  ld.RefreshBrowser();
	}
      else if (save_mode(ld.file_browser_mode))
	{
	  ld.browser_target = basename(ld.browser_entries[idx].path);
	  if (dbl)
	    ld.CompleteSavePrompt(false);
	}
      else if (dbl)
	ld.OpenFile(ld.browser_entries[idx].path);
      return (true);
    }
  if (pos.x >= right_x && pos.x < right_x + panel_w)
    {
      idx = ld.opened_offset + row;
      dbl = last_side == 1 && last_index == idx && elapsed < 420;
      last_side = 1;
      last_index = idx;
      last_click = now;
      if (idx < 0 || idx >= (int)ld.documents.size())
	return (true);
      if (save_mode(ld.file_browser_mode))
	{
	  ld.browser_target = ld.documents[idx]->file_name;
	  if (dbl)
	    ld.CompleteSavePrompt(false);
	}
      else
	{
	  ld.SelectDocument((size_t)idx);
	  if (dbl)
	    ld.file_browser = false;
	}
      return (true);
    }
  return (true);
}
