// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__HBS_4017_HPP__
# define			__HBS_4017_HPP__
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

    CMP_4017(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&pos);
    virtual ~CMP_4017(void);
  };
}

#endif	//			__HBS_4017_HPP__
