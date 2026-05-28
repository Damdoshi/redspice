// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Pentacle Technologie 2008-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#ifndef				__ICOMPONENT_HPP__
# define			__ICOMPONENT_HPP__
# include			<iosfwd>
# include			<string>
# include			<sys/types.h>
# include			"Screen.hpp"

namespace			hbs
{
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
    /// Return the canonical type name of the component.
    virtual const std::string	&GetType(void) const = 0;

    /// Return the instance name of the component.
    virtual const std::string	&GetName(void) const = 0;

    /// Return the number of pins exposed by the component.
    virtual size_t		GetPinCount(void) const = 0;

    /// Optional value stored in the textual declaration.
    virtual std::string		GetDefinitionValue(void) const { return (""); }

    /// Compute value of the precised pin
    virtual hbs::Tristate	Compute(size_t			pin_num_this = 1) = 0;

    /// Useful to link IComponent together
    virtual void		SetLink(size_t			pin_num_this,
					hbs::IComponent		&component,
					size_t			pin_num_target,
					const std::string	&pos = "") = 0;

    virtual bool		IsUnder(const hbs::Screen	&screen,
					const t_bunny_position	&pos) const = 0;
    virtual void		Move(const Position		&pos) = 0;
    virtual Position		GetPosition(void) const = 0;
    virtual Position		GetPinPosition(size_t		pin) const = 0;
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
