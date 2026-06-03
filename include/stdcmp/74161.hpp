// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_74161_HPP__
# define                        __HBS_74161_HPP__
# include                       "AComponent.hpp"

namespace                       hbs
{
  class                         CMP_74161 : public hbs::AComponent<16>
  {
  private:
    enum                        CounterKind
      {
        BCD_ASYNC_CLEAR,
        BINARY_ASYNC_CLEAR,
        BCD_SYNC_CLEAR,
        BINARY_SYNC_CLEAR
      };

    std::string                 canonical;
    std::string                 display;
    CounterKind                 kind;
    unsigned char               value;
    size_t                      last_tick;

    const std::string           &GetType(void) const;
    unsigned char               Modulo(void) const;
    bool                        IsSynchronousClear(void) const;
    unsigned char               ReadPreset(void);
    void                        UpdateCounter(void);
    hbs::Tristate               OutputBit(size_t n) const;
    hbs::Tristate               CarryOut(void);

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

    CMP_74161(const hbs::Timer  &timer,
              const std::string &name,
              const std::string &position,
              const std::string &canonical,
              const std::string &display,
              CounterKind       kind);
    virtual ~CMP_74161(void);
  };
}

#endif //                       __HBS_74161_HPP__
