// RED Spice - explicitly digital models; no voltage/current solver.
#include "Prototype.hpp"
#include "Logic.hpp"
#include <algorithm>
#include <cctype>

static std::string normalized(std::string text)
{
  for (char &c : text) c = char(std::toupper(static_cast<unsigned char>(c)));
  return text;
}

const std::string &hbs::PrototypeName(PrototypeKind kind)
{
  static const std::string names[] = {
    "AND", "NAND", "OR", "NOR", "BUF", "NOT", "XOR", "XNOR", "PROBE",
    "DIODE", "SWITCH", "NPN", "PNP", "RESISTOR", "LED", "PUSHBUTTON",
    "PUSHBUTTON_COMPACT", "CAPACITOR", "INVALID"
  };
  return names[kind >= PT_AND && kind <= PT_INVALID ? kind : PT_INVALID];
}

hbs::PrototypeKind hbs::PrototypeType(const std::string &type)
{
  std::string name = normalized(type);
  for (int i = PT_AND; i < PT_INVALID; ++i)
    if (name == PrototypeName(PrototypeKind(i))) return PrototypeKind(i);
  struct Alias { const char *name; PrototypeKind kind; };
  static const Alias aliases[] = {
    {"ET",PT_AND}, {"NON_ET",PT_NAND}, {"OU",PT_OR}, {"NON_OU",PT_NOR},
    {"OUI",PT_BUF}, {"BUFFER",PT_BUF}, {"NON",PT_NOT}, {"NON_XOR",PT_XNOR},
    {"NXOR",PT_XNOR}, {"SONDE",PT_PROBE}, {"INTERRUPTEUR",PT_SWITCH},
    {"RESISTANCE",PT_RESISTOR}, {"DEL",PT_LED}, {"BP",PT_PUSHBUTTON},
    {"BOUTON",PT_PUSHBUTTON}, {"BP_COMPACT",PT_PUSHBUTTON_COMPACT},
    {"CONDENSATEUR",PT_CAPACITOR}, {"ELECTROLYTIC",PT_CAPACITOR}
  };
  for (const Alias &alias : aliases) if (name == alias.name) return alias.kind;
  return PT_INVALID;
}

size_t hbs::PrototypePinCount(PrototypeKind kind)
{
  if (kind == PT_PROBE) return 1;
  if (kind == PT_PUSHBUTTON || kind == PT_PUSHBUTTON_COMPACT) return 4;
  if (kind == PT_NPN || kind == PT_PNP ||
      (kind >= PT_AND && kind <= PT_XNOR && kind != PT_BUF && kind != PT_NOT)) return 3;
  return 2;
}

hbs::Position hbs::PrototypePinPosition(PrototypeKind kind, size_t pin)
{
  if (!pin || pin > PrototypePinCount(kind)) throw BadPin("Prototype: Bad pin.");
  if (kind == PT_PROBE) return {-2,0};
  if (kind == PT_PUSHBUTTON || kind == PT_PUSHBUTTON_COMPACT)
    {
      double height = kind == PT_PUSHBUTTON ? 3 : 1;
      const Position pins[] = {{0,0},{0,height},{3,height},{3,0}};
      return pins[pin-1];
    }
  if (kind == PT_NPN || kind == PT_PNP)
    {
      const Position pins[] = {{-3,0},{3,-2},{3,2}}; // B, C, E
      return pins[pin-1];
    }
  if (PrototypePinCount(kind) == 3)
    {
      const Position pins[] = {{-3,-1},{-3,1},{3,0}};
      return pins[pin-1];
    }
  return pin == 1 ? Position{-3,0} : Position{3,0};
}

bool hbs::PrototypeIsUnder(PrototypeKind kind, Position point)
{
  if (kind == PT_PROBE) return point.x>=-2.4 && point.x<=1.1 && std::abs(point.y)<=1.1;
  if (kind == PT_PUSHBUTTON || kind == PT_PUSHBUTTON_COMPACT)
    return point.x>=-0.4 && point.x<=3.4 && point.y>=-0.4 &&
      point.y <= (kind == PT_PUSHBUTTON ? 3.4 : 1.4);
  double height = kind <= PT_XNOR || kind == PT_NPN || kind == PT_PNP ? 2.4 : 1.1;
  return std::abs(point.x)<=3.4 && std::abs(point.y)<=height;
}

