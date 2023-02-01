// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef					__ACOMPONENT_HPP__
# define				__ACOMPONENT_HPP__
# include				<iostream>
# include				<typeinfo>
# include				<list>
# include				<map>
# include				"IComponent.hpp"
# include				"Exception.hpp"
# include				"Timer.hpp"
# include				"Screen.hpp"
# include				"Parsing.hpp"

namespace				hbs
{
  struct				Link
  {
    enum				Layer
      {
	TOP,
	BOTTOM
      };
    IComponent				*first;
    size_t				second;

    typedef std::list<std::pair<hbs::Screen::Position, Layer> > Positions;
    Positions				third;
    void				Draw(hbs::Screen		&screen,
					     const IComponent		&origin,
					     size_t			ori_pin) const;
    Link(IComponent			*icom,
	 size_t				pin,
	 const std::string		&pos,
	 bool				rev = false);
    ~Link(void) {}
  };

  template <int				Pin>
  class					AComponent: public virtual hbs::IComponent
  {
  protected:
    virtual const std::string		&GetType(void) const = 0;
    const hbs::Timer			&timer;
    const std::string			name;

    typedef std::map<size_t, Tristate>	PinState;
    typedef PinState::iterator		Preset;

    std::map<size_t, std::list<Link> >	links;
    std::map<size_t, PinState>		timeline;
    //float				x;
    //float				y;

    mutable std::string			type;

    hbs::Screen::Position		position;
    bool				orientation;

    void				CleanOld(void)
    {
      if (timer.GetTime() > 4)
	timeline.erase(timeline.begin(), timeline.find(timer.GetTime() - 3));
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
      timeline[timer.GetTime()][n] = timeline[timer.GetTime() - 1][n];
    }

    hbs::Tristate			GetPrevious(size_t		n)
    {
      std::map<size_t,  PinState>::reverse_iterator			it;

      if (timer.GetTime() == 0)
	return (hbs::UNDEFINED);
      return (timeline[timer.GetTime() - 1][n]);
    }

    //// Get a pin value
    hbs::Tristate			GetPin(size_t			n)
    {
      typename std::map<size_t, std::list<Link> >::iterator		lnk;
      typename std::list<Link>::iterator				it;
      hbs::Tristate							out;
      hbs::Tristate							tmp;

      /// If not linked
      if ((lnk = links.find(n)) == links.end())
	return (timeline[timer.GetTime()][n] = hbs::UNDEFINED);

      /// Linked
      it = lnk->second.begin();
      out = it->first->Compute(it->second);
      for (++it; it != lnk->second.end(); ++it)
	{
	  tmp = it->first->Compute(it->second);
	  if (out == hbs::UNDEFINED)
	    out = tmp;
	  else if (tmp != hbs::UNDEFINED)
	    return (timeline[timer.GetTime()][n] = hbs::BROKEN);
	}
      return (timeline[timer.GetTime()][n] = out);
    }

  public:
    void				SetLink(size_t			pin_num_this,
						hbs::IComponent		&component,
						size_t			pin_num_target,
						const std::string	&pos)
    {
      typename std::map<size_t, std::list<Link> >::iterator		it;
      typename std::list<Link>::iterator				itx;

      /// If the pin is out of bound
      if (pin_num_this > Pin || pin_num_this == 0)
	throw hbs::BadPin(GetType() + ": Bad pin.");

      /// Already linked
      if ((it = links.find(pin_num_this)) != links.end())
	for (itx = it->second.begin(); itx != it->second.end(); ++itx)
	  if (itx->first == &component && itx->second == pin_num_target)
	    return ;

      /// Linked and signal to the other component the link
      if (pos[0] != '!')
	links[pin_num_this].push_back({&component, pin_num_target, pos});
      else
	links[pin_num_this].push_back({&component, pin_num_target, pos.substr(1), true});
      component.SetLink(pin_num_target, *this, pin_num_this, "!" + pos);
    }

