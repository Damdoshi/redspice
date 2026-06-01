// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_4027_HPP__
# define                        __HBS_4027_HPP__
# include                       "AComponent.hpp"

namespace                       hbs
{
  class                         CMP_4027 : public hbs::AComponent<16>
  {
  private:
    const std::string           &GetType(void) const;
    virtual bool                DrivesPin(size_t n) const { return (n == 1 || n == 2 || n == 14 || n == 15); }

    hbs::Tristate               val[2];
    size_t                      last_tick[2];

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

    CMP_4027(const hbs::Timer   &timer,
             const std::string  &name,
             const std::string  &pos);
    virtual ~CMP_4027(void);
  };
}

#endif
