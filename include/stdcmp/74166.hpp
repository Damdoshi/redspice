// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef				__HBS_74166_HPP__
# define			__HBS_74166_HPP__
# include			<bitset>
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_74166 : public hbs::AComponent<16>
  {
  private:
    const std::string		&GetType(void) const;
    std::bitset<8>		value;
    size_t			last_tick;

    void			Tick(void);
    void			LoadParallel(void);

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

    CMP_74166(const hbs::Timer	&timer,
	      const std::string	&name,
	      const std::string	&pos);
    virtual ~CMP_74166(void);
  };
}

#endif	//			__HBS_74166_HPP__
