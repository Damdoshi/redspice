// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_4001_HPP__
# define			__NTS_4001_HPP__
# include			"4071.hpp"

namespace			hbs
{
  class				CMP_4001 : public CMP_4071
  {
  private:
    const std::string		&GetType(void) const;

  protected:
    virtual hbs::Tristate	Logic(hbs::Tristate		a,
				      hbs::Tristate		b) const;

  public:
    CMP_4001(const hbs::Timer	&timer);
    virtual ~CMP_4001(void);
  };
}

#endif	//			__NTS_4001_HPP__
