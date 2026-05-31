// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__HBS_4013_HPP__
# define			__HBS_4013_HPP__
# include			<bitset>
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_4013 : public hbs::AComponent<14>
  {
  private:
    const std::string		&GetType(void) const;
    virtual bool			DrivesPin(size_t			n) const { return (n == 1 || n == 2 || n == 12 || n == 13); }

    hbs::Tristate		val[2];
    size_t			last_tick[2];

  public:
    static bool			TypeMatches(const std::string	&type);
    static hbs::IComponent		*Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position);
    virtual std::string		GetDisplayType(void) const;

  public:
    hbs::Tristate		Compute(size_t				n);

    CMP_4013(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&pos);
    virtual ~CMP_4013(void);
  };
}

#endif	//			__HBS_4013_HPP__
