// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_INPUT_HPP__
# define			__NTS_INPUT_HPP__
# include			"AComponent.hpp"

namespace			nts
{
  class				Input : public nts::AComponent<1>
  {
  private:
    virtual const std::string	&GetType(void) const;

  protected:
    nts::Tristate		value;

  public:
    virtual nts::Tristate	Compute(size_t		pin_num_this = 1);
    void			SetValue(nts::Tristate	tristate);

    Input(nts::Timer		&timer,
	  const std::string	&value = "");
    virtual ~Input(void);
  };
}

#endif	//			__NTS_INPUT_HPP__
