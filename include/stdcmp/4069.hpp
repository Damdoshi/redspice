// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__HBS_4069_HPP__
# define			__HBS_4069_HPP__
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_4069 : public hbs::AComponent<14>
  {
  private:
    const std::string		&GetType(void) const;

  public:
    virtual bool			DrivesPin(size_t			n) const { return (n == 2 || n == 4 || n == 6 || n == 8 || n == 10 || n == 12); }

    hbs::Tristate		Compute(size_t			n);

    CMP_4069(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&pos);
    virtual ~CMP_4069(void);
  };
}

#endif	//			__HBS_4069_HPP__
