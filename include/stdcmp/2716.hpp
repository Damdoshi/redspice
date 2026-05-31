// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__HBS_2716_HPP__
# define			__HBS_2716_HPP__
# include			"Memory.hpp"

namespace			hbs
{
  class				CMP_2716 : public hbs::Memory<2048, 24>
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
    virtual bool			DrivesPin(size_t			n) const { return ((n >= 9 && n <= 11) || (n >= 13 && n <= 17)); }

    hbs::Tristate		Compute(size_t				n);

    CMP_2716(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&file,
	     const std::string	&position);
    virtual ~CMP_2716(void);
  };
}

#endif	//			__HBS_2716_HPP__
