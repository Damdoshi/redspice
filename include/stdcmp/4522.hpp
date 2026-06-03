// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_4522_HPP__
# define                        __HBS_4522_HPP__
# include                       "AComponent.hpp"

namespace                       hbs
{
  class                         CMP_4522 : public hbs::AComponent<16>
  {
  private:
    std::string                 canonical;
    std::string                 display;
    unsigned char               modulo;
    unsigned char               value;
    size_t                      last_tick;

    const std::string           &GetType(void) const;
    unsigned char               ReadPreset(void);
    void                        Update(void);
    hbs::Tristate               OutputBit(size_t n) const;

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

    CMP_4522(const hbs::Timer  &timer,
             const std::string &name,
             const std::string &position,
             const std::string &canonical,
             const std::string &display,
             unsigned char      modulo);
    virtual ~CMP_4522(void);
  };
}

#endif //                       __HBS_4522_HPP__
