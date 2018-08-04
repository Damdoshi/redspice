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

namespace				nts
{
  template <int				Pin>
  class					AComponent: public virtual nts::IComponent
  {
  protected:
    virtual const std::string		&GetType(void) const = 0;
    const nts::Timer			&timer;

    typedef std::pair<IComponent*, size_t> Link;
    typedef std::map<size_t, Tristate>	PinState;
    typedef PinState::iterator		Preset;

    std::map<size_t, std::list<Link> >	links;
    std::map<size_t, PinState>		timeline;

    mutable std::string			type;

    void				CleanOld(void)
    {
      //std::cout << timeline.size() << std::endl;
      if (timer.GetTime() > 4)
	timeline.erase(timeline.begin(), timeline.find(timer.GetTime() - 3));
    }

    //// Check if the pin was already computed at this current timeline
    /// This may be a preset, but that the purpose of presets
    bool				AlreadyComputed(size_t		n,
							nts::Tristate	&out)
    {
      std::map<size_t, PinState>::iterator				pin;
      std::map<size_t, Tristate>::iterator				tri;

      /// For memory sake
      CleanOld();

      /// Bad pin
      if (n < 1 || n > Pin)
	throw nts::BadPin(GetType() + ":Bad pin.");

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
	    timeline[timer.GetTime()][n] = nts::UNDEFINED;
	  else if ((itx = it->second.find(n)) == it->second.end())
	    it->second[n] = nts::UNDEFINED;
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
	  timeline[timer.GetTime()][n] = nts::UNDEFINED;
	  return ;
	}
      timeline[timer.GetTime()][n] = timeline[timer.GetTime() - 1][n];
    }

    nts::Tristate			GetPrevious(size_t		n)
    {
      std::map<size_t,  PinState>::reverse_iterator			it;

      if (timer.GetTime() == 0)
	return (nts::UNDEFINED);
      return (timeline[timer.GetTime() - 1][n]);
    }

    //// Get a pin value
    nts::Tristate			GetPin(size_t			n)
    {
      std::map<size_t, std::list<Link> >::iterator			lnk;
      std::list<Link>::iterator						it;
      nts::Tristate							out;
      nts::Tristate							tmp;

      /// If not linked
      if ((lnk = links.find(n)) == links.end())
	return (timeline[timer.GetTime()][n] = nts::UNDEFINED);

      /// Linked
      it = lnk->second.begin();
      out = it->first->Compute(it->second);
      for (++it; it != lnk->second.end(); ++it)
	{
	  tmp = it->first->Compute(it->second);
	  if (out == nts::UNDEFINED)
	    out = tmp;
	  else if (tmp != nts::UNDEFINED)
	    return (timeline[timer.GetTime()][n] = nts::BROKEN);
	}
      return (timeline[timer.GetTime()][n] = out);
    }

  public:
    void				SetLink(size_t			pin_num_this,
						nts::IComponent		&component,
						size_t			pin_num_target)
    {
      std::map<size_t, std::list<Link> >::iterator			it;
      std::list<Link>::iterator						itx;

      /// If the pin is out of bound
      if (pin_num_this > Pin || pin_num_this == 0)
	throw nts::BadPin(GetType() + ": Bad pin.");

      /// Already linked
      if ((it = links.find(pin_num_this)) != links.end())
	for (itx = it->second.begin(); itx != it->second.end(); ++itx)
	  if (itx->first == &component && itx->second == pin_num_target)
	    return ;

      /// Linked and signal to the other component the link
      links[pin_num_this].push_back(std::pair<nts::IComponent*, size_t>(&component, pin_num_target));
      component.SetLink(pin_num_target, *this, pin_num_this);
    }

    void				Dump(void) const
    {
      std::map<size_t, Tristate>::const_iterator it;
      std::map<size_t, PinState>::const_iterator itx;

      if ((itx = timeline.find(timer.GetTime())) == timeline.end())
	{
	  std::cout << "Not simulated yet." << std::endl;
	  return ;
	}
      for (it = itx->second.begin(); it != itx->second.end(); ++it)
	{
	  std::cout << "Pin " << it->first << ": ";
	  if (it->second == nts::UNDEFINED)
	    std::cout << "U";
	  else if (it->second == nts::FALSE)
	    std::cout << "0";
	  else
	    std::cout << "1";
	  std::cout << std::endl;
	}
    }

    AComponent(const nts::Timer		&tim)
      : timer(tim)
    {}
    virtual ~AComponent(void) {}
  };
}

#endif	//			__ACOMPONENT_HPP__
