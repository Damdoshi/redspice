// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_6264_HPP__
# define                        __HBS_6264_HPP__
# include                       <stdint.h>
# include                       <string>
# include                       <vector>
# include                       "Memory8.hpp"

namespace                       hbs
{
  class                         CMP_6264 : public hbs::AComponent<28>, public hbs::Memory8Utility
  {
  private:
    const std::string           &GetType(void) const;
    std::vector<uint8_t>        memory;
    size_t                      last_write_tick;

    size_t                      Address(void);
    uint8_t                     ReadDataBus(void);
    bool                        Selected(void);
    bool                        OutputEnabled(void);
    bool                        WriteEnabled(void);
    void                        TickWrite(void);
    hbs::Tristate               DataBit(size_t n);

  public:
    static bool                 TypeMatches(const std::string &type);
    static hbs::IComponent      *Create(hbs::Timer        &timer,
                                        const std::string &type,
                                        const std::string &name,
                                        const std::string &value,
                                        const std::string &position);
    virtual std::string         GetDisplayType(void) const;
    virtual bool                DrivesPin(size_t          n) const;

  public:
    hbs::Tristate               Compute(size_t            n);

    CMP_6264(const hbs::Timer   &timer,
             const std::string  &name,
             const std::string  &file,
             const std::string  &position);
    virtual ~CMP_6264(void);
  };
}

#endif //                       __HBS_6264_HPP__
