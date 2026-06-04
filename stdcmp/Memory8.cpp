// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                <fstream>
#include                "Exception.hpp"
#include                "Memory8.hpp"

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
  ss.open(file.c_str(), std::ios::in | std::ios::binary);
  if (!ss)
    throw hbs::CannotOpenFile(file);
  ss.seekg(0, std::ios::end);
  if (ss.tellg() > (std::streampos)memory.size())
    throw hbs::FileIsTooBig(file);
  content.resize(ss.tellg());
  ss.seekg(0, std::ios::beg);
  ss.read(&content[0], content.size());
  for (i = 0; i < content.size(); ++i)
    memory[i] = (uint8_t)content[i];
  return (true);
}
