// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__AGATE_HPP__
# define			__AGATE_HPP__
# include			"AComponent.hpp"

namespace			hbs
{
  class				AGate : public hbs::AComponent<14>
  {
  protected:
    virtual hbs::Tristate	Logic(hbs::Tristate		a,
				      hbs::Tristate		b) const = 0;

    hbs::Tristate		Compute(size_t			n);

    AGate(const hbs::Timer	&timer);
    virtual ~AGate(void);
  };
}

#endif	//			__AGATE_HPP__
