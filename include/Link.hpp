// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Pentacle Technologie 2008-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#ifndef					__LINK_HPP__
# define				__LINK_HPP__
# include				<list>
# include				<map>
# include				"IComponent.hpp"
# include				"ILink.hpp"
# include				"Screen.hpp"

namespace				hbs
{
  struct				Link : public ILink
  {
    IComponent				*first;
    size_t				second;

    typedef std::list<
      std::pair<hbs::Screen::Position, hbs::ILink::Layer>
      >					Positions;
    Positions				third;
    void				Draw(hbs::Screen		&screen,
					     const IComponent		&origin,
					     size_t			ori_pin) const;
    
    std::pair<int, int>			GetStep(size_t			i) const
    {
      auto				it = third.begin();
      
      for (; it != third.end() && i + 1 > 0; ++it, --i);
      if (it == third.end())
	throw 0;
      return (std::pair<int, int>(round(it->first.x), round(it->first.y)));
    }
    size_t				GetNbrSteps(void) const
    {
      return (third.size());
    }
    const IComponent			*GetTarget(void) const
    {
      return (first);
    }
    size_t				GetTargetPin(void) const
    {
      return (second);
    }
    Link(const hbs::Screen::Position	&from,
	 const hbs::Screen::Position	&to,
	 IComponent			*icom,
	 size_t				pin,
	 const std::string		&pos,
	 bool				rev = false);
    ~Link(void) {}
  };
}

#endif	//				__LINK_HPP__

