// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#ifndef					__SCREEN_HPP__
# define				__SCREEN_HPP__
# define				PINSIZE_DEFAULT		20
# include				<lapin.h>
# include				<map>
# include				<set>
# include				<string>
# include				<vector>
# include				"Positions.hpp"
# include				"Timer.hpp"

namespace				hbs
{
  class					IComponent;
  class					Circuit;
  class					Track;
  class					Screen
  {
  public:
    mutable t_bunny_window		*win;
    mutable t_bunny_picture		*pic;
    t_bunny_picture			*author;
    t_bunny_picture			*controls;
    hbs::Position			camera;
    int					pin_size;
    bool				panning;
    bool				selecting;
    bool				dragging_selection;
    bool				context_menu;
    bool				rename_mode;
    bool				drawing_mode;
    bool				placing_component;
    hbs::IComponent			*component_to_place;
    std::string				placing_type;
    t_bunny_position			context_pos;
    std::string			rename_buffer;
    bool				search_panel;
    int				search_offset;
    std::string			search_query;
    t_bunny_position			pan_last;
    t_bunny_position			pan_origin;
    bool				right_panning;
    bool				right_panning_moved;
    bool				loopsim;
    size_t				loop_ticks_per_frame;
    IComponent				*grabbed;
    hbs::Packet				grabbed_step;
    hbs::Track				*active_track;
    size_t				active_node;
    std::set<hbs::IComponent*>	selected_components;
    std::set<hbs::Track*>		selected_tracks;
    t_bunny_position			grab_pos;
    std::string				file_name;
    
  public:

    static constexpr unsigned int	White = WHITE;
    static constexpr unsigned int	Black = BLACK;
    static constexpr unsigned int	Red = RED;
    static constexpr unsigned int	Blue = BLUE;
    static constexpr unsigned int	Yellow = YELLOW;
    static constexpr unsigned int	Teal = TEAL;
    static constexpr unsigned int	Green = GREEN;
    static constexpr unsigned int	Purple = PURPLE;
    static constexpr unsigned int	Pink = PINK2;

    static constexpr unsigned int	TristateColor[5] =
      {
	YELLOW,
	0,
	((unsigned int)GRAY(64)),
	RED,
	BLUE
      };
    
    int					PinSize(int			p = 0);
    hbs::Position			ScreenToWorld(t_bunny_position	mouse) const;
    void				ZoomAt(t_bunny_position		mouse,
				       int			delta);
    void				PanByScreenDelta(int			dx,
						 int			dy);
    bool				HasSelection(void) const;
    bool				GetSelectionBounds(hbs::Position	&from,
						   hbs::Position	&to) const;
    bool				IsInsideSelectionBox(const hbs::Position &pos) const;
    void				DrawSelectionBox(void);
    void				ResetZoom(t_bunny_position mouse);
    hbs::IComponent			*GetUniqueSelection(void) const;

    void				Resize(const t_bunny_window	&win,
					       t_bunny_position		pos);
    bool				Draw(hbs::Circuit		&c);

    void				Circle(Position			pos,
					       Size			siz,
					       unsigned int		col,
					       bool			full = false);
    void				Line(Position			a,
					     Position			b,
					     unsigned int		col);
    void				Square(Position			pos,
					       Size			siz,
					       unsigned int		col,
					       bool			full = false);
    void				Text(Position			p,
					     Size			s,
					     unsigned int		col,
					     const std::string		&str);
    Size				TextSize(Size			s,
						 const std::string	&str);

    bool				Loop(hbs::Circuit		&circ,
					     hbs::Timer			&timer);

    Screen(std::string const &filename);
    ~Screen(void);
  };

  typedef hbs::IComponent *(*CreateFunction)(void);
  struct				ComponentLib
  {
    void				*handler;
    hbs::CreateFunction			create;
  };
}

enum					FileBrowserMode
{
  FILE_BROWSER_OPEN = 0,
  FILE_BROWSER_SAVE_ACTIVE,
  FILE_BROWSER_SAVE_COPY,
  FILE_BROWSER_SAVE_THEN_CLOSE,
  FILE_BROWSER_SAVE_THEN_QUIT
};

struct					LoopDocument
{
  std::string				file_name;
  hbs::Timer				*timer;
  hbs::Circuit				*circuit;
  hbs::Position				camera;
  int					pin_size;
  bool					owned;
  bool					dirty;
  bool					temporary;

  LoopDocument(hbs::Circuit &c, hbs::Timer &t, const std::string &file);
  LoopDocument(const std::string &file, bool create_if_missing = false, bool temporary = false);
  ~LoopDocument(void);
};

struct					FileBrowserEntry
{
  std::string				name;
  std::string				path;
  bool					directory;
};

struct					LoopData
{
  hbs::Screen				&screen;
  std::vector<LoopDocument*>		documents;
  size_t				active_document;
  bool					file_browser;
  FileBrowserMode			file_browser_mode;
  std::string				browser_directory;
  std::vector<FileBrowserEntry>	browser_entries;
  int					browser_offset;
  int					opened_offset;
  std::string				browser_error;
  std::string				browser_target;
  bool					quit_requested;
  t_bunny_position			last_mouse = {0, 0};
  std::map<
    std::string,
    hbs::ComponentLib
    >					library;

  LoopData(hbs::Circuit &c, hbs::Timer &t, hbs::Screen &s);
  ~LoopData(void);
  bool					HasDocument(void) const;
  hbs::Circuit				&CurrentCircuit(void);
  hbs::Timer				&CurrentTimer(void);
  const hbs::Circuit			&CurrentCircuit(void) const;
  const hbs::Timer			&CurrentTimer(void) const;
  void					SaveActiveView(void);
  void					ApplyActiveView(void);
  bool					SelectDocument(size_t index);
  bool					OpenFile(const std::string &file);
  bool					NewDocument(void);
  bool					CloseCurrentDocument(bool discard = false);
  bool					SaveCurrentDocument(void);
  bool					SaveCurrentDocumentAs(const std::string &file,
						      bool copy_only = false);
  void					BeginOpenFileBrowser(void);
  void					BeginFileMenu(void);
  void					BeginSaveAs(bool copy_only = false);
  void					BeginSavePrompt(FileBrowserMode mode);
  bool					CompleteSavePrompt(bool discard = false);
  bool					RequestQuit(void);
  void					MarkDirty(void);
  void					RefreshBrowser(void);
};

namespace					hbs
{
  void					DrawFileBrowser(LoopData &ld);
  bool					FileBrowserKey(t_bunny_keysym sym, LoopData &ld);
  bool					FileBrowserClick(t_bunny_event_state state,
							 t_bunny_mouse_button sym,
							 LoopData &ld);
  bool					FileBrowserWheel(int delta, LoopData &ld);
}

#endif	//			__SCREEN_HPP__
