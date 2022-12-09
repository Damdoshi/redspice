// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__ICOMPONENT_HPP__
# define			__ICOMPONENT_HPP__
# include			<iosfwd>
# include			<sys/types.h>

namespace			hbs
{
  class				Screen;
  
  enum				Tristate
    {
      BROKEN			= -2,
      UNDEFINED			= -1,
      FALSE			= +0,
      TRUE			= +1
    };

  class				IComponent
  {
  public:
    /// Compute value of the precised pin
    virtual hbs::Tristate	Compute(size_t			pin_num_this = 1) = 0;

    /// Useful to link IComponent together
    virtual void		SetLink(size_t			pin_num_this,
					hbs::IComponent		&component,
					size_t			pin_num_target,
					const std::string	&pos = "") = 0;

    virtual void		Draw(hbs::Screen		&screen) const = 0;

    ///// Print on terminal the name of the component and
    //// the state of every pin of the current component
    /// The output won't be tested, but it may help you
    // as a trace.
    virtual void		Dump(void) const = 0;

    virtual ~IComponent(void) {}
  };
}

std::ostream			&operator<<(std::ostream	&os,
					    hbs::Tristate	t);
char				&operator<<(char		&c,
					    hbs::Tristate	t);

#endif	//			__ICOMPONENT_HPP__
