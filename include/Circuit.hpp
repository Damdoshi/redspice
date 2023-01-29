// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef					__HBS_CIRCUIT_HPP__
# define				__HBS_CIRCUIT_HPP__
# include				"Timer.hpp"
# include				"Input.hpp"
# include				"Output.hpp"

namespace				hbs
{
  class					Circuit : public hbs::IComponent
  {
  protected:
    hbs::Timer				&timer;

    std::map<std::string, IComponent*>	circuit;
    std::map<std::string, Input*>	inputs;
    std::map<std::string, Output*>	outputs;

    bool				ReadChipsetsInside(const std::string	&code,
							   int			&i);
    bool				ReadChipsets(const std::string		&code,
						     int			&i);


    bool				ReadOneLink(const std::string		&code,
						    int				&i,
						    std::string			&cmp,
						    int				&pin);
    bool				ReadLinksInside(const std::string	&code,
							int			&i);
    bool				ReadLinks(const std::string		&code,
						  int				&i);

    IComponent				*Create(const std::string		&type,
						const std::string		&value,
						const std::string		&position);

  public:
    hbs::Tristate			Compute(size_t				output);
    hbs::Tristate			Compute(void);

    void				SetLink(size_t				pnthis,
						hbs::IComponent			&component,
						size_t				pntarg,
						const std::string		&pos);
    void				Dump(void) const;
    void				Map(void) const;

    const std::string			&GetOutputName(size_t			n) const;
    bool				GetDisplayable(size_t			n) const;
    size_t				GetOutputNum(void) const;

    bool				Load(const std::string			&file);
    void				SetValue(const std::string		&input,
						 hbs::Tristate			value);

    void				Draw(hbs::Screen			&screen) const;

    Circuit(hbs::Timer	&timer);
    virtual ~Circuit(void);
  };

  int					Command(const std::string		&cmd,
						hbs::Circuit			&circuit,
						hbs::Timer			&timer);
}

#endif	//				__HBS_CIRCUIT_HPP__
