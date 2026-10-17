#include <cerrno>
#include <filesystem>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include "SafeFile.hpp"
#include "Exception.hpp"

namespace
{
  class FileDescriptor
  {
  public:
    int fd;
    explicit FileDescriptor(int value) : fd(value) {}
    ~FileDescriptor() { if (fd >= 0) close(fd); }
    bool Close()
    {
      int value = fd;
      fd = -1; // Never retry close(): the descriptor may already be reused.
      return (close(value) == 0);
    }
  private:
    FileDescriptor(const FileDescriptor &) = delete;
    FileDescriptor &operator=(const FileDescriptor &) = delete;
  };

  bool sync_file(int fd)
  {
    int result;
    do result = fsync(fd); while (result < 0 && errno == EINTR);
    return (result == 0);
  }
}

std::string hbs::ReadFile(const std::string &file, size_t limit)
{
  FileDescriptor input(open(file.c_str(), O_RDONLY | O_CLOEXEC | O_NONBLOCK));
  struct stat info;
  if (input.fd < 0 || fstat(input.fd, &info) < 0 || !S_ISREG(info.st_mode) ||
      info.st_size < 0)
    throw hbs::CannotOpenFile("Cannot read regular file '" + file + "'.");
  if ((uintmax_t)info.st_size > limit)
    throw hbs::FileIsTooBig(file);
  std::string content((size_t)info.st_size, '\0');
  size_t offset = 0;
  while (offset < content.size())
    {
      ssize_t count = read(input.fd, &content[offset], content.size() - offset);
      if (count < 0 && errno == EINTR)
        continue;
      if (count <= 0)
        throw hbs::CannotOpenFile("Incomplete read of " + file);
      offset += (size_t)count;
    }
  char extra;
  ssize_t count;
  do count = read(input.fd, &extra, 1); while (count < 0 && errno == EINTR);
  if (count != 0)
    throw hbs::CannotOpenFile("File changed while reading " + file);
  return (content);
}

bool hbs::AtomicWriteFile(const std::string &file, const std::string &content,
                          bool exclusive)
{
  namespace fs = std::filesystem;
  if (file.empty())
    return (false);
  fs::path target(file);
  struct stat info;
  bool exists = (lstat(file.c_str(), &info) == 0);
  if (!exists && errno != ENOENT)
    return (false);
  if (exists && exclusive)
    return (false);
  // Preserve the meaning of saving through an existing symbolic link.
  if (exists && S_ISLNK(info.st_mode))
    {
      std::error_code ec;
      target = fs::canonical(target, ec);
      if (ec || stat(target.c_str(), &info) < 0)
        return (false);
    }
  if (exists && !S_ISREG(info.st_mode))
    return (false);
  if (exists && access(target.c_str(), W_OK) != 0)
    return (false);
  fs::path parent = target.parent_path();
  if (parent.empty())
    parent = ".";
  FileDescriptor directory(open(parent.c_str(), O_RDONLY | O_DIRECTORY | O_CLOEXEC));
  if (directory.fd < 0)
    return (false);
  std::string pattern = (parent / ("." + target.filename().string() + ".tmp.XXXXXX")).string();
  std::vector<char> temporary(pattern.begin(), pattern.end());
  temporary.push_back('\0');
  FileDescriptor output(mkstemp(temporary.data()));
  if (output.fd < 0)
    return (false);
  bool good = (fcntl(output.fd, F_SETFD, FD_CLOEXEC) == 0);
  if (good && exists)
    good = (fchmod(output.fd, info.st_mode & 0777) == 0);
  size_t offset = 0;
  while (good && offset < content.size())
    {
      ssize_t count = write(output.fd, content.data() + offset, content.size() - offset);
      if (count < 0 && errno == EINTR)
        continue;
      if (count <= 0)
        good = false;
      else
        offset += (size_t)count;
    }
  if (good)
    good = sync_file(output.fd);
  if (!output.Close())
    good = false;
  if (good)
    {
      if (exclusive)
        good = (link(temporary.data(), target.c_str()) == 0);
      else
        good = (rename(temporary.data(), target.c_str()) == 0);
    }
  unlink(temporary.data());
  if (good)
    good = sync_file(directory.fd);
  return (good);
}
