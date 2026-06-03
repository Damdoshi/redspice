// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_4536_HPP__
# define                        __HBS_4536_HPP__
# include                       "AComponent.hpp"

namespace                       hbs
{
  class                         CMP_4536 : public hbs::AComponent<16>
  {
  private:
    const std::string           &GetType(void) const;
    unsigned int                counter;
    size_t                      last_tick;

    void                        Tick(void);
    unsigned int                SelectedStage(void);
    hbs::Tristate               DecodedOutput(void);

  public:
    static bool                 TypeMatches(const std::string &type);
    static hbs::IComponent      *Create(hbs::Timer        &timer,
                                        const std::string &type,
                                        const std::string &name,
                                        const std::string &value,
                                        const std::string &position);
    virtual std::string         GetDisplayType(void) const;
    virtual bool                DrivesPin(size_t n) const;

  public:
    virtual hbs::Tristate       Compute(size_t n);

    CMP_4536(const hbs::Timer   &timer,
             const std::string  &name,
             const std::string  &pos);
    virtual ~CMP_4536(void);
  };
}

#endif
