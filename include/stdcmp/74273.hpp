// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_74273_HPP__
# define                        __HBS_74273_HPP__
# include                       "AComponent.hpp"

namespace                       hbs
{
  class                         CMP_74273 : public hbs::AComponent<20>
  {
  private:
    const std::string           &GetType(void) const;
    virtual bool                DrivesPin(size_t n) const;

    hbs::Tristate               value[8];
    size_t                      last_tick;

    void                        UpdateRegister(void);

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

    CMP_74273(const hbs::Timer      &timer,
             const std::string  &name,
             const std::string  &pos);
    virtual ~CMP_74273(void);
  };
}

#endif
