#ifndef HBS_SILICON_CHIP_HPP
#define HBS_SILICON_CHIP_HPP
#include "AComponent.hpp"
#include "Silicon.hpp"
#include <limits>

namespace hbs {
class SiliconNetwork;
class SiliconChip: public AComponent<64> {
  friend class SiliconNetwork;
  std::string source_path;
  silicon::Document layout;
  std::shared_ptr<silicon::SimulationSession> session;
  std::shared_ptr<SiliconNetwork> network;
  std::vector<int> package_nodes;
  std::vector<std::string> package_names;
  void ReadPackage();
protected:
  Tristate GetPinDisplayState(size_t pin) const override;
public:
  SiliconChip(const Timer &timer,const std::string &name,const std::string &file,const std::string &position);
  ~SiliconChip() override;
  const std::string &GetType() const override;
  std::string GetDisplayType() const override;
  std::string GetDefinitionValue() const override {return source_path;}
  size_t GetPinCount() const override {return package_nodes.size();}
  bool DrivesPin(size_t pin) const override;
  Tristate Compute(size_t pin=1) override;
  Position GetPinPosition(size_t pin) const override;
  bool IsUnder(const Screen &screen,const t_bunny_position &p) const override;
  void Draw(Screen &screen) const override;
  void SetLink(size_t pin,IComponent &component,size_t other,const std::string &pos="") override;
  void DisconnectFrom(const IComponent *component) override;
  bool Reload();
  void ResetSimulation();
  const std::string &SourcePath() const {return source_path;}
  std::shared_ptr<silicon::SimulationSession> Session() const {return session;}
};
}
#endif
