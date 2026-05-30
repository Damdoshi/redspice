// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef				__TRACK_HPP__
# define			__TRACK_HPP__
# include			<limits>
# include			<map>
# include			<vector>
# include			"IComponent.hpp"
# include			"ILink.hpp"
# include			"Positions.hpp"
# include			"Timer.hpp"
# include			"Parsing.hpp"
# include			"Exception.hpp"

namespace			hbs
{
  class				Track : public virtual hbs::IComponent, public hbs::ILink
  {
  public:
    static const size_t		NoNode;

    struct			Attachment
    {
      hbs::IComponent		*component;
      size_t			pin;
      size_t			node;
    };

    struct			Segment
    {
      size_t			from;
      size_t			to;
    };

  protected:
    typedef std::map<size_t, Tristate> PinState;
    typedef PinState::iterator	Preset;

    const hbs::Timer		&timer;
    std::string		name;
    mutable std::string		type;
    hbs::Positions		path;
    std::vector<Segment>		segments;
    std::vector<Attachment>	attachments;
    bool				routing_error;
    std::map<size_t, PinState>	timeline;
    bool			computing;

    void			CleanOld(void);
    bool			AlreadyComputed(size_t n, hbs::Tristate &out);
    Preset			PresetOutput(size_t n);
    void			SetAllPins(hbs::Tristate value);
    void			SetNetPins(size_t pin_num_this, hbs::Tristate value);
    size_t			DefaultNodeForPin(size_t pin, const hbs::IComponent &component, size_t component_pin) const;
    hbs::Positions::iterator	NodeIterator(size_t node);
    hbs::Positions::const_iterator NodeIterator(size_t node) const;
    double			DistanceToSegment(const hbs::Position &p, const hbs::Position &a, const hbs::Position &b) const;

  public:
    void			SetPath(const std::string &geometry);
    const hbs::Positions	&GetPath(void) const;
    hbs::Positions		&GetPath(void);
    const std::vector<Segment> &GetSegments(void) const;
    const std::vector<Attachment> &GetAttachments(void) const;
    void			SetAttachmentNode(size_t pin, size_t node);
    size_t			GetAttachmentNode(size_t pin) const;
    size_t			AddNode(const hbs::Position &pos, hbs::ILink::Layer layer = hbs::ILink::BOTTOM);
    size_t			AddFreeNode(const hbs::Position &pos, hbs::ILink::Layer layer = hbs::ILink::BOTTOM);
    size_t			AddNodeAfter(size_t from, const hbs::Position &pos, hbs::ILink::Layer layer = hbs::ILink::BOTTOM);
    void			AddSegment(size_t from, size_t to);
    bool			DeleteBranchFromSegment(size_t segment);
    size_t			InsertNodeOnSegment(size_t segment, const hbs::Position &pos);
    void			MoveNode(size_t node, const hbs::Position &delta);
    hbs::Position		GetNodePosition(size_t node) const;
    hbs::ILink::Layer	GetNodeLayer(size_t node) const;
    void			SetNodeLayer(size_t node, hbs::ILink::Layer layer);
    void			ToggleNodeLayer(size_t node);
    size_t			FindNode(const hbs::Screen &screen, t_bunny_position pos) const;
    size_t			FindSegment(const hbs::Screen &screen, t_bunny_position pos) const;
    void			SetRoutingError(bool error);
    bool			HasRoutingError(void) const;
    hbs::Packet		GetLinkStep(const hbs::Screen &screen, t_bunny_position pos);
    hbs::Packet		EndLinkStep(void);

    const std::string		&GetType(void) const;
    const std::string		&GetName(void) const;
    void			SetName(const std::string &name);
    void			DisconnectFrom(const hbs::IComponent *component);
    void			RemoveAttachment(size_t pin);
    size_t			GetPinCount(void) const;
    hbs::Tristate		Compute(size_t pin_num_this = 1);
    void			SetLink(size_t pin_num_this, hbs::IComponent &component, size_t pin_num_target, const std::string &pos = "");
    bool			IsUnder(const hbs::Screen &screen, const t_bunny_position &pos) const;
    void			Move(const hbs::Position &pos);
    hbs::Position		GetPosition(void) const;
    hbs::Position		GetPinPosition(size_t pin) const;
    void			Draw(hbs::Screen &screen) const;
    void			Dump(void) const;

    const IComponent		*GetTarget(void) const;
    size_t			GetTargetPin(void) const;
    size_t			GetNbrSteps(void) const;
    std::pair<int, int>		GetStep(size_t i) const;

    Track(const hbs::Timer &timer, const std::string &name, const std::string &geometry = "");
    virtual ~Track(void);
  };
}

#endif	//			__TRACK_HPP__
