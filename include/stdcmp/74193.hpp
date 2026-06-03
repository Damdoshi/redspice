// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_74193_HPP__
# define                        __HBS_74193_HPP__
# include                       "AComponent.hpp"

namespace                       hbs
{
  class                         CMP_74193 : public hbs::AComponent<16>
  {
  private:
    bool                        bcd;
    std::string                 canonical;
    std::string                 display;
    unsigned char               value;
    size_t                      last_tick;

    const std::string           &GetType(void) const;
    unsigned char               Modulo(void) const;
    unsigned char               ReadPreset(void);
    void                        UpdateCounter(void);
    hbs::Tristate               OutputBit(size_t n) const;
    hbs::Tristate               CarryOut(void) const;
    hbs::Tristate               BorrowOut(void) const;

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

    CMP_74193(const hbs::Timer  &timer,
              const std::string &name,
              const std::string &position,
              const std::string &canonical,
              const std::string &display,
              bool              bcd);
    virtual ~CMP_74193(void);
  };
}

#endif //                       __HBS_74193_HPP__
