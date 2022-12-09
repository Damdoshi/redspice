// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__HBS_4081_HPP__
# define			__HBS_4081_HPP__
# include			"AGate.hpp"

namespace			hbs
{
  class				CMP_4081 : public AGate
  {
  private:
    const std::string		&GetType(void) const;

  protected:
    virtual hbs::Tristate	Logic(hbs::Tristate		a,
				      hbs::Tristate		b) const;

  public:
    CMP_4081(const hbs::Timer	&timer,
	     const std::string	&pos);
    virtual ~CMP_4081(void);
  };
}

#endif	//			__HBS_4081_HPP__
