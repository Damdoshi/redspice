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
#include                "Prototype.hpp"

// Apply the drag preview before drawing any tracks, so airwires use the same
// pin positions as the symbols. Restore the document even if drawing throws.
class PreviewMoves
{
  struct Move
  {
    hbs::IComponent *component;
    hbs::Track *track;
    size_t node;
    hbs::Position delta;
  };
  std::vector<Move> moves;
public:
  explicit PreviewMoves(size_t count) { moves.reserve(count); }
  void Component(hbs::IComponent *component, hbs::Position delta)
  {
    component->Move(delta);
    moves.push_back({component, NULL, 0, delta});
  }
  void Node(hbs::Track *track, size_t node, hbs::Position delta)
  {
    track->MoveNode(node, delta);
    moves.push_back({NULL, track, node, delta});
  }
  ~PreviewMoves()
  {
    for (auto move = moves.rbegin(); move != moves.rend(); ++move)
      {
        hbs::Position undo{-move->delta.x, -move->delta.y};
        if (move->track) move->track->MoveNode(move->node, undo);
        else move->component->Move(undo);
      }
  }
};


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
  if (!hbs::DrawPrototypePreview(screen, type, x + 12, y + 60, w - 24, std::min(230, h / 2)))
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

static void draw_component_menu(hbs::Screen &screen,const std::vector<LoopData::MenuEntry> &entries,const hbs::Screen &state)
{
  if(!state.search_panel)return;
  auto oldcam=screen.camera;int oldscale=screen.pin_size;
  auto g=hbs::GetComponentMenuGeometry(screen);auto mouse=*bunny_get_mouse_position();
  std::vector<size_t> matches;for(size_t n=0;n<entries.size();++n)if(hbs::ComponentMenuMatches(entries[n],state.search_query))matches.push_back(n);
  screen.search_offset=std::clamp(screen.search_offset,0,std::max(0,int(matches.size())-1));
  screen.camera={0,0};screen.pin_size=1;
  screen.Square({double(g.x)-screen.pic->buffer.width/2.,double(g.y)-screen.pic->buffer.height/2.},{double(g.w),double(g.h)},ALPHA(240,GRAY(28)),true);
  screen.Square({double(g.x)-screen.pic->buffer.width/2.,double(g.y)-screen.pic->buffer.height/2.},{double(g.w),double(g.h)},hbs::Screen::White,false);
  draw_fixed_text(screen,g.x+10,g.y+8,{12,18},hbs::Screen::Yellow,"Component");
  draw_fixed_text(screen,g.x+180,g.y+10,{8,13},GRAY(180),"Ctrl+clic : ajouter aux favoris lateraux");
  draw_search_box(screen,g,int(matches.size()));
  draw_fixed_text(screen,g.x+18,g.y+64,{8,13},GRAY(180),"reference");
  draw_fixed_text(screen,g.x+104,g.y+64,{8,13},GRAY(180),"description");
  const LoopData::MenuEntry *detail=nullptr;
  for(int row=0;row<g.rows && row+screen.search_offset<int(matches.size());++row){
    const auto &entry=entries[matches[size_t(row+screen.search_offset)]];int y=g.first_y+row*g.line;
    bool hover=mouse.x>=g.x+8 && mouse.x<g.x+g.list_w-8 && mouse.y>=y && mouse.y<y+g.line;
    if(!detail || hover)detail=&entry;
    auto color=row==0 || hover?hbs::Screen::Yellow:hbs::Screen::White;
    if(hover)screen.Square({double(g.x+8)-screen.pic->buffer.width/2.,double(y)-screen.pic->buffer.height/2.},{double(g.list_w-16),double(g.line)},ALPHA(80,GRAY(96)),true);
    draw_fixed_text(screen,g.x+18,y+3,{10,16},color,truncate_to_width(screen,{10,16},74,entry.label));
    draw_fixed_text(screen,g.x+104,y+3,{10,16},color,truncate_to_width(screen,{10,16},g.list_w-122,entry.description));
  }
  if(!detail)draw_fixed_text(screen,g.x+18,g.first_y+8,{10,16},hbs::Screen::Red,"Aucun composant ne correspond a cette recherche.");
  else if(g.detail_w>0){
    if(detail->component.kind=="component")draw_component_detail(screen,g,detail->component.type);
    else{
      int x=g.x+g.list_w+10,w=g.detail_w-20;
      draw_fixed_text(screen,x,g.first_y+8,{12,18},hbs::Screen::Yellow,truncate_to_width(screen,{12,18},w,detail->label));
      draw_wrapped_text(screen,x,g.first_y+42,w,{9,14},hbs::Screen::White,detail->description);
      if(detail->component.kind=="cell")draw_wrapped_text(screen,x,g.first_y+120,w,{8,13},hbs::Screen::Teal,detail->component.source.substr(0,220));
      draw_wrapped_text(screen,x,g.first_y+240,w,{9,14},hbs::Screen::White,detail->browse?"Ouvrir le selecteur de fichiers.":"Clic : choisir ce composant. Ctrl+clic : le garder dans un bouton lateral.");
    }
  }
  draw_fixed_text(screen,g.x+10,g.y+g.h-28,{8,14},hbs::Screen::White,"Entree/Clic: choisir  Ctrl+clic: favori  Molette: defiler  Echap: retour");
  screen.camera=oldcam;screen.pin_size=oldscale;
}
void hbs::DrawComponentMenu(LoopData &ld){
  if(!ld.screen.search_panel)return;
  if(ld.component_menu.empty())hbs::RefreshComponentMenu(ld);
  draw_component_menu(ld.screen,ld.component_menu,ld.screen);
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

bool hbs::Screen::Draw(hbs::Circuit &c,LoopData *workspace)
{
  double		x;
  double		y;

  pin_size = std::clamp(pin_size, PINSIZE_DEFAULT / 4, PINSIZE_DEFAULT * 10);
  if (!std::isfinite(camera.x) || std::abs(camera.x) > 1000000.0) camera.x = 0;
  if (!std::isfinite(camera.y) || std::abs(camera.y) > 1000000.0) camera.y = 0;

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

  {
    size_t count = dragging_selection ? selected_tracks.size() + selected_components.size() : 0;
    PreviewMoves preview(count + (grabbed ? 1 : 0) + (grabbed_step != c.EndLinkStep() ? 1 : 0));
    if (dragging_selection)
      {
        for (auto track : selected_tracks) preview.Component(track, npos);
        for (auto component : selected_components) preview.Component(component, npos);
      }
    if (grabbed) preview.Component(grabbed, npos);
    if (grabbed_step != c.EndLinkStep())
      preview.Node(grabbed_step.track, grabbed_step.node, npos);
    c.Draw(*this);

    if (selecting)
      {
        hbs::Position from = ScreenToWorld(grab_pos);
        hbs::Position to = ScreenToWorld(*bunny_get_mouse_position());
        hbs::Position corner = {std::min(from.x, to.x), std::min(from.y, to.y)};
        hbs::Size size = {fabs(to.x - from.x), fabs(to.y - from.y)};

        Square(corner, size, ALPHA(128, WHITE), true);
        Square(corner, size, WHITE, false);
      }
    else DrawSelectionBox();
  }

  char			buffer[32];

  snprintf(&buffer[0], sizeof(buffer), "TIME %04d", (int)c.GetTime());

  hbs::Position oldcam = camera;
  int oldpinsize = pin_size;

  pin_size = 1;
  camera = {0, 0};
  hbs::DrawShortcutBar(*this,workspace);

  Text(Position{0.5 * pic->buffer.width - TextSize({12, 15}, buffer).x - 6.0, pic->buffer.height - 24.0 - pic->buffer.height / 2}, {12, 15}, RED, buffer);

  Text({5.0 - pic->buffer.width / 2.0, pic->buffer.height - 5.0 * 15 - pic->buffer.height / 2.0}, {10, 10}, hbs::Screen::Red, "BUNNY CAD - by Jason Brillante Damdoshi");
  Text({5.0 - pic->buffer.width / 2.0, pic->buffer.height - 4.0 * 15 - pic->buffer.height / 2.0}, {10, 10}, hbs::Screen::Red, "EFRITS SAS 2022-2023");
  Text({5.0 - pic->buffer.width / 2.0, pic->buffer.height - 3.0 * 15 - pic->buffer.height / 2.0}, {10, 10}, hbs::Screen::Red, "Hanged Bunny Studio 2014-2021");
  Text({5.0 - pic->buffer.width / 2.0, pic->buffer.height - 2.0 * 15 - pic->buffer.height / 2.0}, {10, 10}, hbs::Screen::Red, "Pentacle Technologie 2008-2023");
  Text({5.0 - pic->buffer.width / 2.0, pic->buffer.height - 1.0 * 15 - pic->buffer.height / 2.0}, {10, 10}, hbs::Screen::Red, "Under GNU GPL V3");

  if (IsRoutingMode())
    Text({8 - pic->buffer.width / 2.0, hbs::ShortcutBarHeight + 6.0 - pic->buffer.height / 2.0}, {10, 15},
	 hbs::Screen::Yellow, "Dessin: clic pose, V/double-clic via, clic droit detache, Espace+glisse gauche camera");
  else if (active_track)
    Text({8 - pic->buffer.width / 2.0, hbs::ShortcutBarHeight + 6.0 - pic->buffer.height / 2.0}, {10, 15},
         hbs::Screen::Yellow, "Trace en attente: Ctrl pour continuer, glisse gauche camera, clic droit detache");
  else if (HasSelection())
    Text({8 - pic->buffer.width / 2.0, hbs::ShortcutBarHeight + 6.0 - pic->buffer.height / 2.0}, {10, 15},
         hbs::Screen::Yellow, auto_routing ?
         "PageUp/PageDown: rotation  Auto-routage local actif (Ctrl+F10)" :
         "PageUp/PageDown: rotation  Auto-routage local inactif (Ctrl+F10)");
  if (placing_component)
    Text({8 - pic->buffer.width / 2.0, hbs::ShortcutBarHeight + 26.0 - pic->buffer.height / 2.0}, {10, 15},
	 hbs::Screen::Yellow, "Placement: clic pose, PageUp/PageDown tourne, Shift+clic continue, clic droit/Echap annule");
  camera = oldcam;
  pin_size = oldpinsize;

  if(workspace){hbs::DrawComponentDocks(*workspace);hbs::DrawComponentMenu(*workspace);}
  else if(search_panel){std::vector<LoopData::MenuEntry> entries;for(const auto &type:c.GetCreatableTypes())entries.push_back({{"component",type,""},type,hbs::ComponentShortDescription(type),false});draw_component_menu(*this,entries,*this);}
  draw_context_menu(*this);
  draw_rename_box(*this);
  bunny_clear(&win->buffer, BLACK);
  bunny_blit(&win->buffer, pic, NULL);
  bunny_display(win);
  return (true);
}
