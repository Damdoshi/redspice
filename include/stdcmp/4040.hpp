// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__HBS_4040_HPP__
# define			__HBS_4040_HPP__
# include			<bitset>
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_4040 : public hbs::AComponent<16>
  {
  private:
    const std::string		&GetType(void) const;
    unsigned int		value;
    size_t			last_tick;

  public:
    static bool			TypeMatches(const std::string	&type);
    static hbs::IComponent		*Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position);
    virtual std::string		GetDisplayType(void) const;

  public:
    virtual bool			DrivesPin(size_t			n) const { return ((n >= 1 && n <= 7) || n == 9 || (n >= 12 && n <= 15)); }

    hbs::Tristate		Compute(size_t				n);

    CMP_4040(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&pos);
    virtual ~CMP_4040(void);
  };
}

#endif	//			__HBS_4040_HPP__
