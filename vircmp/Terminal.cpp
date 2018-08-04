// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		<unistd.h>
#include		"Terminal.hpp"

const std::string	&nts::Terminal::GetType(void) const
{
  return (nts::AComponent<9>::type = typeid(*this).name());
}

void			nts::Terminal::SetLink(size_t		a,
					     nts::IComponent	&b,
					     size_t		c)
{
  nts::AComponent<9>::SetLink(a, b, c);
}

void			nts::Terminal::Dump(void) const
{
  nts::AComponent<9>::Dump();
}

nts::Tristate		nts::Terminal::Compute(size_t		n)
{
  int			i;

  (void)n;
  if (nts::AComponent<9>::GetPrevious(9) == nts::FALSE &&
      nts::AComponent<9>::GetPin(9) == nts::TRUE &&
      last_tick != nts::AComponent<9>::timer.GetTime())
    {
      c = 0;
      last_tick = nts::AComponent<9>::timer.GetTime();
      for (i = 0; i < 7; ++i)
	c |= AComponent<9>::GetPin(i + 1) << i;
      if (write(2, &c, 1)) {}
    }
  else
    for (i = 1; i < 8; ++i)
      AComponent<9>::GetPin(i);
  return (nts::TRUE);
}

bool			nts::Terminal::Displayable(void) const
{
  return (false);
}

nts::Terminal::Terminal(const nts::Timer	&timer)
  : AComponent<1>(timer), AComponent<9>(timer), Output(timer),
    last_tick(0),
    c('\0')
{}

nts::Terminal::~Terminal(void)
{}

