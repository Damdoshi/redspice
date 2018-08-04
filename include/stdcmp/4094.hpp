// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_4094_HPP__
# define			__NTS_4094_HPP__
# include			<bitset>
# include			"AComponent.hpp"

namespace			nts
{
  class				CMP_4094 : public nts::AComponent<16>
  {
  private:
    const std::string		&GetType(void) const;
    std::bitset<8>		value;
    bool			qns;
    size_t			last_tick_up;
    size_t			last_tick_down;

  public:
    nts::Tristate		Compute(size_t				n);

    CMP_4094(const nts::Timer	&timer);
    virtual ~CMP_4094(void);
  };
}

#endif	//			__NTS_4094_HPP__
