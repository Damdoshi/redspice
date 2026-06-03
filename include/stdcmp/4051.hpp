// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef				__HBS_4051_HPP__
# define			__HBS_4051_HPP__
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_4051 : public hbs::AComponent<16>
  {
  private:
    const std::string		&GetType(void) const;
    size_t			SelectedChannel(void);
    hbs::Tristate		SwitchValue(size_t n);

  public:
    static bool			TypeMatches(const std::string &type);
    static hbs::IComponent	*Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position);
    virtual std::string		GetDisplayType(void) const;
    virtual bool		DrivesPin(size_t n) const;

  public:
    hbs::Tristate		Compute(size_t n);

    CMP_4051(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&pos);
    virtual ~CMP_4051(void);
  };
}

#endif	//			__HBS_4051_HPP__
