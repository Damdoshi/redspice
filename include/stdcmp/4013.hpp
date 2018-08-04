// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_4013_HPP__
# define			__NTS_4013_HPP__
# include			<bitset>
# include			"AComponent.hpp"

namespace			nts
{
  class				CMP_4013 : public nts::AComponent<14>
  {
  private:
    const std::string		&GetType(void) const;
    nts::Tristate		val[2];
    size_t			last_tick[2];

  public:
    nts::Tristate		Compute(size_t				n);

    CMP_4013(const nts::Timer	&timer);
    virtual ~CMP_4013(void);
  };
}

#endif	//			__NTS_4013_HPP__
