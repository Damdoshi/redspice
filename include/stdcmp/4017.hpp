// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_4017_HPP__
# define			__NTS_4017_HPP__
# include			<bitset>
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_4017 : public hbs::AComponent<16>
  {
  private:
    const std::string		&GetType(void) const;
    std::bitset<10>		value;
    size_t			last_tick;

  public:
    hbs::Tristate		Compute(size_t				n);

    CMP_4017(const hbs::Timer	&timer);
    virtual ~CMP_4017(void);
  };
}

#endif	//			__NTS_4017_HPP__
