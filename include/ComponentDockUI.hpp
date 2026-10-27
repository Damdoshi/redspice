#ifndef BCAD_COMPONENT_DOCK_UI_HPP
#define BCAD_COMPONENT_DOCK_UI_HPP
#include "Screen.hpp"
namespace silicon {struct Image;}
namespace hbs {
int ComponentDockWidth(const Screen &screen);
int ComponentDockSlotAt(const Screen &screen,t_bunny_position point);
bcad::ComponentDock &CurrentComponentDock(LoopData &workspace);
const bcad::ComponentDock &CurrentComponentDock(const LoopData &workspace);
bool ComponentMenuMatches(const LoopData::MenuEntry &entry,const std::string &query);
void ToggleComponentMenu(LoopData &workspace);
void CloseComponentMenu(LoopData &workspace);
void RefreshComponentMenu(LoopData &workspace);
void ActivateComponent(LoopData &workspace,const bcad::DockEntry &entry);
bool PinComponent(LoopData &workspace,const bcad::DockEntry &entry);
bool ComponentInterfaceKey(t_bunny_event_state state,t_bunny_keysym key,LoopData &workspace);
bool ComponentInterfaceClick(t_bunny_event_state state,t_bunny_mouse_button button,LoopData &workspace);
bool ComponentInterfaceMove(LoopData &workspace);
void DrawComponentDocks(LoopData &workspace);
void RenderComponentDock(silicon::Image &image,const bcad::ComponentDock &dock,int side,int pressed=-1,int hovered=-1,bool dragging=false);
void DrawComponentMenu(LoopData &workspace);
}
#endif
