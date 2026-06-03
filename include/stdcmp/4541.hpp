// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_4541_HPP__
# define                        __HBS_4541_HPP__
# include                       "AComponent.hpp"

namespace                       hbs
{
  class                         CMP_4541 : public hbs::AComponent<14>
  {
  private:
    const std::string           &GetType(void) const;
    unsigned int                counter;
    size_t                      last_tick;

    void                        Tick(void);
    unsigned int                SelectedStage(void);
    hbs::Tristate               Output(void);

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

    CMP_4541(const hbs::Timer   &timer,
             const std::string  &name,
             const std::string  &pos);
    virtual ~CMP_4541(void);
  };
}

#endif
