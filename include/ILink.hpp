// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Pentacle Technologie 2008-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#ifndef					__ILINK_HPP__
# define				__ILINK_HPP__
# include				<sys/types.h>

namespace				hbs
{
  class					IComponent;
  struct				ILink
  {
    enum				Layer
      {
	TOP,
	BOTTOM
      };

    virtual const IComponent		*GetTarget(void) const = 0;
    virtual size_t			GetTargetPin(void) const = 0;
    virtual size_t			GetNbrSteps(void) const = 0;
    virtual std::pair<int, int>		GetStep(size_t i) const = 0;
    virtual ~ILink(void) {}
  };

}

#endif	//				__ILINK_HPP__
