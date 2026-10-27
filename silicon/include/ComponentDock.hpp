#ifndef BCAD_COMPONENT_DOCK_HPP
#define BCAD_COMPONENT_DOCK_HPP
#include <algorithm>
#include <array>
#include <cctype>
#include <iomanip>
#include <ostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>

namespace bcad {
struct DockEntry {
  std::string kind, type, source;
  bool Empty() const { return kind.empty() && type.empty() && source.empty(); }
  std::tuple<std::string,std::string,std::string> Key() const {
    std::string name=type;
    if(kind=="component")for(char &c:name)c=char(std::tolower(static_cast<unsigned char>(c)));
    return {kind,name,source};
  }
  bool operator==(const DockEntry &other) const { return Key()==other.Key(); }
  void Validate() const {
    if(Empty())return;
    if(kind!="component" && kind!="tool" && kind!="cell")throw std::runtime_error("Type de favori inconnu");
    if(type.size()>128 || source.size()>2048 || type.find_first_of("\r\n\0",0,3)!=std::string::npos || source.find_first_of("\r\n\0",0,3)!=std::string::npos)
      throw std::runtime_error("Favori trop long ou invalide");
    if(kind=="cell" ? source.empty() || !type.empty() : type.empty() || !source.empty())throw std::runtime_error("Definition de favori invalide");
  }
};
class ComponentDock {
public:
  static constexpr size_t SlotsPerSide=10, SlotCount=2*SlotsPerSide;
  std::array<DockEntry,SlotCount> slots;
  enum AddResult { Added, AlreadyPresent, Full };
  AddResult Add(const DockEntry &entry) {
    entry.Validate();if(entry.Empty())throw std::runtime_error("Favori vide");
    for(const DockEntry &slot:slots)if(slot==entry)return AlreadyPresent;
    for(DockEntry &slot:slots)if(slot.Empty()){slot=entry;return Added;}
    return Full;
  }
  bool Remove(size_t slot) {
    if(slot>=SlotCount || slots[slot].Empty())return false;
    slots[slot]=DockEntry();return true;
  }
  bool Move(size_t from,size_t to) {
    if(from>=SlotCount || to>=SlotCount || from==to || slots[from].Empty())return false;
    std::swap(slots[from],slots[to]);return true;
  }
  void Validate() const {
    std::set<std::tuple<std::string,std::string,std::string>> seen;
    for(const DockEntry &entry:slots){entry.Validate();if(!entry.Empty() && !seen.insert(entry.Key()).second)throw std::runtime_error("Composant favori duplique");}
  }
  bool operator==(const ComponentDock &other) const { return slots==other.slots; }
  void Write(std::ostream &out) const {
    Validate();
    for(size_t slot=0;slot<SlotCount;++slot)if(!slots[slot].Empty()){
      const DockEntry &entry=slots[slot];
      out<<"favorite "<<(slot<SlotsPerSide?"left":"right")<<' '<<slot%SlotsPerSide<<' '
         <<std::quoted(entry.kind)<<' '<<std::quoted(entry.type)<<' '<<std::quoted(entry.source)<<'\n';
    }
  }
  void Read(std::istream &row,std::set<size_t> &seen) {
    std::string side,extra;int number=-1;DockEntry entry;
    if(!(row>>side>>number>>std::quoted(entry.kind)>>std::quoted(entry.type)>>std::quoted(entry.source)) || (row>>extra) ||
       (side!="left" && side!="right") || number<0 || number>=int(SlotsPerSide))throw std::runtime_error("Emplacement de favori invalide");
    size_t slot=size_t(number)+(side=="right"?SlotsPerSide:0);
    if(!seen.insert(slot).second || entry.Empty())throw std::runtime_error("Emplacement de favori vide ou duplique");
    entry.Validate();slots[slot]=entry;
  }
};
}
#endif
