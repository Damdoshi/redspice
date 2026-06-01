// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_4099_HPP__
# define                        __HBS_4099_HPP__
# include                       "AComponent.hpp"

namespace                       hbs
{
  class                         CMP_4099 : public hbs::AComponent<16>
  {
  private:
    const std::string           &GetType(void) const;
    virtual bool                DrivesPin(size_t n) const;

    hbs::Tristate               value[8];
    size_t                      last_update;

    void                        UpdateLatch(void);

  public:
    static bool                 TypeMatches(const std::string &type);
    static hbs::IComponent      *Create(hbs::Timer             &timer,
                                        const std::string      &type,
                                        const std::string      &name,
                                        const std::string      &value,
                                        const std::string      &position);
    virtual std::string         GetDisplayType(void) const;

  public:
    hbs::Tristate               Compute(size_t n);

    CMP_4099(const hbs::Timer   &timer,
             const std::string  &name,
             const std::string  &pos);
    virtual ~CMP_4099(void);
  };
}

#endif
