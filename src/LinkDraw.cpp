// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"AComponent.hpp"

void			hbs::Link::Draw(hbs::Screen		&screen,
					const IComponent	&origin,
					size_t			ori_pin) const
{
  auto			prev = third.begin();
  auto			now = (int)(50 * clock() / CLOCKS_PER_SEC) % 2;

  //if (&origin == screen.grabbed || first == screen.grabbed) return ;
  // Pas de positions déterminées
  if (third.size() <= 2)
    {
      auto a = origin.GetPinPosition(ori_pin);
      auto b = first->GetPinPosition(second);

      for (double i = -1; i < 2; ++i)
	for (double j = -1; j < 2; ++j)
	  screen.Line(a + hbs::Screen::Position{i / screen.PinSize(), j / screen.PinSize()},
		      b + hbs::Screen::Position{i / screen.PinSize(), j / screen.PinSize()},
		      now ? hbs::Screen::Yellow : hbs::Screen::White
		      );
      return ;
    }
  // Des positions déterminées
  for (auto it = third.begin(); it != third.end(); ++it)
    {
      for (double i = -1; i < 2; ++i)
	for (double j = -1; j < 2; ++j)
	  screen.Line(prev->first + hbs::Screen::Position{i / screen.PinSize(), j / screen.PinSize()},
		      it->first + hbs::Screen::Position{i / screen.PinSize(), j / screen.PinSize()},
		      prev->second == TOP ? hbs::Screen::Red : hbs::Screen::Blue
		      );
      if (prev->second != it->second) // Il y a un VIA
	{
	  screen.Circle(it->first, {1, 1}, hbs::Screen::Yellow, true);
	  screen.Circle(it->first, {1, 1}, hbs::Screen::White, false);
	}
      prev = it;
    }
}
