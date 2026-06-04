// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_6522_HPP__
# define                        __HBS_6522_HPP__
# include                       <stdint.h>
# include                       <string>
# include                       "AComponent.hpp"

namespace                       hbs
{
  class                         CMP_6522 : public hbs::AComponent<40>
  {
  private:
    const std::string           &GetType(void) const;
    uint8_t                     reg[16];
    size_t                      last_write_tick;

    bool                        Selected(void);
    uint8_t                     RegisterAddress(void);
    uint8_t                     ReadDataBus(void);
    uint8_t                     ReadPortA(void);
    uint8_t                     ReadPortB(void);
    uint8_t                     ReadRegister(uint8_t addr);
    void                        WriteRegister(uint8_t addr, uint8_t value);
    void                        TickBus(void);
    hbs::Tristate               DataBit(size_t n);
    hbs::Tristate               PortBit(size_t n);

  public:
    static bool                 TypeMatches(const std::string &type);
    static hbs::IComponent      *Create(hbs::Timer        &timer,
                                        const std::string &type,
                                        const std::string &name,
                                        const std::string &value,
                                        const std::string &position);
    virtual std::string         GetDisplayType(void) const;
    virtual std::string         GetDefinitionValue(void) const;
    virtual bool                DrivesPin(size_t          n) const;

  public:
    hbs::Tristate               Compute(size_t            n);

    CMP_6522(const hbs::Timer   &timer,
             const std::string  &name,
             const std::string  &position);
    virtual ~CMP_6522(void);
  };
}

#endif //                       __HBS_6522_HPP__
