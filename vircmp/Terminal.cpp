// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		<unistd.h>
#include		"Terminal.hpp"

const std::string	&hbs::Terminal::GetType(void) const
{
  return (hbs::AComponent<9>::type = typeid(*this).name());
}

void			hbs::Terminal::SetLink(size_t		a,
					     hbs::IComponent	&b,
					     size_t		c)
{
  hbs::AComponent<9>::SetLink(a, b, c);
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
      if (write(2, &c, 1)) {}
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

hbs::Terminal::Terminal(const hbs::Timer	&timer)
  : AComponent<1>(timer), AComponent<9>(timer), Output(timer),
    last_tick(0),
    c('\0')
{}

hbs::Terminal::~Terminal(void)
{}

