// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Pentacle Technologie 2008-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#ifndef					__ACOMPONENT_HPP__
# define				__ACOMPONENT_HPP__
# include				<iostream>
# include				<typeinfo>
# include				<list>
# include				<map>
# include				<cmath>
# include				"IComponent.hpp"
# include				"Exception.hpp"
# include				"Timer.hpp"
# include				"Screen.hpp"
# include				"Parsing.hpp"
# include                               "Signal.hpp"

namespace				hbs
{
  template <int				Pin>
  class					AComponent: public virtual hbs::IComponent
  {
  protected:
    virtual const std::string		&GetType(void) const = 0;
    const hbs::Timer			&timer;
    std::string			name;

    typedef std::map<size_t, Tristate>	PinState;
    typedef PinState::iterator		Preset;

    struct				Connection
    {
      hbs::IComponent			*component;
      size_t				pin;
    };

    std::map<size_t, std::list<Connection> > links;
    std::map<size_t, PinState>		timeline;
    //float				x;
    //float				y;

    mutable std::string			type;

    // Drawing reads the last observed state without advancing the simulation.
    virtual hbs::Tristate GetPinDisplayState(size_t pin) const
    {
      auto last = timeline.rbegin();
      if (last == timeline.rend()) return hbs::UNDEFINED;
      auto state = last->second.find(pin);
      return state == last->second.end() ? hbs::UNDEFINED : state->second;
    }

    hbs::Position			position;

    void				CleanOld(void)
    {
      if (timer.GetTime() > 4)
	timeline.erase(timeline.begin(), timeline.lower_bound(timer.GetTime() - 3));
    }

    //// Check if the pin was already computed at this current timeline
    /// This may be a preset, but that the purpose of presets
    bool				AlreadyComputed(size_t		n,
							hbs::Tristate	&out)
    {
      std::map<size_t, PinState>::iterator				pin;
      std::map<size_t, Tristate>::iterator				tri;

      /// For memory sake
      CleanOld();

      /// Bad pin
      if (n < 1 || n > Pin)
	throw hbs::BadPin(GetType() + ":Bad pin.");

      /// If already computed
      if ((pin = timeline.find(timer.GetTime())) != timeline.end())
	if ((tri = pin->second.find(n)) != pin->second.end())
	  {
	    out = tri->second;
	    return (true);
	  }

      return (false);
    }

    //// Preset a value to the previous state
    /// Use UNDEFINED if there is no previous state
    Preset				PresetOutput(size_t		n)
    {
      std::map<size_t, PinState>::iterator	it;
      std::map<size_t, Tristate>::iterator	itx;

      if (timer.GetTime() == 0)
	{
	  if ((it = timeline.find(timer.GetTime())) == timeline.end())
	    timeline[timer.GetTime()][n] = hbs::UNDEFINED;
	  else if ((itx = it->second.find(n)) == it->second.end())
	    it->second[n] = hbs::UNDEFINED;
	  else
	    return (itx);
	  return (timeline[timer.GetTime()].find(n));
	}
      KeepOld(n);
      return (timeline[timer.GetTime()].find(n));
    }

    //// Set to the current state the old state
    void				KeepOld(size_t			n)
    {
      std::map<size_t,  PinState>::reverse_iterator			it;

      if ((it = timeline.rbegin()) == timeline.rend())
	{
	  timeline[timer.GetTime()][n] = hbs::UNDEFINED;
	  return ;
	}
      timeline[timer.GetTime()][n] = GetPrevious(n);
    }

    hbs::Tristate			GetPrevious(size_t		n)
    {
      if (timer.GetTime() == 0)
	return (hbs::UNDEFINED);
      auto previous = timeline.find(timer.GetTime() - 1);
      if (previous == timeline.end())
        return (hbs::UNDEFINED);
      auto pin = previous->second.find(n);
      return (pin == previous->second.end() ? hbs::UNDEFINED : pin->second);
    }

  public:
    //// Get a pin value
    virtual hbs::Tristate		GetPin(size_t			n)
    {
      if (n < 1 || n > Pin)
        throw hbs::BadPin(GetType() + ": Bad pin.");
      CleanOld();
      return (timeline[timer.GetTime()][n] = hbs::ResolveSignal(*this, n));
    }

