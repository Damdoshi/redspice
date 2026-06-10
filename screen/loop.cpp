// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Circuit.hpp"
#include		"Screen.hpp"

extern "C"
t_bunny_response	screen_loop(LoopData			&ld)
{
  const bool *keyboard = bunny_get_keyboard();
  double speed = 0.65;

  if (keyboard[BKS_LSHIFT] || keyboard[BKS_RSHIFT])
    speed *= 3.0;
  bool moved = false;

  if (ld.file_browser)
    return (GO_ON);

  if (ld.screen.search_panel)
    {
      if (ld.screen.loopsim)
	hbs::Simulate(ld.CurrentCircuit(), ld.CurrentTimer(), ld.screen.loop_ticks_per_frame);
      return (GO_ON);
    }

  if (keyboard[BKS_Z] || keyboard[BKS_W] || keyboard[BKS_UP])
    ld.screen.camera.y -= speed, moved = true;
  if (keyboard[BKS_S] || keyboard[BKS_DOWN])
    ld.screen.camera.y += speed, moved = true;
  if (keyboard[BKS_Q] || keyboard[BKS_A] || keyboard[BKS_LEFT])
    ld.screen.camera.x -= speed, moved = true;
  if (keyboard[BKS_D] || keyboard[BKS_RIGHT])
    ld.screen.camera.x += speed, moved = true;
  if (moved)
    ld.screen.context_menu = false;
  if (ld.screen.loopsim)
    hbs::Simulate(ld.CurrentCircuit(), ld.CurrentTimer(), ld.screen.loop_ticks_per_frame);
  return (GO_ON);
}

