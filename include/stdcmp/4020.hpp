// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef				__HBS_4020_HPP__
# define			__HBS_4020_HPP__
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_4020 : public hbs::AComponent<16>
  {
  private:
    const std::string		&GetType(void) const;
    unsigned int		value;
    size_t			last_tick;

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

    CMP_4020(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&pos);
    virtual ~CMP_4020(void);
  };
}

#endif	//			__HBS_4020_HPP__
