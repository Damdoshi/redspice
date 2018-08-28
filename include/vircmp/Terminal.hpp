// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_TERMINAL_HPP__
# define			__NTS_TERMINAL_HPP__
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
					size_t				pntarg);

    hbs::Tristate		Compute(size_t				n);
    void			Dump(void) const;
    bool			Displayable(void) const;

    Terminal(const hbs::Timer	&timer);
    virtual ~Terminal(void);
  };
}

#endif	//			__NTS_TERMINAL_HPP__
