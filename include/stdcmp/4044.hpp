// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_4044_HPP__
# define                        __HBS_4044_HPP__
# include                       "4043.hpp"

namespace                       hbs
{
  class                         CMP_4044 : public hbs::CMP_4043
  {
  protected:
    virtual bool                NandLatch(void) const { return (true); }

  private:
    const std::string           &GetType(void) const;

  public:
    static bool                 TypeMatches(const std::string &type);
    static hbs::IComponent      *Create(hbs::Timer             &timer,
                                        const std::string      &type,
                                        const std::string      &name,
                                        const std::string      &value,
                                        const std::string      &position);
    virtual std::string         GetDisplayType(void) const;

    CMP_4044(const hbs::Timer   &timer,
             const std::string  &name,
             const std::string  &pos);
    virtual ~CMP_4044(void);
  };
}

#endif
