// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__HBS_TRUE_HPP__
# define			__HBS_TRUE_HPP__
# include			"AComponent.hpp"

namespace			hbs
{
  class				True : public hbs::AComponent<1>
  {
  private:
    const std::string		&GetType(void) const;

  public:
    hbs::Tristate		Compute(size_t		pin = 1);
    virtual bool			DrivesPin(size_t		pin) const { return (pin == 1); }

    True(hbs::Timer		&timer,
	 const std::string	&name,
	 const std::string	&pos);
    virtual ~True(void);
  };
}

#endif	//			__HBS_TRUE_HPP__
