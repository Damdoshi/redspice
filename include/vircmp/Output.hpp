// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_OUTPUT_HPP__
# define			__NTS_OUTPUT_HPP__
# include			"AComponent.hpp"

namespace			nts
{
  class				Output : public virtual nts::AComponent<1>
  {
  private:
    virtual const std::string	&GetType(void) const;

  public:
    virtual nts::Tristate	Compute(size_t			pin = 1);
    virtual bool		Displayable(void) const;

    Output(const nts::Timer	&timer);
    ~Output(void);
  };
}

#endif	//			__NTS_OUTPUT_HPP__
