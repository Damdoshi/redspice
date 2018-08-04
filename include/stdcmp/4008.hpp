// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_4008_HPP__
# define			__NTS_4008_HPP__
# include			"AComponent.hpp"

namespace			nts
{
  class				CMP_4008 : public nts::AComponent<16>
  {
  private:
    const std::string		&GetType(void) const;

  public:
    nts::Tristate		Compute(size_t				n);

    CMP_4008(const nts::Timer	&timer);
    virtual ~CMP_4008(void);
  };
}

#endif	//			__NTS_4008_HPP__
