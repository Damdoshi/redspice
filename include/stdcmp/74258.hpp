// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef				__HBS_74258_HPP__
# define			__HBS_74258_HPP__
# include			"74158.hpp"

namespace			hbs
{
  class				CMP_74258 : public hbs::CMP_74158
  {
  private:
    const std::string		&GetType(void) const;

  protected:
    virtual hbs::Tristate	DisabledValue(void) const;

  public:
    static bool			TypeMatches(const std::string	&type);
    static hbs::IComponent	*Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position);
    virtual std::string		GetDisplayType(void) const;

    CMP_74258(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&pos);
    virtual ~CMP_74258(void);
  };
}

#endif	//			__HBS_74258_HPP__
