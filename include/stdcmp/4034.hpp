// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef				__HBS_4034_HPP__
# define			__HBS_4034_HPP__
# include			<bitset>
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_4034 : public hbs::AComponent<24>
  {
  private:
    const std::string		&GetType(void) const;
    std::bitset<8>		value;
    size_t			last_tick;

    bool			OutputEnabled(void);
    void			Tick(void);

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
    hbs::Tristate		Compute(size_t			n);

    CMP_4034(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&pos);
    virtual ~CMP_4034(void);
  };
}

#endif	//			__HBS_4034_HPP__
