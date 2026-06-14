// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<algorithm>
#include		<cmath>
#include		<sstream>
#include		"Circuit.hpp"
#include		"Screen.hpp"
#include		"Shortcuts.hpp"
#include		"ComponentMenu.hpp"
#include		"Track.hpp"


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

static void		draw_fixed_line(hbs::Screen &screen,
					double x1,
					double y1,
					double x2,
					double y2,
					unsigned int color)
{
  screen.Line({x1 - screen.pic->buffer.width / 2.0,
	       y1 - screen.pic->buffer.height / 2.0},
	      {x2 - screen.pic->buffer.width / 2.0,
	       y2 - screen.pic->buffer.height / 2.0}, color);
}

static std::string                      truncate_to_width(hbs::Screen &screen,
                                                          hbs::Size size,
                                                          int max_width,
                                                          const std::string &text)
{
  std::string                           out = text;

  while (!out.empty() && screen.TextSize(size, out).x > max_width)
    out.erase(out.size() - 1);
  if (out.size() < text.size())
    {
      while (!out.empty() && screen.TextSize(size, out + "...").x > max_width)
        out.erase(out.size() - 1);
      out += "...";
    }
  return (out);
}

static void                             draw_wrapped_text(hbs::Screen &screen,
                                                          int x,
                                                          int y,
                                                          int max_width,
                                                          hbs::Size size,
                                                          unsigned int color,
                                                          const std::string &text)
{
  std::istringstream                    iss(text);
  std::string                           word;
  std::string                           line;
  int                                   cy = y;
  int                                   line_height = (int)size.y + 4;

  while (iss >> word)
    {
      std::string candidate = line.empty() ? word : line + " " + word;
      if (!line.empty() && screen.TextSize(size, candidate).x > max_width)
        {
          draw_fixed_text(screen, x, cy, size, color, line);
          cy += line_height;
          line = word;
        }
      else
        line = candidate;
    }
  if (!line.empty())
    draw_fixed_text(screen, x, cy, size, color, line);
}

static void                             draw_dip_preview(hbs::Screen &screen,
                                                         int x,
                                                         int y,
                                                         int w,
                                                         int h,
                                                         const std::string &type)
{
  int                                   pins = std::max(1, hbs::ComponentPinGuess(type));
  int                                   side = std::max(1, pins / 2);
  int                                   body_w = std::min(210, std::max(80, w - 90));
  int                                   body_h = std::min(190, std::max(70, side * 14));
  int                                   bx = x + (w - body_w) / 2;
  int                                   by = y + 8;
  int                                   pitch = std::max(8, body_h / (side + 1));

  screen.Square({(double)bx - screen.pic->buffer.width / 2.0,
                 (double)by - screen.pic->buffer.height / 2.0},
                {(double)body_w, (double)body_h}, ALPHA(210, GRAY(20)), true);
  screen.Square({(double)bx - screen.pic->buffer.width / 2.0,
                 (double)by - screen.pic->buffer.height / 2.0},
                {(double)body_w, (double)body_h}, hbs::Screen::White, false);
  screen.Square({(double)(bx + body_w / 2 - 14) - screen.pic->buffer.width / 2.0,
                 (double)(by - 1) - screen.pic->buffer.height / 2.0},
                {28, 10}, hbs::Screen::White, false);
  draw_fixed_text(screen, bx + body_w / 2 - screen.TextSize({14, 18}, type).x / 2,
                  by + body_h / 2 - 9, {14, 18}, hbs::Screen::Yellow, type);
  for (int i = 0; i < side; ++i)
    {
      int py = by + pitch * (i + 1);
      draw_fixed_line(screen, bx - 22, py, bx, py, hbs::Screen::White);
      draw_fixed_line(screen, bx + body_w, py, bx + body_w + 22, py, hbs::Screen::White);
      draw_fixed_text(screen, bx - 42, py - 6, {6, 10}, hbs::Screen::White, std::to_string(i + 1));
      draw_fixed_text(screen, bx + body_w + 26, py - 6, {6, 10}, hbs::Screen::White, std::to_string(pins - i));
    }
  if (pins <= 2)
    draw_fixed_text(screen, x + 10, y + h - 22, {8, 13}, hbs::Screen::White, "symbole virtuel / 1 broche logique");
  else
    draw_fixed_text(screen, x + 10, y + h - 22, {8, 13}, hbs::Screen::White,
                    "Apercu boitier DIP " + std::to_string(pins) + " broches");
}

