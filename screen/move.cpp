// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<cmath>
#include		"IComponent.hpp"
#include		"Screen.hpp"

extern "C"
t_bunny_response	screen_move(const t_bunny_position	*rel,
				    LoopData			&ld)
{
  (void)rel;
  if (ld.screen.panning)
    {
      const t_bunny_position *mouse = bunny_get_mouse_position();
      ld.screen.context_menu = false;
      ld.screen.PanByScreenDelta(mouse->x - ld.screen.pan_last.x,
					 mouse->y - ld.screen.pan_last.y);
      ld.screen.pan_last = *mouse;
    }
  if (ld.screen.placing_component && ld.screen.component_to_place != NULL)
    {
      hbs::Position target = ld.screen.ScreenToWorld(*bunny_get_mouse_position());
      hbs::Position current = ld.screen.component_to_place->GetPosition();

      target.x = round(target.x);
      target.y = round(target.y);
      ld.screen.component_to_place->Move({target.x - current.x, target.y - current.y});
    }
  return (GO_ON);
}

