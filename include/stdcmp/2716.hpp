// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_2716_HPP__
# define			__NTS_2716_HPP__
# include			"Memory.hpp"

namespace			nts
{
  class				CMP_2716 : public nts::Memory<2048, 24>
  {
  private:
    const std::string		&GetType(void) const;

  public:
    nts::Tristate		Compute(size_t				n);

    CMP_2716(const nts::Timer	&timer,
	     const std::string	&file);
    virtual ~CMP_2716(void);
  };
}

#endif	//			__NTS_2716_HPP__
