// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_TIMER_HPP__
# define			__NTS_TIMER_HPP__
# include			<sys/types.h>

namespace			hbs
{
  class				Timer
  {
  protected:
    size_t			cnt;

  public:
    size_t			GetTime(void) const;
    void			Tick(void);

    Timer(void);
    virtual ~Timer(void);
  };
}

#endif	//			__NTS_TIMER_HPP__