static void                             draw_component_detail(hbs::Screen &screen,
                                                              const hbs::ComponentMenuGeometry &g,
                                                              const std::string &type)
{
  if (type.empty() || g.detail_w <= 0)
    return ;

  int                                   x = g.x + g.list_w + 10;
  int                                   y = g.first_y;
  int                                   w = g.detail_w - 10;
  int                                   h = g.h - 126;
  std::string                           short_desc = hbs::ComponentShortDescription(type);
  std::string                           long_desc = hbs::ComponentLongDescription(type);

  screen.Square({(double)x - screen.pic->buffer.width / 2.0,
                 (double)y - screen.pic->buffer.height / 2.0},
                {(double)w, (double)h}, ALPHA(150, GRAY(12)), true);
  screen.Square({(double)x - screen.pic->buffer.width / 2.0,
                 (double)y - screen.pic->buffer.height / 2.0},
                {(double)w, (double)h}, GRAY(150), false);
  draw_fixed_text(screen, x + 12, y + 8, {16, 22}, hbs::Screen::Yellow, type);
  draw_fixed_text(screen, x + 12, y + 34, {10, 15}, hbs::Screen::White,
                  truncate_to_width(screen, {10, 15}, w - 24, short_desc));
  draw_dip_preview(screen, x + 12, y + 60, w - 24, std::min(230, h / 2), type);
  draw_wrapped_text(screen, x + 12, y + std::min(310, h / 2 + 88), w - 24,
                    {9, 14}, hbs::Screen::White, long_desc);
}

static void                             draw_search_box(hbs::Screen &screen,
                                                        const hbs::ComponentMenuGeometry &g,
                                                        int match_count)
{
  std::string                           query = screen.search_query.empty() ? "tape pour chercher..." : screen.search_query + "_";
  std::string                           label = "Recherche";
  std::string                           count = std::to_string(match_count) + " resultat" + (match_count > 1 ? "s" : "");
  int                                   label_x = g.x + 20;
  int                                   text_x = label_x + screen.TextSize({10, 16}, label).x + 24;
  int                                   count_x = g.x + g.w - screen.TextSize({8, 13}, count).x - 20;
  int                                   text_w = std::max(20, count_x - text_x - 16);

  screen.Square({(double)g.x + 10 - screen.pic->buffer.width / 2.0,
                 (double)g.search_y - screen.pic->buffer.height / 2.0},
                {(double)g.w - 20, 30}, ALPHA(180, GRAY(10)), true);
  screen.Square({(double)g.x + 10 - screen.pic->buffer.width / 2.0,
                 (double)g.search_y - screen.pic->buffer.height / 2.0},
                {(double)g.w - 20, 30}, GRAY(150), false);
  draw_fixed_text(screen, label_x, g.search_y + 7, {10, 16}, hbs::Screen::Yellow, label);
  draw_fixed_text(screen, text_x, g.search_y + 7, {10, 16}, hbs::Screen::White,
                  truncate_to_width(screen, {10, 16}, text_w, query));
  draw_fixed_text(screen, count_x, g.search_y + 9, {8, 13}, GRAY(180), count);
}

