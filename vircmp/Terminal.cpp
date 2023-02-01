// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		<unistd.h>
#include		"Terminal.hpp"

const std::string	&hbs::Terminal::GetType(void) const
{
  return (hbs::AComponent<9>::type = "Terminal");
}

hbs::Screen::Position	hbs::Terminal::GetPosition(void) const
{
  return (hbs::AComponent<9>::GetPosition());
}

hbs::Screen::Position	hbs::Terminal::GetPinPosition(size_t	pin) const
{
  return (hbs::Screen::Position{0, (double)pin});
}

void			hbs::Terminal::Draw(hbs::Screen		&screen) const
{
  screen.Square(hbs::AComponent<9>::position + hbs::Screen::Position{1, 0}, {1, 9}, hbs::Screen::White);
  for (double i = 0; i < 9; ++i)
    {
      screen.Line(hbs::AComponent<9>::position + hbs::Screen::Position{0, i},
		  hbs::AComponent<9>::position + hbs::Screen::Position{1, i},
		  hbs::Screen::White);
      screen.Circle(hbs::AComponent<9>::position + hbs::Screen::Position{0, i},
		    hbs::Screen::Position{0.5, 0.5}, hbs::Screen::Teal, true);
      screen.Circle(hbs::AComponent<9>::position + hbs::Screen::Position{0, i},
		    hbs::Screen::Position{0.5, 0.5}, hbs::Screen::White, false);
    }
  screen.Text(hbs::AComponent<9>::position + hbs::Screen::Position{0, -1}, {10, 10}, hbs::Screen::White, GetType());
  char			buf[2] = {c, '\0'};

  screen.Text(hbs::AComponent<9>::position + hbs::Screen::Position{1, 1}, {10, 10}, hbs::Screen::White, std::string(buf));
}

void			hbs::Terminal::SetLink(size_t		a,
					       hbs::IComponent	&b,
					       size_t		 c,
					       const std::string &linkpath)
{
  hbs::AComponent<9>::SetLink(a, b, c, linkpath);
}

void			hbs::Terminal::Dump(void) const
{
  hbs::AComponent<9>::Dump();
}

hbs::Tristate		hbs::Terminal::Compute(size_t		n)
{
  int			i;

  (void)n;
  if (hbs::AComponent<9>::GetPrevious(9) == hbs::FALSE &&
      hbs::AComponent<9>::GetPin(9) == hbs::TRUE &&
      last_tick != hbs::AComponent<9>::timer.GetTime())
    {
      c = 0;
      last_tick = hbs::AComponent<9>::timer.GetTime();
      for (i = 0; i < 7; ++i)
	c |= AComponent<9>::GetPin(i + 1) << i;
      if (write(1, &c, 1)) {}
    }
  else
    for (i = 1; i < 8; ++i)
      AComponent<9>::GetPin(i);
  return (hbs::TRUE);
}

bool			hbs::Terminal::Displayable(void) const
{
  return (false);
}

hbs::Terminal::Terminal(const hbs::Timer	&timer,
			const std::string	&name,
			const std::string	&pos)
  : AComponent<1>(timer, name, pos), AComponent<9>(timer, name, pos), Output(timer, name, pos),
    last_tick(0),
    c('\0')
{}

hbs::Terminal::~Terminal(void)
{}

