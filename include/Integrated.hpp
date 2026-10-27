#ifndef BCAD_INTEGRATED_HPP
#define BCAD_INTEGRATED_HPP
#include "Screen.hpp"
#include "Editor.hpp"
namespace hbs {
bool IsIntegratedCircuitFile(const std::string &file);
void DrawIntegratedCircuit(LoopData &ld);
bool IntegratedKey(t_bunny_event_state state,t_bunny_keysym key,LoopData &ld);
bool IntegratedClick(t_bunny_event_state state,t_bunny_mouse_button button,LoopData &ld);
bool IntegratedMove(LoopData &ld);
bool IntegratedWheel(int wheel,int delta,LoopData &ld);
const char *IntegratedShortcutLabel(int index,bool control);
bool IntegratedShortcut(int index,bool control,LoopData &ld);
}
#endif
