// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_7475_HPP__
# define                        __HBS_7475_HPP__
# include                       <bitset>
# include                       "AComponent.hpp"

namespace                       hbs
{
  class                         CMP_7475 : public hbs::AComponent<16>
  {
  private:
    const std::string           &GetType(void) const;
    std::bitset<4>              value;
    size_t                      last_tick;

    void                        UpdateLatch(void);
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

    CMP_7475(const hbs::Timer   &timer,
             const std::string  &name,
             const std::string  &position);
    virtual ~CMP_7475(void);
  };
}

#endif //                       __HBS_7475_HPP__