static void                             draw_component_menu(hbs::Screen &screen,
                                                            const hbs::Circuit &c,
                                                            const hbs::Screen &state)
{
  if (!state.search_panel)
    return ;
  hbs::Position oldcam = screen.camera;
  int oldpinsize = screen.pin_size;
  const std::vector<std::string> &types = c.GetCreatableTypes();
  const t_bunny_position *mouse = bunny_get_mouse_position();
  hbs::ComponentMenuGeometry g = hbs::GetComponentMenuGeometry(screen);
  int seen = 0;
  int row = 0;
  int match_count = hbs::ComponentMatchCount(c, state.search_query);
  std::string detail_type;

  screen.pin_size = 1;
  screen.camera = {0, 0};
  screen.Square({(double)g.x - screen.pic->buffer.width / 2.0,
                 (double)g.y - screen.pic->buffer.height / 2.0},
                {(double)g.w, (double)g.h}, ALPHA(224, GRAY(32)), true);
  screen.Square({(double)g.x - screen.pic->buffer.width / 2.0,
                 (double)g.y - screen.pic->buffer.height / 2.0},
                {(double)g.w, (double)g.h}, hbs::Screen::White, false);
  draw_fixed_text(screen, g.x + 10, g.y + 8, {12, 18}, hbs::Screen::Yellow, "Ajouter un composant");
  draw_fixed_text(screen, g.x + 220, g.y + 10, {8, 13}, GRAY(180),
                  "Recherche multi-mots: nand quad, 74hc, compteur, ram, eeprom...");
  draw_search_box(screen, g, match_count);

  if (g.detail_w > 0)
    draw_fixed_text(screen, g.x + g.list_w + 10, g.y + 64, {8, 13}, GRAY(180),
                    "Apercu et fiche du composant survole.");
  draw_fixed_text(screen, g.x + 18, g.y + 64, {8, 13}, GRAY(180), "reference");
  draw_fixed_text(screen, g.x + 104, g.y + 64, {8, 13}, GRAY(180), "description courte");

  for (size_t i = 0; i < types.size(); ++i)
    {
      if (!hbs::ComponentMatchesQuery(types[i], state.search_query))
        continue ;
      if (seen++ < state.search_offset)
        continue ;
      if (row >= g.rows)
        break ;
      int item_y = g.first_y + row * g.line;
      bool hover = mouse->x >= g.x + 8 && mouse->x <= g.x + g.list_w - 8 &&
        mouse->y >= item_y && mouse->y < item_y + g.line;
      unsigned int col = (row == 0 || hover) ? hbs::Screen::Yellow : hbs::Screen::White;
      std::string desc = hbs::ComponentShortDescription(types[i]);

      if (detail_type.empty() || hover)
        detail_type = types[i];
      if (hover)
        screen.Square({(double)g.x + 8 - screen.pic->buffer.width / 2.0,
                       (double)item_y - screen.pic->buffer.height / 2.0},
                      {(double)g.list_w - 16, (double)g.line}, ALPHA(80, GRAY(96)), true);
      draw_fixed_text(screen, g.x + 18, item_y + 3, {10, 16}, col,
                      truncate_to_width(screen, {10, 16}, 74, types[i]));
      draw_fixed_text(screen, g.x + 104, item_y + 3, {10, 16}, col,
                      truncate_to_width(screen, {10, 16}, g.list_w - 122, desc));
      row += 1;
    }
  if (g.detail_w > 0)
    screen.Square({(double)g.x + g.list_w - screen.pic->buffer.width / 2.0,
                   (double)g.first_y - screen.pic->buffer.height / 2.0},
                  {1, (double)g.h - 126}, GRAY(100), true);
  if (match_count == 0)
    draw_fixed_text(screen, g.x + 18, g.first_y + 8, {10, 16}, hbs::Screen::Red,
                    "Aucun composant ne correspond a cette recherche.");
  else
    draw_component_detail(screen, g, detail_type);
  draw_fixed_text(screen, g.x + 10, g.y + g.h - 30, {8, 14}, hbs::Screen::White,
                  "Tape: filtrer  Backspace: effacer  Entree/Clic: choisir  Molette/PageUp/PageDown: defiler  Echap: fermer");
  screen.camera = oldcam;
  screen.pin_size = oldpinsize;
}

