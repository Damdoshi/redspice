// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__HBS_4008_HPP__
# define			__HBS_4008_HPP__
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_4008 : public hbs::AComponent<16>
  {
  private:
    const std::string		&GetType(void) const;

  public:
    static bool			TypeMatches(const std::string	&type);
    static hbs::IComponent		*Create(hbs::Timer		&timer,
					const std::string	&type,
					const std::string	&name,
					const std::string	&value,
					const std::string	&position);
    virtual std::string		GetDisplayType(void) const;

  public:
    virtual bool			DrivesPin(size_t			n) const { return (n >= 10 && n <= 14); }

    hbs::Tristate		Compute(size_t				n);

    CMP_4008(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&position);
    virtual ~CMP_4008(void);
  };
}

#endif	//			__HBS_4008_HPP__
