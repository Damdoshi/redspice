// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__HBS_CLOCK_HPP__
# define			__HBS_CLOCK_HPP__
# include			"Input.hpp"

namespace			hbs
{
  class				Clock : public hbs::Input
  {
  private:
    const std::string		&GetType(void) const;

  public:
    virtual hbs::Tristate	Compute(size_t		pin_num_this = 1);

    Clock(hbs::Timer		&timer,
	  const std::string	&value = "",
	  const std::string	&pos = "");
    virtual ~Clock(void);
  };
}

#endif	//			__HBS_CLOCK_HPP__
