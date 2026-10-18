// RED Spice - vector symbols, shared by the editor and visual regression sheet.
#include "Prototype.hpp"
#include <algorithm>
#include <cmath>

using hbs::Position;
using hbs::SymbolCanvas;

unsigned hbs::PrototypeColor(Tristate value)
{
  switch(value)
    {
    case FALSE: return Screen::Red;
    case TRUE: return Screen::Green;
    case BROKEN: return Screen::Yellow;
    case HIGH_IMPEDANCE: return Screen::Purple;
    default: return GRAY(128);
    }
}

hbs::ScreenSymbolCanvas::ScreenSymbolCanvas(Screen &screen, Position origin)
  : screen(screen), origin(origin) {}
void hbs::ScreenSymbolCanvas::Line(Position a, Position b, unsigned color)
{ screen.Line(origin+a,origin+b,color); }
void hbs::ScreenSymbolCanvas::Circle(Position center, Size radius, unsigned color, bool fill)
{ screen.Circle(origin+center,radius,color,fill); }
void hbs::ScreenSymbolCanvas::Text(Position center, double height, unsigned color,
                                 const std::string &text)
{
  double pixels=height*std::max(1,screen.pin_size);
  Size font{pixels*0.6,pixels};
  double width=screen.TextSize(font,text).x/std::max(1,screen.pin_size);
  screen.Text(origin+Position{center.x-width/2,center.y},font,color,text);
}

namespace
{
  const unsigned white=hbs::Screen::White;
  void rectangle(SymbolCanvas &c, double x, double y, double w, double h, unsigned color=white)
  {
    c.Line({x,y},{x+w,y},color); c.Line({x+w,y},{x+w,y+h},color);
    c.Line({x+w,y+h},{x,y+h},color); c.Line({x,y+h},{x,y},color);
  }
  void polyline(SymbolCanvas &c, const Position *points, size_t count, unsigned color=white)
  { for(size_t i=1;i<count;++i)c.Line(points[i-1],points[i],color); }
  void arrow(SymbolCanvas &c, Position start, Position end, unsigned color=white)
  {
    c.Line(start,end,color);
    double dx=end.x-start.x, dy=end.y-start.y;
    double length=std::hypot(dx,dy);
    if(length==0)return;
    dx/=length; dy/=length;
    c.Line(end,{end.x-.5*dx+.24*dy,end.y-.5*dy-.24*dx},color);
    c.Line(end,{end.x-.5*dx-.24*dy,end.y-.5*dy+.24*dx},color);
  }
  void one(SymbolCanvas &c, double x, double y)
  {
    c.Line({x-.15,y-.28},{x+.08,y-.5},white);
    c.Line({x+.08,y-.5},{x+.08,y+.5},white);
    c.Line({x-.2,y+.5},{x+.32,y+.5},white);
  }
  void and_mark(SymbolCanvas &c)
  {
    const Position points[]={
      {.68,.6},{-.35,-.6},{-.5,-.92},{-.3,-1.12},{.08,-1.08},
      {.25,-.8},{.1,-.5},{-.55,-.07},{-.73,.25},{-.6,.6},
      {-.27,.72},{.12,.57},{.48,.15},{.67,-.3}
    };
    polyline(c,points,sizeof(points)/sizeof(points[0]));
  }
  void diode(SymbolCanvas &c)
  {
    const Position triangle[]={{-.8,-.7},{-.8,.7},{.65,0},{-.8,-.7}};
    polyline(c,triangle,4);
    c.Line({.65,-.8},{.65,.8},white);
    c.Line({-3,0},{-.8,0},white); c.Line({.65,0},{3,0},white);
  }
}