hbs::Prototype::Prototype(const Timer &timer, PrototypeKind kind, const std::string &name,
                          const std::string &text, const std::string &position)
  : AComponent(timer,name,position), kind(kind), value(text)
{
  if (kind == PT_INVALID || kind == PT_PROBE) throw BadComponent("Invalid prototype kind");
  if (kind == PT_SWITCH)
    {
      std::string state = normalized(text);
      if (state.empty() || state=="0" || state=="OPEN" || state=="OUVERT") closed=false;
      else if (state=="1" || state=="CLOSED" || state=="FERME") closed=true;
      else throw BadComponent("SWITCH expects open/closed or 0/1");
      value.clear();
    }
  else if (IsMomentary())
    {
      if (!text.empty()) throw BadComponent("Momentary buttons have no saved pressed state");
    }
  else if (kind == PT_RESISTOR && value.empty()) value="1k";
  else if (kind == PT_CAPACITOR && value.empty()) value="10uF";
}

void hbs::Prototype::CheckPin(size_t pin) const
{ if (!pin || pin>GetPinCount()) throw BadPin(GetType()+": Bad pin."); }
const std::string &hbs::Prototype::GetType() const { return PrototypeName(kind); }
size_t hbs::Prototype::GetPinCount() const { return PrototypePinCount(kind); }
hbs::Position hbs::Prototype::GetPinPosition(size_t pin) const
{ CheckPin(pin); return LocalToWorld(PrototypePinPosition(kind,pin)); }
bool hbs::Prototype::IsUnder(const Screen &, const t_bunny_position &point) const
{ return PrototypeIsUnder(kind, WorldToLocal({double(point.x),double(point.y)})); }
std::string hbs::Prototype::GetDisplayType() const
{
  if (kind <= PT_XNOR) return GetType()+" - porte logique pedagogique";
  return GetType()+" - composant de prototypage, modele logique";
}
std::string hbs::Prototype::GetDefinitionValue() const
{
  if (kind == PT_SWITCH) return closed?"closed":"open";
  if (IsMomentary()) return "";
  return value;
}
void hbs::Prototype::SetLink(size_t pin, IComponent &other, size_t other_pin,
                            const std::string &geometry)
{ CheckPin(pin); AComponent::SetLink(pin,other,other_pin,geometry); }
hbs::Tristate hbs::Prototype::GetPin(size_t pin)
{ CheckPin(pin); return AComponent::GetPin(pin); }
bool hbs::Prototype::DrivesPin(size_t pin) const
{ return kind<=PT_XNOR && pin==GetPinCount(); }
bool hbs::Prototype::IsPassivePin(size_t pin) const
{
  if (!pin || pin>GetPinCount() || kind<=PT_XNOR) return false;
  return (kind!=PT_NPN && kind!=PT_PNP) || pin!=1;
}

hbs::Tristate hbs::Prototype::GetPassiveConnections(size_t pin,
                                                  std::vector<SignalConnection> &out)
{
  CheckPin(pin);
  if (kind == PT_RESISTOR) out.push_back({this,3-pin,true});
  else if (kind == PT_SWITCH && closed) out.push_back({this,3-pin});
  else if (IsMomentary())
    {
      out.push_back({this,pin==1?2u:pin==2?1u:pin==3?4u:3u});
      if (closed) out.push_back({this,pin<=2?3u:1u});
    }
  else if (kind == PT_DIODE)
    out.push_back({this,3-pin,false,pin==2?2u:1u});
  else if ((kind == PT_NPN || kind == PT_PNP) && pin!=1)
    {
      Tristate base=GetPin(1);
      if (base==BROKEN) return BROKEN;
      if (!LogicKnown(base)) return UNDEFINED;
      if (base==(kind==PT_NPN?TRUE:FALSE)) out.push_back({this,pin==2?3u:2u});
    }
  // LED is a digital indicator across A/K, not an analog diode load.
  // Capacitors are open circuits; there is deliberately no transient solver.
  return HIGH_IMPEDANCE;
}

