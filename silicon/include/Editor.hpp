#ifndef REDSILICON_EDITOR_HPP
#define REDSILICON_EDITOR_HPP
#include "Silicon.hpp"
#include <memory>

namespace silicon {
using Color=uint32_t; // RRGGBBAA, independent of the window library
struct Image {
  int width,height;std::vector<Color> pixels;
  Image(int w=1440,int h=900);
  void Pixel(int x,int y,Color c);
  void Line(int x,int y,int x2,int y2,Color color);
  void Box(int x,int y,int w,int h,Color color,bool fill=true);
  void Text(int x,int y,const std::string &text,Color color,int scale=1);
  std::string Ppm() const;
};
enum class Tool { Select, Rectangle, Input, Output, Supply, Ground, Probe, Note, Resistor, Capacitor, Current, Terminal, InOut };
class Editor {
  std::vector<std::string> undo,redo;
  std::string saved;
  Document drag_original;
  Point down,mouse,last_pixel;
  bool pressed=false,dragging=false,panning=false,boxing=false,shift_select=false;
  int highlighted=-1;
  uint64_t observed_generation=0;
  void Changed(const Document &before);
  void Refresh();
  void FinishPrompt();
  void Ask(const std::string &action,const std::string &initial="");
  bool Button(int x,int y);
  int Hit(Point p) const;
public:
  Document document;
  Netlist nets;
  std::shared_ptr<Simulator> simulator;
  std::shared_ptr<SimulationSession> session;
  std::shared_ptr<CompiledDesign> design;
  std::string path,message="Dessiner les regions, poser les contacts, extraire.";
  std::string prompt,prompt_text;
  std::set<int> selected;
  Layer layer=Layer::Active;
  Tool tool=Tool::Select;
  bool visible[10]={true,true,true,true,true,true,true,true,true,true};
  bool running=false,quit=false;
  bool embedded=false,read_only=false;
  std::string instance_path,requested;
  double zoom=14,center_x=10,center_y=3;
  int width=1440,height=900;
  explicit Editor(const Document &doc=Document(),const std::string &file="");
  bool Dirty() const;
  Point World(int x,int y) const;
  Point Screen(Point p) const;
  void Fit();
  void MouseDown(int x,int y,int button,bool shift=false);
  void MouseUp(int x,int y,int button);
  void MouseMove(int x,int y);
  void Wheel(int x,int y,int delta);
  void Key(const std::string &key,bool ctrl=false,bool shift=false);
  void Type(uint32_t character);
  void Step();
  void Save(const std::string &file);
  void Load(const std::string &file);
  void Render(Image &image) const;
  void Rebuild();
  void Bind(const std::shared_ptr<SimulationSession> &session,const std::string &instance);
  void SyncObservation();
  void InsertInstance(const std::string &source,Point position);
  const Item *SelectedInstance() const;
  void MarkSaved(const std::string &file);
};
int RunWindow(Editor &editor);
int CommandLine(int argc,char **argv);
}
#endif
