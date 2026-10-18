// Jason Brillante "Damdoshi" / RED Spice
#ifndef __HBS_6502_HPP__
#define __HBS_6502_HPP__
#include "AComponent.hpp"
#include "W65C02.hpp"

namespace hbs
{
  // Historical class name retained for source compatibility. PDIP-40 W65C02S.
  class CMP_6502 : public AComponent<40>
  {
    W65C02 core;
    size_t last_tick;
    Tristate previous_clock = UNDEFINED, previous_so = UNDEFINED;
    Tristate clock = UNDEFINED;
    bool reset_asserted = false, bus_enabled = false;
    uint8_t sampled_data = 0xff;
    void Tick();
    void Publish();
    Tristate PinValue(size_t pin) const;
    const std::string &GetType() const override;

  public:
    static bool TypeMatches(const std::string &type);
    static IComponent *Create(Timer &timer, const std::string &type,
                             const std::string &name, const std::string &value,
                             const std::string &position);
    std::string GetDisplayType() const override;
    std::string GetDefinitionValue() const override;
    bool DrivesPin(size_t pin) const override;
    Tristate Compute(size_t pin) override;
    void BeginTick() override { Tick(); }
    const W65C02 &GetCore() const { return core; }
    CMP_6502(const Timer &timer, const std::string &name, const std::string &position);
  };
}
#endif
