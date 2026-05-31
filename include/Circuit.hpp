// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef					__HBS_CIRCUIT_HPP__
# define				__HBS_CIRCUIT_HPP__
# include				"Timer.hpp"
# include				"Input.hpp"
# include				"Output.hpp"
# include				"Track.hpp"
# include				<list>
# include				<set>
# include				<string>
# include				<vector>
# include				<map>

namespace				hbs
{
  struct				ComponentPin
  {
    hbs::IComponent			*component;
    size_t				pin;

    ComponentPin(void) : component(NULL), pin(0) {}
    ComponentPin(hbs::IComponent *c, size_t p) : component(c), pin(p) {}
    bool IsValid(void) const { return (component != NULL && pin != 0); }
  };

  class					Circuit : public hbs::IComponent
  {
  protected:
    enum					CircuitPinKind
    {
      CIRCUIT_INPUT_PIN,
      CIRCUIT_OUTPUT_PIN
    };

    struct				CircuitPin
    {
      std::string			name;
      CircuitPinKind			kind;
      hbs::IComponent			*component;
      size_t				component_pin;
      hbs::Position			position;
    };

    struct				CircuitConnection
    {
      hbs::IComponent			*component;
      size_t				pin;
    };

    struct				PackageLine
    {
      hbs::Position			from;
      hbs::Position			to;
    };

    hbs::Timer				&timer;

    std::string				component_type;
    std::string				component_name;
    hbs::Position				component_position;
    bool				component_instance;
    bool				package_defined;
    std::vector<CircuitPin>		external_pins;
    std::vector<PackageLine>		package_lines;
    std::map<size_t, std::list<CircuitConnection> > external_links;

    std::map<std::string, IComponent*>	circuit;
    std::map<std::string, Input*>	inputs;
    std::map<std::string, Output*>	outputs;
    std::map<std::string, Track*>	tracks;
    std::vector<std::string>		creatable_types;
    size_t				implicit_track_count;

    bool				ReadChipsetsInside(const std::string	&code,
							   int			&i);
    bool				ReadChipsets(const std::string		&code,
						     int			&i);


    bool				ReadTracksInside(const std::string	&code,
						 int			&i);
    bool				ReadTracks(const std::string		&code,
					   int			&i);
    hbs::Track			*CreateTrack(const std::string		&name,
					     const std::string		&geometry);
    hbs::Track			*CreateImplicitTrack(const std::string	&geometry);
    hbs::Track			*NormalizeTrack(hbs::Track		*track,
					       size_t			preferred_node);

    struct				LinkEnd
    {
      std::string			cmp;
      int				pin;
      size_t			node;
    };

    bool				ReadOneLink(const std::string		&code,
						    int				&i,
						    LinkEnd			&end);
    bool				ReadLinksInside(const std::string	&code,
							int			&i);
    bool				ReadLinks(const std::string		&code,
						  int				&i);

    IComponent				*Create(const std::string		&type,
						const std::string		&name,
						const std::string		&value,
						const std::string		&position);

    bool				ReadPackageInside(const std::string	&code,
						  int			&i);
    bool				ReadPackage(const std::string		&code,
						    int			&i);
    void				LoadExternalComponents(void);
    void				ConfigureAsComponent(const std::string	&type,
						     const std::string	&name,
						     const std::string	&position);
    void				BuildExternalPins(void);
    void				UpdateExternalInputs(void);

  public:
    const std::string			&GetType(void) const;
    const std::string			&GetName(void) const;
    void				SetName(const std::string &name);
    void				DisconnectFrom(const hbs::IComponent *component);
    size_t				GetPinCount(void) const;
    bool				DrivesPin(size_t pin) const;

    hbs::Tristate			Compute(size_t				output);
    hbs::Tristate			Compute(void);

    void				SetLink(size_t				pnthis,
						hbs::IComponent			&component,
						size_t				pntarg,
						const std::string		&pos);
    void				Dump(void) const;
    void				Map(void) const;

    void				Move(const hbs::Position		&pos);
    hbs::Position			GetPosition(void) const;
    hbs::Position			GetPinPosition(size_t			pin) const;
    bool				IsUnder(const hbs::Screen		&screen,
						const t_bunny_position		&pos) const;

    const std::string			&GetOutputName(size_t			n) const;
    bool				GetDisplayable(size_t			n) const;
    size_t				GetOutputNum(void) const;

    bool				Load(const std::string			&file);
    bool				Save(const std::string			&file) const;
    void				SetValue(const std::string		&input,
						 hbs::Tristate			value);

    bool				HasInput(const std::string		&name) const;
    bool				HasOutput(const std::string		&name) const;
    bool				HasComponent(const std::string		&name) const;
    bool				HasComponentType(const std::string	&type) const;
    hbs::Tristate			GetOutputValue(const std::string		&output);
    std::vector<std::string>		GetInputNames(void) const;
    std::vector<std::string>		GetOutputNames(void) const;
    std::vector<std::string>		GetComponentNames(void) const;
    std::string				GetComponentType(const std::string	&component) const;
    std::map<std::string, size_t>	GetComponentTypeCount(void) const;
    size_t				GetComponentCount(bool			include_tracks = false) const;
    size_t				GetLinkCount(void) const;

    void				Draw(hbs::Screen			&screen) const;
    size_t				GetTime(void) const;

    hbs::Input				*GetInput(const hbs::Screen		&screen,
						  t_bunny_position		pos) const;
    hbs::IComponent			*GetComponent(const hbs::Screen		&screen,
						      t_bunny_position		pos) const;
    hbs::ComponentPin			GetPinAt(const hbs::Screen		&screen,
						 t_bunny_position		pos) const;
    hbs::Track				*CreateUserTrackAt(const hbs::Position	&pos);
    hbs::Track				*MergeTracks(hbs::Track *dst, hbs::Track *src, size_t dst_node, size_t src_node);
    void				NormalizeTracks(void);
    hbs::Track				*FindTrackAttachedTo(hbs::IComponent *component, size_t pin, size_t *node = NULL) const;
    const std::vector<std::string>	&GetCreatableTypes(void) const;
    hbs::IComponent			*CreateUserComponent(const std::string	&type,
					     const hbs::Position	&pos);
    bool				DeleteSelected(std::set<hbs::IComponent*> &components,
					       std::set<hbs::Track*> &tracks);
    bool				RenameComponent(hbs::IComponent *component,
						const std::string &name);
    hbs::Track				*GetTrackAt(const hbs::Screen		&screen,
						     t_bunny_position		pos) const;
    void				GetSelectionInRect(const hbs::Position	&from,
						   const hbs::Position	&to,
						   std::set<hbs::IComponent*> &components,
						   std::set<hbs::Track*> &tracks) const;
    hbs::Packet				GetLinkStep(const hbs::Screen		&screen,
						    t_bunny_position		pos) const;
    hbs::Packet				EndLinkStep(void) const;
    void				ValidateRouting(void) const;
    Circuit(hbs::Timer	&timer);
    virtual ~Circuit(void);
  };

  int					Command(const std::string		&cmd,
						hbs::Circuit			&circuit,
						hbs::Timer			&timer);
}

#endif	//				__HBS_CIRCUIT_HPP__