hbs::Tristate hbs::Prototype::Compute(size_t pin)
{
  CheckPin(pin);
  if (kind > PT_XNOR)
    return timeline[timer.GetTime()][pin]=ResolveSignal(*this,pin);
  Tristate state;
  if (AlreadyComputed(pin,state)) return state;
  auto result=PresetOutput(pin);
  if (!DrivesPin(pin)) return result->second=GetPin(pin);
  Tristate values[2]={GetPin(1),UNDEFINED};
  if (GetPinCount()==3) values[1]=GetPin(2);
  switch (kind)
    {
    case PT_AND: state=LogicAnd(values,2); break;
    case PT_NAND: state=LogicNand(values,2); break;
    case PT_OR: state=LogicOr(values,2); break;
    case PT_NOR: state=LogicNor(values,2); break;
    case PT_XOR: state=values[0]==BROKEN||values[1]==BROKEN?BROKEN:LogicXor(values,2); break;
    case PT_XNOR: state=values[0]==BROKEN||values[1]==BROKEN?BROKEN:LogicXnor(values,2); break;
    case PT_NOT: state=LogicNot(values[0]); break;
    case PT_BUF: state=values[0]==HIGH_IMPEDANCE?UNDEFINED:values[0]; break;
    default: state=UNDEFINED; break;
    }
  return result->second=state;
}

bool hbs::Prototype::IsMomentary() const
{ return kind==PT_PUSHBUTTON || kind==PT_PUSHBUTTON_COMPACT; }
bool hbs::Prototype::Toggle()
{ if(kind!=PT_SWITCH)return false; closed=!closed; return true; }
bool hbs::Prototype::SetPressed(bool pressed)
{
  if (!IsMomentary() || closed==pressed) return false;
  closed=pressed;
  return true;
}
void hbs::Prototype::EndTick()
{
  if (kind==PT_LED)
    {
      auto anode=GetPin(1), cathode=GetPin(2);
      lit=anode==TRUE && cathode==FALSE;
    }
}

void hbs::Prototype::Draw(Screen &screen) const
{
  Tristate states[4]={UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED};
  if (!timeline.empty())
    for(size_t pin=1;pin<=GetPinCount();++pin)
      {
        auto found=timeline.rbegin()->second.find(pin);
        if(found!=timeline.rbegin()->second.end()) states[pin-1]=found->second;
      }
  ScreenSymbolCanvas canvas(screen,position,GetRotation());
  DrawPrototypeSymbol(canvas,kind,name,value,closed,states,lit);
}

hbs::IComponent *hbs::Prototype::Create(Timer &timer, const std::string &type,
                                      const std::string &name, const std::string &value,
                                      const std::string &position)
{
  auto kind=PrototypeType(type);
  if (kind==PT_INVALID) return nullptr;
  if (kind==PT_PROBE) return new Probe(timer,name,position);
  return new Prototype(timer,kind,name,value,position);
}

hbs::Probe::Probe(const Timer &timer, const std::string &name, const std::string &position)
  : AComponent(timer,name,position), Output(timer,name,position) {}
const std::string &hbs::Probe::GetType() const { return PrototypeName(PT_PROBE); }
std::string hbs::Probe::GetDisplayType() const { return "PROBE - sonde logique cinq couleurs"; }
hbs::Position hbs::Probe::GetPinPosition(size_t pin) const
{ return LocalToWorld(PrototypePinPosition(PT_PROBE,pin)); }
bool hbs::Probe::IsUnder(const Screen &, const t_bunny_position &point) const
{ return PrototypeIsUnder(PT_PROBE,WorldToLocal({double(point.x),double(point.y)})); }
void hbs::Probe::Draw(Screen &screen) const
{
  Tristate states[4]={UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED};
  if (!timeline.empty())
    {
      auto value=timeline.rbegin()->second.find(1);
      if(value!=timeline.rbegin()->second.end()) states[0]=value->second;
    }
  ScreenSymbolCanvas canvas(screen,position,GetRotation());
  DrawPrototypeSymbol(canvas,PT_PROBE,name,"",false,states);
}
