// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef				__HBS_4095_HPP__
# define			__HBS_4095_HPP__
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_4095 : public hbs::AComponent<16>
  {
  private:
    const std::string		&GetType(void) const;
    std::string			canonical;
    std::string			display;
    hbs::Tristate		val[2];
    size_t			last_tick[2];

    void			Update(size_t idx, size_t clk, size_t reset, size_t k, size_t j, size_t set);

  public:
    static bool			TypeMatches(const std::string	&type);
    static hbs::IComponent	*Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position);
    virtual std::string		GetDisplayType(void) const;
    virtual bool		DrivesPin(size_t n) const { return (n == 1 || n == 2 || n == 14 || n == 15); }

  public:
    hbs::Tristate		Compute(size_t			n);

    CMP_4095(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&pos,
	     const std::string	&canonical,
	     const std::string	&display);
    virtual ~CMP_4095(void);
  };
}

#endif	//			__HBS_4095_HPP__
