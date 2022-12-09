// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__HBS_4094_HPP__
# define			__HBS_4094_HPP__
# include			<bitset>
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_4094 : public hbs::AComponent<16>
  {
  private:
    const std::string		&GetType(void) const;
    std::bitset<8>		value;
    bool			qns;
    size_t			last_tick_up;
    size_t			last_tick_down;

  public:
    hbs::Tristate		Compute(size_t				n);

    CMP_4094(const hbs::Timer	&timer,
	     const std::string	&pos);
    virtual ~CMP_4094(void);
  };
}

#endif	//			__HBS_4094_HPP__
