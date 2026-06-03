// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef				__HBS_74158_HPP__
# define			__HBS_74158_HPP__
# include			"74157.hpp"

namespace			hbs
{
  class				CMP_74158 : public hbs::CMP_74157
  {
  private:
    const std::string		&GetType(void) const;

  protected:
    virtual hbs::Tristate	DisabledValue(void) const;
    virtual hbs::Tristate	OutputValue(hbs::Tristate v) const;

  public:
    static bool			TypeMatches(const std::string	&type);
    static hbs::IComponent	*Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position);
    virtual std::string		GetDisplayType(void) const;

    CMP_74158(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&pos);
    virtual ~CMP_74158(void);
  };
}

#endif	//			__HBS_74158_HPP__
