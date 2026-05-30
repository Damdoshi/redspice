// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<lapin.h>
#include		"Screen.hpp"
#include		"Track.hpp"

hbs::Screen::Screen(std::string const &filename)
{
  file_name = filename;
  loopsim = false;
  grabbed = NULL;
  grabbed_step = hbs::Packet();
  active_track = NULL;
  active_node = hbs::Track::NoNode;
  panning = false;
  selecting = false;
  dragging_selection = false;
  context_menu = false;
  rename_mode = false;
  drawing_mode = false;
  placing_component = false;
  component_to_place = NULL;
  placing_type = "";
  context_pos = {0, 0};
  rename_buffer = "";
  search_panel = false;
  search_offset = 0;
  search_query = "";
  pan_last = {0, 0};
  pin_size = PINSIZE_DEFAULT;
  if ((win = bunny_start(1440, 900, false, "Bunny CAD")) == NULL)
    throw 0;
  if ((pic = bunny_new_picture(win->buffer.width, win->buffer.height)) == NULL)
    throw 0;
  if ((author = bunny_load_picture("./auth.png")) == NULL)
    throw 0;
  author->position.y = pic->buffer.height - author->buffer.height;
  if ((controls = bunny_load_picture("./control.png")) == NULL)
    throw 0;
  camera.x = 0;
  camera.y = 0;
}

hbs::Screen::~Screen(void)
{
  bunny_delete_clipable(author);
  bunny_delete_clipable(controls);
  bunny_delete_clipable(pic);
  bunny_stop(win);
}

