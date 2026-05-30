// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__HBS_4514_HPP__
# define			__HBS_4514_HPP__
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_4514 : public hbs::AComponent<24>
  {
  private:
    const std::string		&GetType(void) const;

  public:
    virtual bool			DrivesPin(size_t			n) const { return ((n >= 4 && n <= 11) || (n >= 13 && n <= 20)); }

    hbs::Tristate		Compute(size_t				n);

    CMP_4514(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&pos);
    virtual ~CMP_4514(void);
  };
}

#endif	//			__HBS_4514_HPP__
