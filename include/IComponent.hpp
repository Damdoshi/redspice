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
# include                       <vector>
# include			<sys/types.h>
# include			"Screen.hpp"

namespace			hbs
{
  enum				Tristate
    {
      BROKEN			= -2,
      UNDEFINED			= -1,
      FALSE			= +0,
      TRUE			= +1,
      HIGH_IMPEDANCE		= +2
    };

  class IComponent;
  struct SignalConnection
  {
    IComponent *component;
    size_t pin;
    bool weak = false;
    unsigned levels = 3; // bit 0: low can pass; bit 1: high can pass
  };

  class				IComponent
  {
    int rotation = 0;
  public:
    int GetRotation(void) const { return rotation; }
    virtual void Rotate(int quarter_turns)
    { rotation = (rotation + quarter_turns % 4 + 4) % 4; }
    Position LocalToWorld(Position p) const
    { return GetPosition() + QuarterTurn(p, rotation); }
    Position WorldToLocal(Position p) const
    { return QuarterTurn(p - GetPosition(), -rotation); }
    Size RotatedSize(Size size) const
    { return rotation % 2 ? Size{size.y, size.x} : size; }
    void DrawLabel(Screen &screen, Position local_center, Size font,
                   unsigned color, const std::string &text) const;

    /// Return the canonical type name of the component.
    virtual const std::string	&GetType(void) const = 0;

    /// Return the instance name of the component.
    virtual const std::string	&GetName(void) const = 0;

    /// Rename the component instance.
    virtual void		SetName(const std::string &name) = 0;

    /// Remove every direct connection to this component.
    virtual void		DisconnectFrom(const hbs::IComponent *component) = 0;

    /// Return the number of pins exposed by the component.
    virtual size_t		GetPinCount(void) const = 0;

    /// Optional value stored in the textual declaration.
    virtual std::string		GetDefinitionValue(void) const { return (""); }

    /// Return true when this pin actively drives the attached net.
    /// Passive input pins and observation pins must return false, otherwise
    /// querying a net would recursively poison the cache of sink pins.
    virtual bool		DrivesPin(size_t pin) const { (void)pin; return (false); }

    /// Advance autonomous clocked devices before evaluating observable outputs.
    /// Default gates remain lazy. Implementations must be idempotent per tick.
    virtual void                BeginTick(void) {}
    /// Commit bus writes after clocked drivers and combinational nets settle.
    virtual void                EndTick(void) {}

    /// Physical neighbours of a terminal. Used to resolve passive networks
    /// without turning a switch/resistor into two recursively coupled drivers.
    virtual void GetConnections(size_t, std::vector<SignalConnection> &) const {}
    virtual bool IsPassivePin(size_t) const { return false; }
    virtual Tristate GetPassiveConnections(size_t, std::vector<SignalConnection> &)
    { return HIGH_IMPEDANCE; }

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

    /// Long, human-readable component type. Useful for documentation,
    /// catalogues and error messages. The short type remains the one
    /// drawn on the component and used in .chipsets.
    virtual std::string		GetDisplayType(void) const;

    virtual ~IComponent(void) {}
  };
}

std::ostream			&operator<<(std::ostream	&os,
					    hbs::Tristate	t);
char				&operator<<(char		&c,
					    hbs::Tristate	t);

#endif	//			__ICOMPONENT_HPP__
