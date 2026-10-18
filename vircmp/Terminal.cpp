// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		<unistd.h>
#include                <cerrno>
#include                <csignal>
#include                <pthread.h>
#include                <stdexcept>
#include                <algorithm>
#include		"Terminal.hpp"

// The library must not change its host process's SIGPIPE disposition.
static bool write_terminal_byte(char c)
{
  sigset_t mask, oldmask, pending;
  sigemptyset(&mask);
  sigaddset(&mask, SIGPIPE);
  int error = pthread_sigmask(SIG_BLOCK, &mask, &oldmask);
  if (error != 0)
    return (false);
  if (sigpending(&pending) < 0)
    { pthread_sigmask(SIG_SETMASK, &oldmask, NULL); return (false); }
  bool already_pending = (sigismember(&pending, SIGPIPE) == 1);
  ssize_t count;
  do count = write(STDOUT_FILENO, &c, 1); while (count < 0 && errno == EINTR);
  int saved_errno = errno;
  if (count < 0 && saved_errno == EPIPE && !already_pending)
    {
      const struct timespec timeout = {0, 0};
      while (sigtimedwait(&mask, NULL, &timeout) < 0 && errno == EINTR) {}
    }
  pthread_sigmask(SIG_SETMASK, &oldmask, NULL);
  errno = saved_errno;
  return (count == 1);
}

const std::string	&hbs::Terminal::GetType(void) const
{
  return (hbs::AComponent<9>::type = "Terminal");
}

const std::string	&hbs::Terminal::GetName(void) const
{
  return (hbs::AComponent<9>::GetName());
}

void			hbs::Terminal::SetName(const std::string &name)
{
  hbs::AComponent<9>::SetName(name);
  hbs::AComponent<1>::SetName(name);
}

void			hbs::Terminal::DisconnectFrom(const hbs::IComponent *component)
{
  hbs::AComponent<9>::DisconnectFrom(component);
  hbs::AComponent<1>::DisconnectFrom(component);
}

size_t			hbs::Terminal::GetPinCount(void) const
{
  return (hbs::AComponent<9>::GetPinCount());
}

hbs::Position		hbs::Terminal::GetPosition(void) const
{
  return (hbs::AComponent<9>::GetPosition());
}

hbs::Position		hbs::Terminal::GetPinPosition(size_t		pin) const
{
  if (pin < 1 || pin > 9)
    throw hbs::BadPin(GetType() + ": Bad pin.");
  return (LocalToWorld({0, (double)(pin - 1)}));
}

bool			hbs::Terminal::IsUnder(const hbs::Screen	&screen,
					       const t_bunny_position	&pos) const
{
  (void)screen;
  Position p = WorldToLocal({double(pos.x), double(pos.y)});
  return p.x >= -.5 && p.x <= 2 && p.y >= -1 && p.y <= 9;
}

void			hbs::Terminal::Draw(hbs::Screen			&screen) const
{
  Position a = LocalToWorld({1, 0}), b = LocalToWorld({2, 9});
  screen.Square({std::min(a.x, b.x), std::min(a.y, b.y)},
                {std::abs(b.x-a.x), std::abs(b.y-a.y)}, hbs::Screen::White);
  for (double i = 0; i < 9; ++i)
    {
      screen.Line(LocalToWorld({0, i}),
		  LocalToWorld({1, i}),
		  hbs::Screen::White);
      screen.Circle(LocalToWorld({0, i}),
		    hbs::Size{0.5, 0.5}, hbs::Screen::Teal, true);
      screen.Circle(LocalToWorld({0, i}),
		    hbs::Size{0.5, 0.5}, hbs::Screen::White, false);
    }
  screen.Text(LocalToWorld({0, -1}), {10, 10}, hbs::Screen::White, GetType());
  char			buf[2] = {c, '\0'};

  screen.Text(LocalToWorld({1, 1}), {10, 10}, hbs::Screen::White, std::string(buf));
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

  if (n < 1 || n > 9)
    throw hbs::BadPin(GetType() + ": Bad pin.");
  hbs::Tristate strobe = hbs::AComponent<9>::GetPin(9);
  if (hbs::AComponent<9>::GetPrevious(9) == hbs::FALSE &&
      strobe == hbs::TRUE &&
      last_tick != hbs::AComponent<9>::timer.GetTime())
    {
      c = 0;
      last_tick = hbs::AComponent<9>::timer.GetTime();
      for (i = 0; i < 8; ++i)
	if (AComponent<9>::GetPin(i + 1) == hbs::TRUE)
	  c = (char)((unsigned char)c | (1u << i));
      if (!write_terminal_byte(c))
        throw std::runtime_error("Cannot write terminal output.");
    }
  else
    for (i = 1; i <= 8; ++i)
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

