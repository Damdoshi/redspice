// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef				__HBS_4556_HPP__
# define			__HBS_4556_HPP__
# include			"4555.hpp"

namespace			hbs
{
  class				CMP_4556 : public hbs::CMP_4555
  {
  protected:
    virtual const std::string	&GetType(void) const;
    virtual bool		ActiveLowOutputs(void) const { return (true); }

  public:
    static bool			TypeMatches(const std::string	&type);
    static hbs::IComponent	*Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position);
    virtual std::string		GetDisplayType(void) const;

    CMP_4556(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&position);
    virtual ~CMP_4556(void);
  };
}

#endif	//			__HBS_4556_HPP__
