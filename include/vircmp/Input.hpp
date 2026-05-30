// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__HBS_INPUT_HPP__
# define			__HBS_INPUT_HPP__
# include			"AComponent.hpp"

namespace			hbs
{
  class				Input : public hbs::AComponent<1>
  {
  private:
    virtual const std::string	&GetType(void) const;

  protected:
    hbs::Tristate		value;

  public:
    hbs::Tristate		GetPin(size_t n)
    {
      (void)n;
      return (value);
    }

    virtual hbs::Tristate	Compute(size_t		pin_num_this = 1);
    virtual bool			DrivesPin(size_t		pin) const { return (pin == 1); }
    void			SetValue(hbs::Tristate	tristate);
    std::string			GetDefinitionValue(void) const;

    Input(hbs::Timer		&timer,
	  const std::string	&name,
	  const std::string	&value = "",
	  const std::string	&pos = "");
    virtual ~Input(void);
  };
}

#endif	//			__HBS_INPUT_HPP__
