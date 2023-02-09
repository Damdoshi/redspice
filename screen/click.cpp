// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Circuit.hpp"
#include		"Screen.hpp"

extern "C"
t_bunny_response	screen_click(t_bunny_event_state	state,
				     t_bunny_mouse_button	sym,
				     LoopData			&ld)
{
  t_bunny_position	pos = *bunny_get_mouse_position();
  
  if (pos.y < 75 && pos.x < 400 && state == GO_UP)
    {
      int but = pos.x / 100;

      if (but == 0)
	{}
      else if (but == 1)
	ld.screen.loopsim = false;
      else if (but == 2)
	hbs::Command("simulate", ld.circuit, ld.timer);
      else if (but == 3)
	ld.screen.loopsim = true;
      return (GO_ON);
    }

  pos.x -= ld.screen.camera.x + ld.screen.pic->buffer.width / 2;
  pos.y -= ld.screen.camera.y + ld.screen.pic->buffer.height / 2;
  pos.x /= ld.screen.pin_size;
  pos.y /= ld.screen.pin_size;
  hbs::Input *in = ld.circuit.GetInput(ld.screen, pos);
  hbs::IComponent *ic = ld.circuit.GetComponent(ld.screen, pos);
  hbs::Link::Positions::iterator step = ld.circuit.GetLinkStep(ld.screen, pos);

  if (in && sym == BMB_MIDDLE && state == GO_UP)
    {
      if (in->GetPin(1) != hbs::TRUE)
	in->SetValue(hbs::TRUE);
      else
	in->SetValue(hbs::FALSE);
    }

  if (sym == BMB_LEFT && state == GO_DOWN)
    ld.screen.grab_pos = *bunny_get_mouse_position();
  if (ic && sym == BMB_LEFT && state == GO_DOWN)
    ld.screen.grabbed = ic;

  if (sym == BMB_LEFT && state == GO_UP && ld.screen.grabbed)
    {
      t_bunny_accurate_position npos =
	{
	  (double)bunny_get_mouse_position()->x,
	  (double)bunny_get_mouse_position()->y
	};

      npos.x -= ld.screen.grab_pos.x;
      npos.y -= ld.screen.grab_pos.y;
      npos.x /= ld.screen.pin_size;
      npos.y /= ld.screen.pin_size;
      npos.x = round(npos.x);
      npos.y = round(npos.y);
      ld.screen.grabbed->Move(npos);
      ld.screen.grabbed = NULL;
    }

  (void)step;
  
  return (GO_ON);
}

