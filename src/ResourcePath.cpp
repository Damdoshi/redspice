// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                <cstdlib>
#include                <unistd.h>
#include                "ResourcePath.hpp"

#ifndef                 BCAD_DEFAULT_DATADIR
# define                BCAD_DEFAULT_DATADIR "/usr/share/bcad"
#endif

namespace
{
  static bool           is_absolute_path(const std::string &path)
  {
    return (!path.empty() && path[0] == '/');
  }

  static bool           is_readable_file(const std::string &path)
  {
    return (!path.empty() && access(path.c_str(), R_OK) == 0);
  }

  static std::string    join_path(const std::string &dir,
                                  const std::string &file)
  {
    if (dir.empty() || file.empty() || is_absolute_path(file))
      return (file);
    if (dir[dir.size() - 1] == '/')
      return (dir + file);
    return (dir + "/" + file);
  }

  static std::string    env_data_directory(void)
  {
    const char          *env;

    env = std::getenv("BCAD_DATADIR");
    if (env == NULL)
      return ("");
    return (env);
  }

  static std::string    default_data_directory(void)
  {
    return (BCAD_DEFAULT_DATADIR);
  }
}

std::string             hbs::BcadDataDirectory(void)
{
  std::string           dir;

  dir = env_data_directory();
  if (!dir.empty())
    return (dir);
  return (default_data_directory());
}

std::string             hbs::BcadResourcePath(const std::string &file)
{
  std::string           dir;
  std::string           candidate;

  if (file.empty() || is_absolute_path(file))
    return (file);
  dir = env_data_directory();
  if (!dir.empty())
    {
      candidate = join_path(dir, file);
      if (is_readable_file(candidate))
        return (candidate);
    }
  dir = default_data_directory();
  if (!dir.empty())
    {
      candidate = join_path(dir, file);
      if (is_readable_file(candidate))
        return (candidate);
    }
  return (file);
}

std::string             hbs::BcadResolveReadablePath(const std::string &file)
{
  std::string           dir;
  std::string           candidate;

  if (file.empty() || is_absolute_path(file) || is_readable_file(file))
    return (file);
  dir = env_data_directory();
  if (!dir.empty())
    {
      candidate = join_path(dir, file);
      if (is_readable_file(candidate))
        return (candidate);
    }
  dir = default_data_directory();
  if (!dir.empty())
    {
      candidate = join_path(dir, file);
      if (is_readable_file(candidate))
        return (candidate);
    }
  return (file);
}
