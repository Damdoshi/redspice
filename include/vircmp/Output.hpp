// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_OUTPUT_HPP__
# define			__NTS_OUTPUT_HPP__
# include			"AComponent.hpp"

namespace			hbs
{
  class				Output : public virtual hbs::AComponent<1>
  {
  private:
    virtual const std::string	&GetType(void) const;

  public:
    virtual hbs::Tristate	Compute(size_t			pin = 1);
    virtual bool		Displayable(void) const;

    Output(const hbs::Timer	&timer);
    ~Output(void);
  };
}

#endif	//			__NTS_OUTPUT_HPP__
