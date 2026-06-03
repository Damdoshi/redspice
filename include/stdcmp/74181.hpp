// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef				__HBS_74181_HPP__
# define			__HBS_74181_HPP__
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_74181 : public hbs::AComponent<24>
  {
  private:
    const std::string		&GetType(void) const;
    unsigned char		ReadA(void);
    unsigned char		ReadB(void);
    unsigned char		ReadS(void);
    unsigned char		Function(void);

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

    CMP_74181(const hbs::Timer	&timer,
	      const std::string	&name,
	      const std::string	&pos);
    virtual ~CMP_74181(void);
  };
}

#endif	//			__HBS_74181_HPP__