    void GetConnections(size_t n, std::vector<SignalConnection> &out) const override
    {
      auto link = links.find(n);
      if (link != links.end())
        for (const Connection &connection : link->second)
          out.push_back({connection.component, connection.pin});
    }

    void				SetLink(size_t			pin_num_this,
						hbs::IComponent		&component,
						size_t			pin_num_target,
						const std::string	&pos)
    {
      typename std::map<size_t, std::list<Connection> >::iterator	it;
      typename std::list<Connection>::iterator			itx;

      /// If the pin is out of bound
      if (pin_num_this > Pin || pin_num_this == 0)
	throw hbs::BadPin(GetType() + ": Bad pin.");
      if (pin_num_target == 0 ||
          (component.GetType() != "wire" && component.GetType() != "track" &&
           pin_num_target > component.GetPinCount()))
        throw hbs::BadPin(component.GetType() + ": Bad target pin.");

      /// Already linked
      if ((it = links.find(pin_num_this)) != links.end())
	for (itx = it->second.begin(); itx != it->second.end(); ++itx)
	  if (itx->component == &component && itx->pin == pin_num_target)
	    return ;

      links[pin_num_this].push_back({&component, pin_num_target});
      if (pos.empty() || pos[0] != '!')
	component.SetLink(pin_num_target, *this, pin_num_this, "!" + pos);
    }

    bool				IsUnder(const hbs::Screen	&screen,
						const t_bunny_position	&c) const
    {
      hbs::Position local = WorldToLocal({double(c.x), double(c.y)});
      if (Pin == 1)
	{
	  return (std::hypot(local.x, local.y) < 2);
	}
      if (Pin == 2)
        return (local.x >= 0 && local.x <= 1 && local.y >= 0 && local.y <= 3);
      if (Pin == 3)
	{
	  return (false);
	}
      (void)screen;
      return (local.x >= 0 && local.x < 3 && local.y >= 0 && local.y < Pin / 2);
    }

    const std::string			&GetName(void) const
    {
      return (name);
    }

    void				SetName(const std::string &n)
    {
      name = n;
    }

    void				DisconnectFrom(const hbs::IComponent *component)
    {
      for (typename std::map<size_t, std::list<Connection> >::iterator it = links.begin();
           it != links.end(); ++it)
	for (typename std::list<Connection>::iterator jt = it->second.begin();
	     jt != it->second.end();)
	  if (jt->component == component)
	    jt = it->second.erase(jt);
	  else
	    ++jt;
    }

    size_t				GetPinCount(void) const
    {
      return (Pin);
    }

    hbs::Position			GetPosition(void) const
    {
      return (position);
    }

    void				Move(const hbs::Position &pos)
    {
      position.x += pos.x;
      position.y += pos.y;
    }

    virtual hbs::Position		GetPinPosition(size_t		pin) const
    {
      if (pin > Pin || pin == 0)
	throw hbs::BadPin(GetType() + ": Bad pin.");
      if (Pin == 1)
	return (position);
      if (Pin == 2)
	{
	  if (pin == 1)
	    return (position);
	  return (LocalToWorld({0, 3}));
	}
      if (Pin == 3)
	{
	  if (pin == 1)
	    return (LocalToWorld({-1, 0}));
	  if (pin == 2)
	    return (LocalToWorld({0, -1}));
	  return (LocalToWorld({0, 1}));
	}
      pin -= 1;
      if (pin < Pin / 2)
	return (LocalToWorld({0, (double)pin}));
      return (LocalToWorld({3, Pin - (double)pin - 1}));
    }

