// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#ifndef					__SHORTCUTS_HPP__
# define				__SHORTCUTS_HPP__
# include				<lapin.h>

struct					LoopData;

namespace				hbs
{
  class					Screen;

  enum					ShortcutMode
    {
      SHORTCUT_NORMAL = 0,
      SHORTCUT_CONTROL,
      SHORTCUT_SHIFT,
      SHORTCUT_CONTROL_SHIFT,
      SHORTCUT_MODE_COUNT
    };

  typedef void				(*ShortcutAction)(LoopData &ld);

  struct				ShortcutBinding
  {
    const char				*label;
    ShortcutAction			action;
  };

  static constexpr int			ShortcutFunctionKeyCount = 12;
  static constexpr int			ShortcutBarCellCount = ShortcutFunctionKeyCount + 1;
  static constexpr int			ShortcutBarHeight = 48;

  ShortcutMode				CurrentShortcutMode(void);
  unsigned int				ShortcutModeColor(ShortcutMode mode);
  int					FunctionKeyIndex(t_bunny_keysym sym);
  const ShortcutBinding			&GetShortcutBinding(ShortcutMode mode,
							    int key_index);
  bool					TriggerShortcut(t_bunny_keysym sym,
							LoopData &ld);
  bool					TriggerEscapeShortcut(LoopData &ld);
  bool					HandleShortcutBarClick(t_bunny_event_state state,
							       t_bunny_mouse_button sym,
							       t_bunny_position pos,
							       LoopData &ld);
  void					DrawShortcutBar(Screen &screen,
							const LoopData *ld = NULL);
}

#endif	//				__SHORTCUTS_HPP__
