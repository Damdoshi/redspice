// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                 __RESOURCE_PATH_HPP__
# define                __RESOURCE_PATH_HPP__
# include               <string>

namespace               hbs
{
  std::string           BcadDataDirectory(void);
  std::string           BcadResourcePath(const std::string &file);
  std::string           BcadResolveReadablePath(const std::string &file);
}

#endif
