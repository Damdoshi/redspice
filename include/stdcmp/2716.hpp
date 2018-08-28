// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_2716_HPP__
# define			__NTS_2716_HPP__
# include			"Memory.hpp"

namespace			hbs
{
  class				CMP_2716 : public hbs::Memory<2048, 24>
  {
  private:
    const std::string		&GetType(void) const;

  public:
    hbs::Tristate		Compute(size_t				n);

    CMP_2716(const hbs::Timer	&timer,
	     const std::string	&file);
    virtual ~CMP_2716(void);
  };
}

#endif	//			__NTS_2716_HPP__
