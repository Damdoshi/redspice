// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_4069_HPP__
# define			__NTS_4069_HPP__
# include			"AComponent.hpp"

namespace			hbs
{
  class				CMP_4069 : public hbs::AComponent<14>
  {
  private:
    const std::string		&GetType(void) const;

  public:
    hbs::Tristate		Compute(size_t			n);

    CMP_4069(const hbs::Timer	&timer);
    virtual ~CMP_4069(void);
  };
}

#endif	//			__NTS_4069_HPP__