static void		draw_context_menu(hbs::Screen &screen)
{
  if (!screen.context_menu)
    return ;
  hbs::Position oldcam = screen.camera;
  int oldpinsize = screen.pin_size;

  screen.pin_size = 1;
  screen.camera = {0, 0};
  screen.Square({(double)screen.context_pos.x - screen.pic->buffer.width / 2.0,
		 (double)screen.context_pos.y - screen.pic->buffer.height / 2.0}, {130, 58}, ALPHA(220, GRAY(24)), true);
  screen.Square({(double)screen.context_pos.x - screen.pic->buffer.width / 2.0,
		 (double)screen.context_pos.y - screen.pic->buffer.height / 2.0}, {130, 58}, WHITE, false);
  draw_fixed_text(screen, screen.context_pos.x + 8, screen.context_pos.y + 6,
		  {10, 16}, hbs::Screen::White, "Renommer");
  draw_fixed_text(screen, screen.context_pos.x + 8, screen.context_pos.y + 32,
		  {10, 16}, hbs::Screen::Red, "Supprimer");
  screen.camera = oldcam;
  screen.pin_size = oldpinsize;
}

static void		draw_rename_box(hbs::Screen &screen)
{
  if (!screen.rename_mode)
    return ;
  hbs::Position oldcam = screen.camera;
  int oldpinsize = screen.pin_size;
  int x = screen.pic->buffer.width / 2 - 200;
  int y = screen.pic->buffer.height / 2 - 35;

  screen.pin_size = 1;
  screen.camera = {0, 0};
  screen.Square({(double)x - screen.pic->buffer.width / 2.0,
		 (double)y - screen.pic->buffer.height / 2.0}, {400, 70}, ALPHA(230, GRAY(16)), true);
  screen.Square({(double)x - screen.pic->buffer.width / 2.0,
		 (double)y - screen.pic->buffer.height / 2.0}, {400, 70}, WHITE, false);
  draw_fixed_text(screen, x + 10, y + 8, {10, 16}, hbs::Screen::Yellow, "Nouveau nom:");
  draw_fixed_text(screen, x + 10, y + 34, {12, 18}, hbs::Screen::White, screen.rename_buffer + "_");
  screen.camera = oldcam;
  screen.pin_size = oldpinsize;
}

