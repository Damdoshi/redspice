// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_FALSE_HPP__
# define			__NTS_FALSE_HPP__
# include			"AComponent.hpp"

namespace			nts
{
  class				False : public nts::AComponent<1>
  {
  private:
    const std::string		&GetType(void) const;

  public:
    nts::Tristate		Compute(size_t		pin = 1);

    False(nts::Timer		&timer);
    virtual ~False(void);
  };
}

#endif	//			__NTS_FALSE_HPP__