void hbs::DrawPrototypeSymbol(SymbolCanvas &c, PrototypeKind kind,
                              const std::string &name, const std::string &value,
                              bool closed, const Tristate states[4], bool lit)
{
  double label_x=0, label_y=-2.9, value_y=2.45;
  if(kind<=PT_XNOR)
    {
      bool unary=kind==PT_BUF||kind==PT_NOT;
      bool inverse=kind==PT_NAND||kind==PT_NOR||kind==PT_NOT||kind==PT_XNOR;
      if(unary)
        {
          const Position triangle[]={{-2,-1.8},{-2,1.8},{1.8,0},{-2,-1.8}};
          polyline(c,triangle,4);
          c.Line({-3,0},{-2,0},white);
        }
      else
        {
          rectangle(c,-2,-2,4,4);
          c.Line({-3,-1},{-2,-1},white); c.Line({-3,1},{-2,1},white);
          if(kind==PT_AND||kind==PT_NAND) and_mark(c);
          else
            {
              if(kind==PT_OR||kind==PT_NOR)
                {
                  c.Line({-.95,-.55},{-.3,-.15},white);
                  c.Line({-.3,-.15},{-.95,.25},white);
                  c.Line({-.95,.55},{-.3,.55},white);
                }
              else
                {
                  c.Line({-.95,-.2},{-.3,-.2},white);
                  c.Line({-.95,.2},{-.3,.2},white);
                }
              one(c,.5,0);
            }
        }
      double edge=unary?1.8:2;
      if(inverse)
        {
          c.Circle({edge+.23,0},{.23,.23},white,false);
          edge+=.46;
        }
      c.Line({edge,0},{3,0},white);
    }
  else if(kind==PT_PROBE)
    {
      unsigned color=PrototypeColor(states[0]);
      c.Line({-2,0},{-.85,0},color);
      c.Circle({0,0},{.85,.85},color,true);
      c.Circle({0,0},{.85,.85},white,false);
      char state; state<<states[0];
      c.Text({0,-.36},.72,Screen::Black,std::string(1,state));
      label_y=-1.65;
    }
  else if(kind==PT_RESISTOR)
    {
      c.Line({-3,0},{-1.2,0},white); c.Line({1.2,0},{3,0},white);
      rectangle(c,-1.2,-.45,2.4,.9);
      label_y=-1.7; value_y=.8;
    }
  else if(kind==PT_DIODE||kind==PT_LED)
    {
      if(kind==PT_LED)
        {
          c.Circle({0,0},{1.05,1.05},lit?Screen::Red:GRAY(45),true);
          c.Circle({0,0},{1.05,1.05},lit?Screen::Red:GRAY(110),false);
        }
      diode(c);
      c.Text({-2.25,-.65},.35,white,"A"); c.Text({2.25,-.65},.35,white,"K");
      if(kind==PT_LED)
        {
          arrow(c,{.1,-1.2},{.85,-1.95},lit?Screen::Red:white);
          arrow(c,{.8,-.95},{1.55,-1.7},lit?Screen::Red:white);
          label_y=-2.7;
        }
      else label_y=-1.7;
    }
  else if(kind==PT_CAPACITOR)
    {
      c.Line({-3,0},{-.35,0},white); c.Line({.6,0},{3,0},white);
      c.Line({-.35,-.85},{-.35,.85},white);
      Position previous{.6,-.85};
      for(unsigned i=1;i<=12;++i)
        {
          double y=-.85+1.7*i/12;
          Position next{.4+.2*y*y/(.85*.85),y};
          c.Line(previous,next,white); previous=next;
        }
      c.Line({-1.35,-.75},{-.8,-.75},white);
      c.Line({-1.075,-1.025},{-1.075,-.475},white);
      label_y=-1.7; value_y=1.15;
    }
  else if(kind==PT_SWITCH)
    {
      c.Line({-3,0},{-1,0},white); c.Line({1,0},{3,0},white);
      c.Circle({-1,0},{.13,.13},white,false); c.Circle({1,0},{.13,.13},white,false);
      c.Line({-1,0},closed?Position{1,0}:Position{.65,-1},white);
      label_y=-1.8;
      c.Text({0,.8},.4,closed?Screen::Green:GRAY(160),closed?"ferme":"ouvert");
    }
  else if(kind==PT_NPN||kind==PT_PNP)
    {
      c.Circle({.3,0},{1.75,1.75},GRAY(135),false);
      c.Line({-3,0},{-.65,0},white); c.Line({-.65,-1},{-.65,1},white);
      c.Line({-.65,-.6},{1.5,-1.6},white); c.Line({1.5,-1.6},{3,-2},white);
      c.Line({-.65,.6},{1.5,1.6},white); c.Line({1.5,1.6},{3,2},white);
      if(kind==PT_NPN) arrow(c,{.05,.93},{1.12,1.43});
      else arrow(c,{1.12,1.43},{.05,.93});
      c.Text({-2.1,-.65},.35,white,"B");
      c.Text({2.35,-2.65},.35,white,"C"); c.Text({2.35,2.35},.35,white,"E");
      label_y=-3;
    }
  else if(kind==PT_PUSHBUTTON||kind==PT_PUSHBUTTON_COMPACT)
    {
      double h=kind==PT_PUSHBUTTON?3:1;
      rectangle(c,.35,-.25,2.3,h+.5,GRAY(150));
      c.Line({0,0},{.65,0},white); c.Line({0,h},{.65,h},white);
      c.Line({3,0},{2.35,0},white); c.Line({3,h},{2.35,h},white);
      c.Line({.65,0},{.65,h},white); c.Line({2.35,0},{2.35,h},white);
      if(kind==PT_PUSHBUTTON)
        {
          c.Circle({1.5,h/2},{.65,.65},closed?Screen::Green:GRAY(55),true);
          c.Circle({1.5,h/2},{.65,.65},white,false);
        }
      c.Line({.65,h/2},closed?Position{2.35,h/2}:Position{2.05,h/2-.5},white);
      label_x=1.5; label_y=-1.3; value_y=h+.6;
      c.Text({1.5,value_y},.35,closed?Screen::Green:GRAY(160),closed?"appuye":"relache");
    }
  for(size_t pin=1;pin<=PrototypePinCount(kind);++pin)
    {
      Position point=PrototypePinPosition(kind,pin);
      // Cover the track's .35-radius pad with the signal colour, leaving one
      // thin outline at the existing visible diameter of a connected pin.
      c.Circle(point,{.35,.35},PrototypeColor(states[pin-1]),true);
      c.Circle(point,{.35,.35},white,false);
    }
  if(!name.empty()) c.Text({label_x,label_y},.48,white,name);
  if(!value.empty()) c.Text({label_x,value_y},.4,GRAY(180),value);
}

bool hbs::DrawPrototypePreview(Screen &screen, const std::string &type,
                               int x, int y, int width, int height)
{
  auto kind=PrototypeType(type);
  if(kind==PT_INVALID)return false;
  int previous_size=screen.pin_size;
  Position previous_camera=screen.camera;
  screen.pin_size=std::max(1,std::min((width-20)/8,(height-30)/8));
  screen.camera={0,0};
  double px=(x+width/2.0-screen.pic->buffer.width/2.0)/screen.pin_size;
  double py=(y+height/2.0-screen.pic->buffer.height/2.0)/screen.pin_size;
  if(kind==PT_PUSHBUTTON || kind==PT_PUSHBUTTON_COMPACT)
    { px-=1.5; py-=kind==PT_PUSHBUTTON?1.5:.5; }
  ScreenSymbolCanvas canvas(screen,{px,py});
  Tristate states[4]={UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED};
  DrawPrototypeSymbol(canvas,kind,"","",false,states);
  screen.pin_size=previous_size;
  screen.camera=previous_camera;
  return true;
}
