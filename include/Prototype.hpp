// RED Spice - teaching symbols and simple discrete components.
#ifndef HBS_PROTOTYPE_HPP
#define HBS_PROTOTYPE_HPP
#include "AComponent.hpp"
#include "Output.hpp"

namespace hbs
{
  enum PrototypeKind
  {
    PT_AND, PT_NAND, PT_OR, PT_NOR, PT_BUF, PT_NOT, PT_XOR, PT_XNOR,
    PT_PROBE, PT_DIODE, PT_SWITCH, PT_NPN, PT_PNP, PT_RESISTOR,
    PT_LED, PT_PUSHBUTTON, PT_PUSHBUTTON_COMPACT, PT_CAPACITOR, PT_INVALID
  };
  PrototypeKind PrototypeType(const std::string &type);
  const std::string &PrototypeName(PrototypeKind kind);
  size_t PrototypePinCount(PrototypeKind kind);
  Position PrototypePinPosition(PrototypeKind kind, size_t pin);
  bool PrototypeIsUnder(PrototypeKind kind, Position point);
  unsigned int PrototypeColor(Tristate value);

  // A small drawing surface shared by the editor and SVG visual checks.
  class SymbolCanvas
  {
  public:
    virtual void Line(Position a, Position b, unsigned color) = 0;
    virtual void Circle(Position center, Size radius, unsigned color, bool fill) = 0;
    virtual void Text(Position top_center, double height, unsigned color,
                      const std::string &text) = 0;
    virtual ~SymbolCanvas() {}
  };
  class ScreenSymbolCanvas : public SymbolCanvas
  {
    Screen &screen;
    Position origin;
    int rotation;
  public:
    ScreenSymbolCanvas(Screen &screen, Position origin, int rotation = 0);
    void Line(Position a, Position b, unsigned color) override;
    void Circle(Position center, Size radius, unsigned color, bool fill) override;
    void Text(Position top_center, double height, unsigned color,
              const std::string &text) override;
  };
  void DrawPrototypeSymbol(SymbolCanvas &canvas, PrototypeKind kind,
                           const std::string &name, const std::string &value,
                           bool closed, const Tristate states[4], bool lit = false);
  bool DrawPrototypePreview(Screen &screen, const std::string &type,
                            int x, int y, int width, int height);

  class Prototype : public AComponent<4>
  {
    PrototypeKind kind;
    std::string value;
    bool closed = false;
    bool lit = false;
    void CheckPin(size_t pin) const;
  public:
    Prototype(const Timer &timer, PrototypeKind kind, const std::string &name,
              const std::string &value, const std::string &position);
    const std::string &GetType() const override;
    std::string GetDisplayType() const override;
    std::string GetDefinitionValue() const override;
    size_t GetPinCount() const override;
    Position GetPinPosition(size_t pin) const override;
    bool IsUnder(const Screen &screen, const t_bunny_position &point) const override;
    void Draw(Screen &screen) const override;
    void SetLink(size_t pin, IComponent &other, size_t other_pin,
                 const std::string &position = "") override;
    Tristate GetPin(size_t pin) override;
    Tristate Compute(size_t pin) override;
    bool DrivesPin(size_t pin) const override;
    bool IsPassivePin(size_t pin) const override;
    Tristate GetPassiveConnections(size_t pin, std::vector<SignalConnection> &out) override;
    void EndTick() override;
    bool Toggle();
    bool SetPressed(bool pressed);
    bool IsMomentary() const;
    bool IsClosed() const { return closed; }
    bool IsLit() const { return lit; }
    PrototypeKind GetKind() const { return kind; }
    static IComponent *Create(Timer &timer, const std::string &type,
                              const std::string &name, const std::string &value,
                              const std::string &position);
  };

  class Probe : public Output
  {
  public:
    Probe(const Timer &timer, const std::string &name, const std::string &position);
    const std::string &GetType() const override;
    std::string GetDisplayType() const override;
    Position GetPinPosition(size_t pin) const override;
    bool IsUnder(const Screen &screen, const t_bunny_position &point) const override;
    void Draw(Screen &screen) const override;
  };
}
#endif