    virtual void			Draw(hbs::Screen		&screen) const
    {
      if (Pin == 1)
	{
	  unsigned int col = hbs::Screen::TristateColor[GetPinDisplayState(1) - BROKEN];

	  screen.Circle(GetPinPosition(1), {0.7, 0.7}, col, true);
	  screen.Circle(GetPinPosition(1), {0.7, 0.7}, hbs::Screen::White, false);
	}
      else if (Pin == 2)
	{
          screen.Circle(LocalToWorld({0, 1}), RotatedSize({0.5, 2}), hbs::Screen::Green);

	  screen.Circle(GetPinPosition(1), {0.5, 0.5}, hbs::Screen::Teal, true);
	  screen.Circle(GetPinPosition(1), {0.5, 0.5}, hbs::Screen::White, false);
	  screen.Circle(GetPinPosition(2), {0.5, 0.5}, hbs::Screen::Teal, true);
	  screen.Circle(GetPinPosition(2), {0.5, 0.5}, hbs::Screen::White, false);
	  screen.Line(GetPinPosition(1), GetPinPosition(2), hbs::Screen::White);
	}
      else if (Pin == 3)
	{
	  screen.Circle(position, {0.5, 0.5}, hbs::Screen::Teal);
	  screen.Line(position, GetPinPosition(1), hbs::Screen::White);
	  screen.Line(position, GetPinPosition(2), hbs::Screen::White);
	  screen.Line(position, GetPinPosition(3), hbs::Screen::White);
	}
      else
	{
	  for (int i = 0; i <= 2; ++i)
	    {
	      hbs::Position xx = QuarterTurn({0, (i - 1) / (double)screen.PinSize()}, GetRotation());
	      hbs::Position yy = QuarterTurn({(i - 1) / (double)screen.PinSize(), 0}, GetRotation());

	      // Horizontal
	      screen.Line(GetPinPosition(1) + xx, GetPinPosition(Pin) + xx, hbs::Screen::Teal);
	      screen.Line(GetPinPosition(Pin / 2) + xx, GetPinPosition(Pin / 2 + 1) + xx, hbs::Screen::Teal);

	      // Vertical
	      screen.Line(GetPinPosition(1) + yy, GetPinPosition(Pin / 2) + yy, hbs::Screen::Teal);
	      screen.Line(GetPinPosition(Pin / 2 + 1) + yy, GetPinPosition(Pin) + yy, hbs::Screen::Teal);
	    }

          for (size_t i = 1; i <= Pin; ++i)
            {
              unsigned int col = hbs::Screen::TristateColor[GetPinDisplayState(i) - BROKEN];

              screen.Circle(GetPinPosition(i), {0.4, 0.4}, col, true);
              screen.Circle(GetPinPosition(i), {0.4, 0.4}, hbs::Screen::White, false);
            }

          DrawLabel(screen, {1.5, Pin / 2 - 0.5}, {10, 10}, hbs::Screen::Teal, GetType());
          DrawLabel(screen, {1.5, -1.5}, {10, 10}, hbs::Screen::White, name);
	  return ;
	}

      DrawLabel(screen, {0, 0.75}, {10, 10}, hbs::Screen::Teal, GetType());
      DrawLabel(screen, {0, -1.5}, {10, 10}, hbs::Screen::White, name);
    }

    void				Dump(void) const
    {
      std::map<size_t, Tristate>::const_iterator it;
      std::map<size_t, PinState>::const_iterator itx;

      if ((itx = timeline.find(timer.GetTime())) == timeline.end())
	{
	  std::cerr << "Not simulated yet." << std::endl;
	  return ;
	}
      for (it = itx->second.begin(); it != itx->second.end(); ++it)
	std::cerr << "Pin " << it->first << ": " << it->second << std::endl;
    }

    AComponent(const hbs::Timer		&tim,
	       const std::string	&nam,
	       const std::string	&pos)
      : timer(tim),
        name(nam)
    {
      int				idy = 0;
      if (!ReadNumber(pos, idy, position.x))
	throw hbs::SyntaxError(pos);
      ReadWhitespace(pos, idy);
      ReadText(pos, idy, ",");
      if (!ReadNumber(pos, idy, position.y))
	throw hbs::SyntaxError(pos);
      ReadWhitespace(pos, idy);
      if ((size_t)idy != pos.size())
	throw hbs::SyntaxError(pos);
    }
    virtual ~AComponent(void) {}
  };
}

#endif	//			__ACOMPONENT_HPP__
