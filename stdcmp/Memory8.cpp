// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                <fstream>
#include                "Exception.hpp"
#include                "Memory8.hpp"
#include                "ResourcePath.hpp"
#include                "SafeFile.hpp"

bool                    hbs::Memory8Utility::LoadFile(const std::string &file,
                                                       std::vector<uint8_t> &memory,
                                                       uint8_t fill)
{
  std::ifstream         ss;
  std::string           content;
  size_t                i;

  for (i = 0; i < memory.size(); ++i)
    memory[i] = fill;
  if (file.empty())
    return (false);
  std::string           path = hbs::BcadResolveReadablePath(file);

  content = hbs::ReadFile(path, memory.size());
  for (i = 0; i < content.size(); ++i)
    memory[i] = (uint8_t)content[i];
  return (true);
}