    virtual hbs::Screen::Position	GetPinPosition(size_t		pin) const
    {
      if (Pin < Pin || pin == 0)
	throw hbs::BadPin(GetType() + ": Bad pin.");
      if (Pin == 1)
	return (position);
      if (Pin == 2)
	{
	  if (pin == 1)
	    return (position + hbs::Screen::Position{0, 0});
	  if (orientation)
	    return (position + hbs::Screen::Position{3, 0});
	  return (position + hbs::Screen::Position{0, 3});
	}
      if (Pin == 3)
	{
	  if (pin == 1)
	    return (position + hbs::Screen::Position{-1, 0});
	  if (pin == 2)
	    return (position + hbs::Screen::Position{0, -1});
	  return (position + hbs::Screen::Position{0, 1});
	}
      pin -= 1;
      if (pin < Pin / 2)
	return (position + hbs::Screen::Position{0, (double)pin});
      return (position + hbs::Screen::Position{3, Pin - (double)pin - 1});
    }

    virtual void			Draw(hbs::Screen		&screen) const
    {
      /// Draw links
      for (auto itx = links.begin(); itx != links.end(); ++itx)
	for (auto ity = itx->second.begin(); ity != itx->second.end(); ++ity)
	  ity->Draw(screen, *this, itx->first);

      if (Pin == 1)
	{
	  screen.Circle(GetPinPosition(1), {0.5, 0.5}, hbs::Screen::Teal, true);
	  screen.Circle(GetPinPosition(1), {0.5, 0.5}, hbs::Screen::White, false);
	}
      else if (Pin == 2)
	{
	  if (orientation)
	    screen.Circle(position + hbs::Screen::Position{1, 0}, {2, 0.5}, hbs::Screen::Green);
	  else
	    screen.Circle(position + hbs::Screen::Position{0, 1}, {0.5, 2}, hbs::Screen::Green);

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
	  for (size_t i = 1; i <= Pin; ++i)
	    {
	      screen.Circle(GetPinPosition(i), {0.25, 0.25}, hbs::Screen::Purple, true);
	      screen.Circle(GetPinPosition(i), {0.25, 0.25}, hbs::Screen::White, false);
	    }
	  for (size_t i = 0; i <= 2; ++i)
	    {
	      hbs::Screen::Position xx = {0, (i - 1) / (double)screen.PinSize()};
	      hbs::Screen::Position yy = {(i - 1) / (double)screen.PinSize(), 0};

	      // Horizontal
	      screen.Line(GetPinPosition(1) + xx, GetPinPosition(Pin) + xx, hbs::Screen::Teal);
	      screen.Line(GetPinPosition(Pin / 2) + xx, GetPinPosition(Pin / 2 + 1) + xx, hbs::Screen::Teal);

	      // Vertical
	      screen.Line(GetPinPosition(1) + yy, GetPinPosition(Pin / 2) + yy, hbs::Screen::Teal);
	      screen.Line(GetPinPosition(Pin / 2 + 1) + yy, GetPinPosition(Pin) + yy, hbs::Screen::Teal);
	    }

	  hbs::Screen::Size sz = screen.TextSize({10, 10}, GetType());

	  sz.x = (-sz.x / 2.0) / screen.PinSize() + 1.5;
	  sz.y = Pin / 2 - 0.5;
	  screen.Text(position + sz, {10, 10}, hbs::Screen::Teal, GetType());

	  sz = screen.TextSize({10, 10}, name);
	  sz.x = (-sz.x / 2.0) / screen.PinSize() + 1.5;
	  sz.y = -1.5;
	  screen.Text(position + sz, {10, 10}, hbs::Screen::White, name);
	  return ;
	}

      hbs::Screen::Size sz = screen.TextSize({10, 10}, GetType());

      sz.x = (-sz.x / 2.0) / screen.PinSize();
      sz.y = 0.75;
      screen.Text(position + sz, {10, 10}, hbs::Screen::Teal, GetType());

      sz = screen.TextSize({10, 10}, name);
      sz.x = (-sz.x / 2.0) / screen.PinSize();
      sz.y = -1.5;
      screen.Text(position + sz, {10, 10}, hbs::Screen::White, name);
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
      size_t				idx = 0;
      int				idy = 0;

      position.x = std::stof(pos, &idx);
      if (idx == 0)
	throw hbs::SyntaxError(pos);
      idy = idx;
      ReadWhitespace(pos, idy);
      idy += pos[idy] == ',';
      ReadWhitespace(pos, idy);
      idx = idy;
      position.y = std::stof(&pos[idx], &idx);
      if (idx == 0)
	throw hbs::SyntaxError(pos);
    }
    virtual ~AComponent(void) {}
  };
}

#endif	//			__ACOMPONENT_HPP__
