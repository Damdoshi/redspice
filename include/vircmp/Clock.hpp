// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_CLOCK_HPP__
# define			__NTS_CLOCK_HPP__
# include			"Input.hpp"

namespace			nts
{
  class				Clock : public nts::Input
  {
  private:
    const std::string		&GetType(void) const;

  public:
    virtual nts::Tristate	Compute(size_t		pin_num_this = 1);

    Clock(nts::Timer		&timer,
	  const std::string	&value = "");
    virtual ~Clock(void);
  };
}

#endif	//			__NTS_CLOCK_HPP__
