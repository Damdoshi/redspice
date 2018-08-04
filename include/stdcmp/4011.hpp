// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_4011_HPP__
# define			__NTS_4011_HPP__
# include			"4081.hpp"

namespace			nts
{
  class				CMP_4011 : public CMP_4081
  {
  private:
    const std::string		&GetType(void) const;

  protected:
    virtual nts::Tristate	Logic(nts::Tristate		a,
				      nts::Tristate		b) const;

  public:
    CMP_4011(const nts::Timer	&timer);
    virtual ~CMP_4011(void);
  };
}

#endif	//			__NTS_4011_HPP__
