// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<lapin.h>
#include		"ResourcePath.hpp"
#include		"Screen.hpp"
#include		"Track.hpp"
#include                <stdexcept>

hbs::Screen::Screen(std::string const &filename)
{
  std::string		author_path;
  std::string		controls_path;

  win = NULL;
  pic = author = controls = NULL;
  grab_pos = pan_origin = {0, 0};

  file_name = filename;
  loopsim = false;
  panning = false;
  right_panning = false;
  right_panning_moved = false;
  loop_ticks_per_frame = 64;
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
  auto_routing = true;
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
  try
  {
  if ((win = bunny_start_style(1440, 900,
				  (t_bunny_window_style)(TITLEBAR | RESIZE_BUTTON | CLOSE_BUTTON),
				  "Bunny CAD")) == NULL)
    throw std::runtime_error("Cannot create BCAD window.");
  bunny_enable_full_blit(true);
  if ((pic = bunny_new_picture(win->buffer.width, win->buffer.height)) == NULL)
    throw std::runtime_error("Cannot allocate BCAD drawing buffer.");
  author_path = hbs::BcadResourcePath("auth.png");
  if ((author = bunny_load_picture(author_path.c_str())) == NULL)
    throw std::runtime_error("Cannot load " + author_path);
  author->position.y = pic->buffer.height - author->buffer.height;
  controls_path = hbs::BcadResourcePath("control.png");
  if ((controls = bunny_load_picture(controls_path.c_str())) == NULL)
    throw std::runtime_error("Cannot load " + controls_path);
  camera.x = 0;
  camera.y = 0;
  }
  catch (...)
    {
      if (author) bunny_delete_clipable(author);
      if (controls) bunny_delete_clipable(controls);
      if (pic) bunny_delete_clipable(pic);
      if (win) bunny_stop(win);
      throw;
    }
}

hbs::Screen::~Screen(void)
{
  bunny_delete_clipable(author);
  bunny_delete_clipable(controls);
  bunny_delete_clipable(pic);
  bunny_stop(win);
}