bool			hbs::Screen::Draw(hbs::Circuit		&c)
{
  double		x;
  double		y;

  bunny_clear(&pic->buffer, BLACK);
  double left = camera.x - pic->buffer.width / pin_size / 2.0 - 1;
  double right = camera.x + pic->buffer.width / pin_size / 2.0 + 1;
  double top = camera.y - pic->buffer.height / pin_size / 2.0 - 1;
  double bottom = camera.y + pic->buffer.height / pin_size / 2.0 + 1;

  for (x = floor(left); x < ceil(right); ++x)
    for (y = floor(top); y < ceil(bottom); ++y)
      {
	Position	pos[2] =
	  {
	    {x, y},
	    {x + 1.0 / pin_size, y + 1.0 / pin_size}
	  };
	Line(pos[0], pos[1], GRAY(128));
      }
  c.Draw(*this);

  t_bunny_accurate_position npos =
    {
      (double)bunny_get_mouse_position()->x,
      (double)bunny_get_mouse_position()->y
    };

  npos.x -= grab_pos.x;
  npos.y -= grab_pos.y;
  npos.x /= pin_size;
  npos.y /= pin_size;
  npos.x = round(npos.x);
  npos.y = round(npos.y);

  if (selecting)
    {
      hbs::Position from = ScreenToWorld(grab_pos);
      hbs::Position to = ScreenToWorld(*bunny_get_mouse_position());
      hbs::Position corner = {std::min(from.x, to.x), std::min(from.y, to.y)};
      hbs::Size size = {fabs(to.x - from.x), fabs(to.y - from.y)};

      Square(corner, size, ALPHA(128, WHITE), true);
      Square(corner, size, WHITE, false);
    }

  if (dragging_selection)
    {
      for (auto it = selected_tracks.begin(); it != selected_tracks.end(); ++it)
	(*it)->Move(npos);
      for (auto it = selected_components.begin(); it != selected_components.end(); ++it)
	(*it)->Move(npos);
      for (auto it = selected_tracks.begin(); it != selected_tracks.end(); ++it)
	(*it)->Draw(*this);
      for (auto it = selected_components.begin(); it != selected_components.end(); ++it)
	(*it)->Draw(*this);
      DrawSelectionBox();
      npos.x *= -1;
      npos.y *= -1;
      for (auto it = selected_tracks.begin(); it != selected_tracks.end(); ++it)
	(*it)->Move(npos);
      for (auto it = selected_components.begin(); it != selected_components.end(); ++it)
	(*it)->Move(npos);
      npos.x *= -1;
      npos.y *= -1;
    }
  else if (!selecting)
    DrawSelectionBox();

  if (grabbed)
    {
      grabbed->Move(npos);
      grabbed->Draw(*this);
      npos.x *= -1;
      npos.y *= -1;
      grabbed->Move(npos);
      npos.x *= -1;
      npos.y *= -1;
    }
  if (grabbed_step != c.EndLinkStep())
    {
      grabbed_step.track->MoveNode(grabbed_step.node, npos);
      grabbed_step.track->Draw(*this);
      npos.x *= -1;
      npos.y *= -1;
      grabbed_step.track->MoveNode(grabbed_step.node, npos);
      npos.x *= -1;
      npos.y *= -1;
    }
  if (active_track != NULL && active_node != hbs::Track::NoNode)
    active_track->Draw(*this);

  char			buffer[32];

  snprintf(&buffer[0], sizeof(buffer), "TIME %04d", (int)c.GetTime());

  hbs::Position oldcam = camera;
  int oldpinsize = pin_size;

  pin_size = 1;
  camera = {0, 0};
  hbs::DrawShortcutBar(*this);

  Text(Position{0.5 * pic->buffer.width - TextSize({12, 15}, buffer).x - 6.0, pic->buffer.height - 24.0 - pic->buffer.height / 2}, {12, 15}, RED, buffer);

  Text({5.0 - pic->buffer.width / 2.0, pic->buffer.height - 5.0 * 15 - pic->buffer.height / 2.0}, {10, 10}, hbs::Screen::Red, "BUNNY CAD - by Jason Brillante Damdoshi");
  Text({5.0 - pic->buffer.width / 2.0, pic->buffer.height - 4.0 * 15 - pic->buffer.height / 2.0}, {10, 10}, hbs::Screen::Red, "EFRITS SAS 2022-2023");
  Text({5.0 - pic->buffer.width / 2.0, pic->buffer.height - 3.0 * 15 - pic->buffer.height / 2.0}, {10, 10}, hbs::Screen::Red, "Hanged Bunny Studio 2014-2021");
  Text({5.0 - pic->buffer.width / 2.0, pic->buffer.height - 2.0 * 15 - pic->buffer.height / 2.0}, {10, 10}, hbs::Screen::Red, "Pentacle Technologie 2008-2023");
  Text({5.0 - pic->buffer.width / 2.0, pic->buffer.height - 1.0 * 15 - pic->buffer.height / 2.0}, {10, 10}, hbs::Screen::Red, "Under GNU GPL V3");

  if (drawing_mode)
    Text({8 - pic->buffer.width / 2.0, hbs::ShortcutBarHeight + 6.0 - pic->buffer.height / 2.0}, {10, 15},
	 hbs::Screen::Yellow, "Mode dessin: clic gauche route, double-clic/V via, clic droit court detache/supprime, glisse camera");
  if (placing_component)
    Text({8 - pic->buffer.width / 2.0, hbs::ShortcutBarHeight + 26.0 - pic->buffer.height / 2.0}, {10, 15},
	 hbs::Screen::Yellow, "Placement: clic pose, Shift+clic pose et continue, clic droit court/Echap annule");
  camera = oldcam;
  pin_size = oldpinsize;

  draw_component_menu(*this, c, *this);
  draw_context_menu(*this);
  draw_rename_box(*this);
  bunny_clear(&win->buffer, BLACK);
  bunny_blit(&win->buffer, pic, NULL);
  bunny_display(win);
  return (true);
}
