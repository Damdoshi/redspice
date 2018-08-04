// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_4040_HPP__
# define			__NTS_4040_HPP__
# include			<bitset>
# include			"AComponent.hpp"

namespace			nts
{
  class				CMP_4040 : public nts::AComponent<16>
  {
  private:
    const std::string		&GetType(void) const;
    unsigned int		value;
    size_t			last_tick;

  public:
    nts::Tristate		Compute(size_t				n);

    CMP_4040(const nts::Timer	&timer);
    virtual ~CMP_4040(void);
  };
}

#endif	//			__NTS_4040_HPP__
