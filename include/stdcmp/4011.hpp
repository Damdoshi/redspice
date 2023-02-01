// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__HBS_4011_HPP__
# define			__HBS_4011_HPP__
# include			"4081.hpp"

namespace			hbs
{
  class				CMP_4011 : public CMP_4081
  {
  private:
    const std::string		&GetType(void) const;

  protected:
    virtual hbs::Tristate	Logic(hbs::Tristate		a,
				      hbs::Tristate		b) const;

  public:
    CMP_4011(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&pos);
    virtual ~CMP_4011(void);
  };
}

#endif	//			__HBS_4011_HPP__
