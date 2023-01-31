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

  // Pas de positions déterminées
  if (prev == third.end())
    {
      auto a = origin.GetPinPosition(ori_pin);
      auto b = first->GetPinPosition(second);

      screen.Line(a, b, GRAY(128));
      return ;
    }
  // Des positions déterminées
  for (auto it = third.begin(); it != third.end(); ++it)
    {
      screen.Line(prev->first, it->first, prev->second == TOP ? GRAY(255) : GRAY(128));
      if (prev->second != it->second) // Il y a un VIA
	{
	  screen.Circle(it->first, {1, 1}, hbs::Screen::Yellow, true);
	  screen.Circle(it->first, {1, 1}, hbs::Screen::White, false);
	}
      prev = it;
    }
}
