// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef					__NTS_CIRCUIT_HPP__
# define				__NTS_CIRCUIT_HPP__
# include				"Timer.hpp"
# include				"Input.hpp"
# include				"Output.hpp"

namespace				nts
{
  class					Circuit : public nts::IComponent
  {
  protected:
    nts::Timer				&timer;

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
						const std::string		&value);

  public:
    nts::Tristate			Compute(size_t				output);
    nts::Tristate			Compute(void);

    void				SetLink(size_t				pnthis,
						nts::IComponent			&component,
						size_t				pntarg);
    void				Dump(void) const;

    const std::string			&GetOutputName(size_t			n) const;
    bool				GetDisplayable(size_t			n) const;
    size_t				GetOutputNum(void) const;

    bool				Load(const std::string			&file);
    void				SetValue(const std::string		&input,
						 nts::Tristate			value);

    Circuit(nts::Timer	&timer);
    virtual ~Circuit(void);
  };
}

#endif	//				__NTS_CIRCUIT_HPP__
