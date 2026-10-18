// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<ctime>
#include		"Link.hpp"
#include		"Parsing.hpp"
#include		"Exception.hpp"

void			hbs::Link::Draw(hbs::Screen		&screen,
					const IComponent	&origin,
					size_t			ori_pin) const
{
  auto			prev = third.begin();
  if (first == NULL)
    return;

  //if (&origin == screen.grabbed || first == screen.grabbed) return ;
  // Pas de positions déterminées
  if (third.size() == 0)
    {
      auto a = origin.GetPinPosition(ori_pin);
      auto b = first->GetPinPosition(second);

      screen.DashedLine(a, b, GRAY(160));
      return ;
    }
  auto a = origin.GetPinPosition(ori_pin);
  auto b = third.begin()->first;

  screen.DashedLine(a, b, GRAY(160));

  // Des positions déterminées
  for (auto it = third.begin(); it != third.end(); ++it)
    {
      for (double i = -1; i < 2; ++i)
	for (double j = -1; j < 2; ++j)
	  screen.Line(prev->first + hbs::Position{i / screen.PinSize(), j / screen.PinSize()},
		      it->first + hbs::Position{i / screen.PinSize(), j / screen.PinSize()},
		      prev->second == TOP ? hbs::Screen::Red : hbs::Screen::Blue
		      );
      if (prev->second != it->second) // Il y a un VIA
	{
          screen.Via(it->first, 1);
	}
      prev = it;
    }

  auto c = third.rbegin()->first;
  auto d = first->GetPinPosition(second);

  screen.DashedLine(c, d, GRAY(160));
}
