#ifndef HBS_SAFE_FILE_HPP
# define HBS_SAFE_FILE_HPP
# include <string>
# include <cstddef>

namespace hbs
{
  // Read a bounded regular file; reject directories, devices and short reads.
  std::string ReadFile(const std::string &file, size_t limit);

  // Publish a fully written, synced sibling file, then sync its directory.
  // Exclusive creation never replaces a file created by another process.
  bool AtomicWriteFile(const std::string &file, const std::string &content,
                       bool exclusive = false);
}
#endif
