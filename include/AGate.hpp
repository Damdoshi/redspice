// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__AGATE_HPP__
# define			__AGATE_HPP__
# include			"AComponent.hpp"

namespace			nts
{
  class				AGate : public nts::AComponent<14>
  {
  protected:
    virtual nts::Tristate	Logic(nts::Tristate		a,
				      nts::Tristate		b) const = 0;

    nts::Tristate		Compute(size_t			n);

    AGate(const nts::Timer	&timer);
    virtual ~AGate(void);
  };
}

#endif	//			__AGATE_HPP__
