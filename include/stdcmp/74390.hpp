// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_74390_HPP__
# define                        __HBS_74390_HPP__
# include                       "AComponent.hpp"

namespace                       hbs
{
  class                         CMP_74390 : public hbs::AComponent<16>
  {
  private:
    unsigned char               q0[2];
    unsigned char               div5[2];
    size_t                      last_tick[2];

    const std::string           &GetType(void) const;
    void                        UpdateHalf(size_t half);
    hbs::Tristate               Output(size_t n) const;

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

    CMP_74390(const hbs::Timer  &timer,
              const std::string &name,
              const std::string &position);
    virtual ~CMP_74390(void);
  };
}

#endif //                       __HBS_74390_HPP__
