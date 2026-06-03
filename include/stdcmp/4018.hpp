// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_4018_HPP__
# define                        __HBS_4018_HPP__
# include                       "AComponent.hpp"

namespace                       hbs
{
  class                         CMP_4018 : public hbs::AComponent<16>
  {
  private:
    hbs::Tristate               qbar[5];
    size_t                      last_tick;

    const std::string           &GetType(void) const;
    void                        Update(void);
    hbs::Tristate               ReadJam(size_t idx);
    size_t                      OutputIndex(size_t n) const;

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

    CMP_4018(const hbs::Timer  &timer,
             const std::string &name,
             const std::string &position);
    virtual ~CMP_4018(void);
  };
}

#endif //                       __HBS_4018_HPP__
