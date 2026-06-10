// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<algorithm>
#include		<chrono>
#include		<filesystem>
#include		<sstream>
#include		"Screen.hpp"

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

static int		visible_rows(const hbs::Screen &screen)
{
  int rows = (screen.pic->buffer.height - 118) / 24;

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
	     "clic: afficher   double-clic: afficher et fermer ce menu");
  for (int row = 0; row < rows; ++row)
    {
      int idx = ld.opened_offset + row;

      if (idx >= (int)ld.documents.size())
	break ;
      draw_entry(screen, x + 10, y + 84 + row * 24, w - 20,
		 basename(ld.documents[idx]->file_name), idx == (int)ld.active_document, false);
      if (idx == (int)ld.active_document)
	fixed_text(screen, x + w - 80, y + 84 + row * 24, {8, 13}, hbs::Screen::Yellow, "VISIBLE");
    }
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
  int panel_w = (w - margin * 2 - gap) / 2;

  bunny_clear(&screen.pic->buffer, BLACK);
  screen.pin_size = 1;
  screen.camera = {0, 0};
  fixed_text(screen, margin, 12, {18, 28}, hbs::Screen::Red, "BUNNY CAD FILE SELECTOR");
  fixed_text(screen, w - 430, 18, {9, 14}, hbs::Screen::White,
	     "F9/Echap: retour CAO   molette: defiler");
  draw_left_panel(ld, margin, 58, panel_w, h - 82);
  draw_right_panel(ld, margin + panel_w + gap, 58, panel_w, h - 82);
  if (!ld.browser_error.empty())
    fixed_text(screen, margin, h - 20, {9, 14}, hbs::Screen::Yellow, shorten(ld.browser_error, 120));
  screen.camera = oldcam;
  screen.pin_size = oldpinsize;
  bunny_blit(&screen.win->buffer, screen.pic, NULL);
  bunny_display(screen.win);
}

bool			hbs::FileBrowserKey(t_bunny_keysym sym, LoopData &ld)
{
  int rows = visible_rows(ld.screen);

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
  int			panel_w = (w - margin * 2 - gap) / 2;
  int			left_x = margin;
  int			right_x = margin + panel_w + gap;
  int			row;
  int			idx;

  if (state != GO_DOWN || sym != BMB_LEFT)
    return (true);
  row = (pos.y - (58 + 84)) / 24;
  if (pos.y < 58 + 84 || row < 0 || row >= visible_rows(ld.screen))
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
      if (!dbl || idx < 0 || idx >= (int)ld.browser_entries.size())
	return (true);
      if (ld.browser_entries[idx].directory)
	{
	  ld.browser_directory = ld.browser_entries[idx].path;
	  ld.browser_offset = 0;
	  ld.RefreshBrowser();
	}
      else
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
      ld.SelectDocument((size_t)idx);
      if (dbl)
	ld.file_browser = false;
      return (true);
    }
  return (true);
}
