// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<algorithm>
#include		<cmath>
#include		"Circuit.hpp"
#include		"Screen.hpp"
#include		"Track.hpp"


static void		draw_fixed_text(hbs::Screen &screen,
					double x,
					double y,
					hbs::Size size,
					unsigned int color,
					const std::string &text)
{
  screen.Text({x - screen.win->buffer.width / 2.0,
	       y - screen.win->buffer.height / 2.0}, size, color, text);
}

static bool		matches_query(const std::string &type, const std::string &query)
{
  if (query.empty())
    return (true);
  return (type.find(query) != std::string::npos);
}

static void		draw_component_menu(hbs::Screen &screen,
					   const hbs::Circuit &c,
					   const hbs::Screen &state)
{
  if (!state.search_panel)
    return ;
  hbs::Position oldcam = screen.camera;
  int oldpinsize = screen.pin_size;
  const std::vector<std::string> &types = c.GetCreatableTypes();
  const t_bunny_position *mouse = bunny_get_mouse_position();
  int x = screen.win->buffer.width - 330;
  int y = 80;
  int line = 24;
  int seen = 0;
  int row = 0;

  screen.pin_size = 1;
  screen.camera = {0, 0};
  screen.Square({(double)x - screen.win->buffer.width / 2.0,
		 (double)y - screen.win->buffer.height / 2.0}, {320, 500}, ALPHA(210, GRAY(32)), true);
  screen.Square({(double)x - screen.win->buffer.width / 2.0,
		 (double)y - screen.win->buffer.height / 2.0}, {320, 500}, WHITE, false);
  draw_fixed_text(screen, x + 10, y + 8, {12, 18}, hbs::Screen::Yellow, "Ajouter un composant");
  draw_fixed_text(screen, x + 10, y + 34, {10, 16}, hbs::Screen::White, "> " + state.search_query);
  for (size_t i = 0; i < types.size(); ++i)
    {
      if (!matches_query(types[i], state.search_query))
	continue ;
      if (seen++ < state.search_offset)
	continue ;
      if (row >= 16)
	break ;
      int item_y = y + 58 + row * line;
      bool hover = mouse->x >= x + 8 && mouse->x <= x + 312 && mouse->y >= item_y && mouse->y < item_y + line;
      unsigned int col = (row == 0 || hover) ? hbs::Screen::Yellow : hbs::Screen::White;

      if (hover)
	screen.Square({(double)x + 8 - screen.win->buffer.width / 2.0,
		     (double)item_y - screen.win->buffer.height / 2.0}, {304, (double)line}, ALPHA(80, GRAY(96)), true);
      draw_fixed_text(screen, x + 18, item_y, {10, 16}, col, types[i]);
      row += 1;
    }
  draw_fixed_text(screen, x + 10, y + 462, {8, 14}, hbs::Screen::White,
		  "Clic/Entree: choisir  Molette: defiler");
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
  screen.Square({(double)screen.context_pos.x - screen.win->buffer.width / 2.0,
		 (double)screen.context_pos.y - screen.win->buffer.height / 2.0}, {130, 58}, ALPHA(220, GRAY(24)), true);
  screen.Square({(double)screen.context_pos.x - screen.win->buffer.width / 2.0,
		 (double)screen.context_pos.y - screen.win->buffer.height / 2.0}, {130, 58}, WHITE, false);
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
  int x = screen.win->buffer.width / 2 - 200;
  int y = screen.win->buffer.height / 2 - 35;

  screen.pin_size = 1;
  screen.camera = {0, 0};
  screen.Square({(double)x - screen.win->buffer.width / 2.0,
		 (double)y - screen.win->buffer.height / 2.0}, {400, 70}, ALPHA(230, GRAY(16)), true);
  screen.Square({(double)x - screen.win->buffer.width / 2.0,
		 (double)y - screen.win->buffer.height / 2.0}, {400, 70}, WHITE, false);
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
  double left = camera.x - win->buffer.width / pin_size / 2.0 - 1;
  double right = camera.x + win->buffer.width / pin_size / 2.0 + 1;
  double top = camera.y - win->buffer.height / pin_size / 2.0 - 1;
  double bottom = camera.y + win->buffer.height / pin_size / 2.0 + 1;

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
  Text(Position{0.5 * pic->buffer.width - TextSize({20, 20}, buffer).x - 0.5, 4.0 - pic->buffer.height / 2}, {20, 20}, RED, buffer);
  Text(Position{0.5 * pic->buffer.width - TextSize({20, 20}, buffer).x + 0.0, 5.0 - pic->buffer.height / 2}, {20, 20}, RED, buffer);
  Text(Position{0.5 * pic->buffer.width - TextSize({20, 20}, buffer).x + 0.5, 6.0 - pic->buffer.height / 2}, {20, 20}, RED, buffer);

  Text({5.0 - win->buffer.width / 2.0, win->buffer.height - 5.0 * 15 - win->buffer.height / 2.0}, {10, 10}, hbs::Screen::Red, "BUNNY CAD - by Jason Brillante Damdoshi");
  Text({5.0 - win->buffer.width / 2.0, win->buffer.height - 4.0 * 15 - win->buffer.height / 2.0}, {10, 10}, hbs::Screen::Red, "EFRITS SAS 2022-2023");
  Text({5.0 - win->buffer.width / 2.0, win->buffer.height - 3.0 * 15 - win->buffer.height / 2.0}, {10, 10}, hbs::Screen::Red, "Hanged Bunny Studio 2014-2021");
  Text({5.0 - win->buffer.width / 2.0, win->buffer.height - 2.0 * 15 - win->buffer.height / 2.0}, {10, 10}, hbs::Screen::Red, "Pentacle Technologie 2008-2023");
  Text({5.0 - win->buffer.width / 2.0, win->buffer.height - 1.0 * 15 - win->buffer.height / 2.0}, {10, 10}, hbs::Screen::Red, "Under GNU GPL V3");

  Text({5 - win->buffer.width / 2.0, 5 - win->buffer.height / 2.0}, {15, 75}, hbs::Screen::Red | (GRAY(0)), "SAVE!");
  Text({105 - win->buffer.width / 2.0, 5 - win->buffer.height / 2.0}, {15, 75}, hbs::Screen::Red | (GRAY(96)), "STOP!");
  Text({205 - win->buffer.width / 2.0, 5 - win->buffer.height / 2.0}, {15, 75}, hbs::Screen::Red | (GRAY(0)), "STEP!");
  Text({305 - win->buffer.width / 2.0, 5 - win->buffer.height / 2.0}, {15, 75}, hbs::Screen::Red | (GRAY(96)), "RUN");
  Text({405 - win->buffer.width / 2.0, 5 - win->buffer.height / 2.0}, {15, 75}, hbs::Screen::Red | (GRAY(0)), "ADD");
  Text({505 - win->buffer.width / 2.0, 5 - win->buffer.height / 2.0}, {15, 75},
       hbs::Screen::Red | (drawing_mode ? GRAY(160) : GRAY(0)), drawing_mode ? "DRAW" : "SEL");
  if (drawing_mode)
    Text({605 - win->buffer.width / 2.0, 5 - win->buffer.height / 2.0}, {10, 15},
	 hbs::Screen::Yellow, "Mode dessin: clic gauche route, double-clic/V via, clic droit detache/supprime");
  if (placing_component)
    Text({605 - win->buffer.width / 2.0, 25 - win->buffer.height / 2.0}, {10, 15},
	 hbs::Screen::Yellow, "Placement: clic pose, Shift+clic pose et continue, Echap annule");
  camera = oldcam;
  pin_size = oldpinsize;

  draw_component_menu(*this, c, *this);
  draw_context_menu(*this);
  draw_rename_box(*this);
  bunny_blit(&win->buffer, pic, NULL);
  bunny_display(win);
  return (true);
}
