// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__HBS_TERMINAL_HPP__
# define			__HBS_TERMINAL_HPP__
# include			<bitset>
# include			"Output.hpp"

namespace			hbs
{
  class				Terminal : public hbs::AComponent<9>, public hbs::Output
  {
  private:
    const std::string		&GetType(void) const;
    size_t			last_tick;
    char			c;

  public:
    void			SetLink(size_t				pnthis,
					hbs::IComponent			&component,
					size_t				pntarg,
					const std::string		&pos);
    void			Draw(hbs::Screen			&screen) const;
    void			Move(const hbs::Position		&pos)
    {
      hbs::AComponent<9>::Move(pos);
    }
    const std::string		&GetName(void) const final;
    size_t			GetPinCount(void) const final;
    hbs::Position		GetPosition(void) const final;
    hbs::Position		GetPinPosition(size_t			pin) const;
    bool			IsUnder(const hbs::Screen		&screen,
					const t_bunny_position		&pos) const;

    hbs::Tristate		Compute(size_t				n);
    void			Dump(void) const;
    bool			Displayable(void) const;

    Terminal(const hbs::Timer	&timer,
	     const std::string	&name,
	     const std::string	&pos);
    virtual ~Terminal(void);
  };
}

#endif	//			__HBS_TERMINAL_HPP__
