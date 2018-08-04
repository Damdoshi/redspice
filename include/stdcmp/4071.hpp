// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_4071_HPP__
# define			__NTS_4071_HPP__
# include			"AGate.hpp"

namespace			nts
{
  class				CMP_4071 : public AGate
  {
  private:
    const std::string		&GetType(void) const;

  protected:
    virtual nts::Tristate	Logic(nts::Tristate		a,
				      nts::Tristate		b) const;

  public:
    CMP_4071(const nts::Timer	&timer);
    virtual ~CMP_4071(void);
  };
}

#endif	//			__NTS_4071_HPP__
