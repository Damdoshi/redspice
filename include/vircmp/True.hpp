// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_TRUE_HPP__
# define			__NTS_TRUE_HPP__
# include			"AComponent.hpp"

namespace			nts
{
  class				True : public nts::AComponent<1>
  {
  private:
    const std::string		&GetType(void) const;

  public:
    nts::Tristate		Compute(size_t		pin = 1);

    True(nts::Timer		&timer);
    virtual ~True(void);
  };
}

#endif	//			__NTS_TRUE_HPP__
