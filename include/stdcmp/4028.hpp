// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef				__HBS_4028_HPP__
# define			__HBS_4028_HPP__
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_4028 : public hbs::AComponent<16>
  {
  private:
    const std::string		&GetType(void) const;

  public:
    static bool			TypeMatches(const std::string	&type);
    static hbs::IComponent	*Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position);
    virtual std::string		GetDisplayType(void) const;

  public:
    virtual bool			DrivesPin(size_t		n) const;
    hbs::Tristate		Compute(size_t			n);

    CMP_4028(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&position);
    virtual ~CMP_4028(void);
  };
}

#endif	//			__HBS_4028_HPP__
