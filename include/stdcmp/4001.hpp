// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_4001_HPP__
# define			__NTS_4001_HPP__
# include			"4071.hpp"

namespace			nts
{
  class				CMP_4001 : public CMP_4071
  {
  private:
    const std::string		&GetType(void) const;

  protected:
    virtual nts::Tristate	Logic(nts::Tristate		a,
				      nts::Tristate		b) const;

  public:
    CMP_4001(const nts::Timer	&timer);
    virtual ~CMP_4001(void);
  };
}

#endif	//			__NTS_4001_HPP__
