// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_4538_HPP__
# define                        __HBS_4538_HPP__
# include                       "4098.hpp"

namespace                       hbs
{
  class                         CMP_4538 : public hbs::CMP_4098
  {
  private:
    const std::string           &GetType(void) const;

  public:
    static bool                 TypeMatches(const std::string &type);
    static hbs::IComponent      *Create(hbs::Timer        &timer,
                                        const std::string &type,
                                        const std::string &name,
                                        const std::string &value,
                                        const std::string &position);
    virtual std::string         GetDisplayType(void) const;

    CMP_4538(const hbs::Timer   &timer,
             const std::string  &name,
             const std::string  &pos);
    virtual ~CMP_4538(void);
  };
}

#endif
