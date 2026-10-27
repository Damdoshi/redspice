#include "Silicon.hpp"
#include <algorithm>
#include <stdexcept>

namespace silicon {
namespace {
void Shape(Document &d,Layer l,int x,int y,int w=1,int h=1,const std::string &group="") {
  Item i;i.layer=l;i.box={x,y,w,h};i.group=group;d.Add(i);
}
void Wire(Document &d,Layer l,Point a,Point b) {
  if(a.x!=b.x && a.y!=b.y)throw std::runtime_error("Fil orthogonal attendu");
  Shape(d,l,std::min(a.x,b.x),std::min(a.y,b.y),std::abs(a.x-b.x)+1,std::abs(a.y-b.y)+1);
}
void PortAt(Document &d,Kind k,const std::string &name,Point p,State initial=State::Low) {
  Item i;i.kind=k;i.name=name;i.box={p.x,p.y,1,1};i.initial=initial;i.watch=k==Kind::Input || k==Kind::Output;d.Add(i);
}
void Element(Document &d,Kind k,const std::string &name,Point a,Point b,double value) {
  Shape(d,Layer::Metal1,a.x,a.y);Shape(d,Layer::Metal1,b.x,b.y);
  Item i;i.kind=k;i.name=name;i.box={a.x,a.y,1,1};i.end=b;i.value=value;d.Add(i);
}
void Bjt(Document &d,int x,int y,bool pnp=false,const std::string &group="Q") {
  Shape(d,pnp?Layer::CollectorP:Layer::CollectorN,x,y,9,9,group);
  Shape(d,Layer::Base,x+2,y+2,5,5,group);Shape(d,Layer::Emitter,x+4,y+4,1,1,group);
  for(int delta:{0,2,4}){Shape(d,Layer::Metal1,x+delta,y+4,1,1,group);Shape(d,Layer::Contact,x+delta,y+4,1,1,group);}
}
void Note(Document &d,const std::string &text,int x,int y){Item i;i.kind=Kind::Note;i.name=text;i.box={x,y,1,1};d.Add(i);}
Document Bipolar(bool pnp,bool pair,bool series) {
  Document d;d.title=pnp?"Inverseur PNP":pair?(series?"NAND bipolaire":"NOR bipolaire"):"Inverseur bipolaire";
  Bjt(d,0,0,pnp,"Q1");
  Element(d,Kind::Resistor,"RL",{0,4},{0,-7},4700);
  PortAt(d,pnp?Kind::Ground:Kind::Supply,pnp?"GND":"VCC",{0,-7});
  PortAt(d,Kind::Output,"Y",{0,4});
  Element(d,Kind::Resistor,"RB_A",{2,4},{2,16},22000);PortAt(d,Kind::Input,"A",{2,16},pnp?State::High:State::Low);
  if(!pair){Wire(d,Layer::Metal1,{4,4},{4,12});PortAt(d,pnp?Kind::Supply:Kind::Ground,pnp?"VCC":"GND",{4,12});}
  else {
    Bjt(d,20,0,false,"Q2");
    Element(d,Kind::Resistor,"RB_B",{22,4},{22,16},22000);PortAt(d,Kind::Input,"B",{22,16});
    Wire(d,Layer::Metal1,{24,4},{24,12});PortAt(d,Kind::Ground,"GND",{24,12});
    Point start=series?Point{4,4}:Point{0,4};
    if(series)Wire(d,Layer::Metal2,start,{20,4});
    else {Wire(d,Layer::Metal2,start,{0,-2});Wire(d,Layer::Metal2,{0,-2},{20,-2});Wire(d,Layer::Metal2,{20,-2},{20,4});}
    Shape(d,Layer::Via,start.x,start.y);Shape(d,Layer::Via,20,4);
    if(!series){Wire(d,Layer::Metal2,{4,4},{4,12});Wire(d,Layer::Metal2,{4,12},{24,12});Shape(d,Layer::Via,4,4);Shape(d,Layer::Via,24,12);}
  }
  Note(d,"Collecteur / base / emetteur : les regions creent les transistors",-4,-12);
  Note(d,"Clic molette sur A ou B pour changer leur etat",-4,20);
  return d;
}
Document Nmos(const std::string &name="nmos-inverter") {
  bool nand=name=="nmos-nand",nor=name=="nmos-nor",hold=name=="nmos-sample-hold";
  Document d;d.technology="nmos";d.title=nand?"NAND NMOS":nor?"NOR NMOS":hold?"Echantillonneur NMOS et charge conservee":"Inverseur NMOS extrait du dessin";
  Shape(d,Layer::Active,0,0,11,3,"M1");
  if(nand){Shape(d,Layer::Poly,3,-3,1,9,"M1");Shape(d,Layer::Poly,7,-3,1,9,"M2");}
  else Shape(d,Layer::Poly,5,-3,1,9,"M1");
  for(Point p:{Point{0,1},Point{10,1}}){Shape(d,Layer::Metal1,p.x,p.y);Shape(d,Layer::Contact,p.x,p.y);}
  if(nand){for(int x:{3,7}){Shape(d,Layer::Metal1,x,-3);Shape(d,Layer::Contact,x,-3);}PortAt(d,Kind::Input,"A",{3,-3});PortAt(d,Kind::Input,"B",{7,-3});}
  else {Shape(d,Layer::Metal1,5,-3);Shape(d,Layer::Contact,5,-3);PortAt(d,Kind::Input,hold?"CLK":"A",{5,-3});}
  if(hold){PortAt(d,Kind::Input,"D",{0,1});PortAt(d,Kind::Output,"Q",{10,1});Element(d,Kind::Capacitor,"CHOLD",{10,1},{10,9},1e-9);PortAt(d,Kind::Ground,"GND",{10,9});}
  else {Element(d,Kind::Resistor,"RL",{0,1},{0,-7},10000);PortAt(d,Kind::Supply,"VCC",{0,-7});PortAt(d,Kind::Output,"Y",{0,1});PortAt(d,Kind::Ground,"GND",{10,1});}
  if(nor){
    Shape(d,Layer::Active,0,10,11,3,"M2");Shape(d,Layer::Poly,5,8,1,9,"M2");
    for(Point p:{Point{0,11},Point{10,11},Point{5,16}}){Shape(d,Layer::Metal1,p.x,p.y);Shape(d,Layer::Contact,p.x,p.y);}
    for(int x:{0,10}){Wire(d,Layer::Metal2,{x,1},{x,11});Shape(d,Layer::Via,x,1);Shape(d,Layer::Via,x,11);}
    PortAt(d,Kind::Input,"B",{5,16});
  }
  Note(d,"Le poly traverse l'actif : une grille NMOS est extraite",-4,-12);
  return d;
}
Document Cml() {
  Document d;d.title="Paire differentielle bipolaire - observation analogique";
  Bjt(d,0,0,false,"Q1");Bjt(d,20,0,false,"Q2");
  Element(d,Kind::Resistor,"RL1",{0,4},{0,-8},1000);Element(d,Kind::Resistor,"RL2",{20,4},{20,-8},1000);
  Wire(d,Layer::Metal2,{0,-8},{20,-8});Shape(d,Layer::Via,0,-8);Shape(d,Layer::Via,20,-8);PortAt(d,Kind::Supply,"VCC",{0,-8});
  Wire(d,Layer::Metal2,{4,4},{24,4});Shape(d,Layer::Via,4,4);Shape(d,Layer::Via,24,4);
  Element(d,Kind::Current,"ITAIL",{4,4},{4,16},.001);PortAt(d,Kind::Ground,"GND",{4,16});
  Element(d,Kind::Resistor,"REF_H",{22,4},{22,-8},10000);
  Wire(d,Layer::Metal1,{20,-8},{22,-8});
  Element(d,Kind::Resistor,"REF_L",{22,4},{22,16},10000);
  Wire(d,Layer::Metal2,{4,16},{22,16});Shape(d,Layer::Via,4,16);Shape(d,Layer::Via,22,16);
  PortAt(d,Kind::Input,"A",{2,4});PortAt(d,Kind::Output,"Y",{0,4});PortAt(d,Kind::Output,"YN",{20,4});
  Item probe;probe.kind=Kind::Probe;probe.name="Reference";probe.box={22,4,1,1};probe.watch=true;d.Add(probe);
  Note(d,"Courant commute entre Q1 et Q2. Observer Y et YN en volts.",-4,-14);
  Note(d,"Modele pedagogique : ni cellule Ferranti ni seuils CML calibres.",-4,21);
  return d;
}
}
std::vector<std::string> Examples(){return {"nmos-inverter","nmos-nand","nmos-nor","nmos-sample-hold"};}
Document Example(const std::string &name) {
  if(name=="bipolar-inverter")return Bipolar(false,false,false);
  if(name=="bipolar-nor")return Bipolar(false,true,false);
  if(name=="bipolar-nand")return Bipolar(false,true,true);
  if(name=="pnp-inverter")return Bipolar(true,false,false);
  if(name=="nmos-inverter" || name=="nmos-nand" || name=="nmos-nor" || name=="nmos-sample-hold")return Nmos(name);
  if(name=="differential-pair")return Cml();
  throw std::runtime_error("Exemple inconnu: "+name);
}
}
