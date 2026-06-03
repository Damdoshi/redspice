// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_4098_HPP__
# define                        __HBS_4098_HPP__
# include                       "AComponent.hpp"

namespace                       hbs
{
  class                         CMP_4098 : public hbs::AComponent<16>
  {
  private:
    const std::string           &GetType(void) const;
    size_t                      pulse_until[2];
    size_t                      last_tick[2];

    void                        Update(size_t id);
    bool                        Triggered(size_t id);
    bool                        Active(size_t id) const;

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

    CMP_4098(const hbs::Timer   &timer,
             const std::string  &name,
             const std::string  &pos);
    virtual ~CMP_4098(void);
  };
}

#endif
