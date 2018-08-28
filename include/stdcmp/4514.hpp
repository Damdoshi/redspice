// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_4514_HPP__
# define			__NTS_4514_HPP__
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_4514 : public hbs::AComponent<24>
  {
  private:
    const std::string		&GetType(void) const;

  public:
    hbs::Tristate		Compute(size_t				n);

    CMP_4514(const hbs::Timer	&timer);
    virtual ~CMP_4514(void);
  };
}

#endif	//			__NTS_4514_HPP__
