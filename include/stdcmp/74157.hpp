// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef				__HBS_74157_HPP__
# define			__HBS_74157_HPP__
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_74157 : public hbs::AComponent<16>
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
    virtual bool		DrivesPin(size_t		n) const;

  public:
    virtual hbs::Tristate	Compute(size_t			n);

    CMP_74157(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&pos);
    virtual ~CMP_74157(void);
  };
}

#endif	//			__HBS_74157_HPP__
