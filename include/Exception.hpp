// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__EXCEPTION_HPP__
# define			__EXCEPTION_HPP__
# include			<stdexcept>

namespace			hbs
{
  class				BadPin : public std::runtime_error
  {
  public:
    BadPin(const std::string	&msg)
      : std::runtime_error(msg)
    {}
    BadPin(const char		*msg)
      : std::runtime_error(msg)
    {}
    virtual ~BadPin(void) throw()
    {}
  };
  class				BadComponent : public std::runtime_error
  {
  public:
    BadComponent(const std::string	&msg)
      : std::runtime_error(msg)
    {}
    BadComponent(const char		*msg)
      : std::runtime_error(msg)
    {}
    virtual ~BadComponent(void) throw()
    {}
  };
  class				SyntaxError : public std::runtime_error
  {
  public:
    SyntaxError(const std::string	&msg)
      : std::runtime_error(msg)
    {}
    SyntaxError(const char		*msg)
      : std::runtime_error(msg)
    {}
    virtual ~SyntaxError(void) throw()
    {}
  };
  class				MissingInputs : public std::runtime_error
  {
  public:
    MissingInputs(const std::string	&msg)
      : std::runtime_error(msg)
    {}
    MissingInputs(const char		*msg)
      : std::runtime_error(msg)
    {}
    virtual ~MissingInputs(void) throw()
    {}
  };
  class				InvalidCommandLine : public std::runtime_error
  {
  public:
    InvalidCommandLine(const std::string	&msg)
      : std::runtime_error(msg)
    {}
    InvalidCommandLine(const char		*msg)
      : std::runtime_error(msg)
    {}
    virtual ~InvalidCommandLine(void) throw()
    {}
  };
  class				MissingOutputs : public std::runtime_error
  {
  public:
    MissingOutputs(const std::string	&msg)
      : std::runtime_error(msg)
    {}
    MissingOutputs(const char		*msg)
      : std::runtime_error(msg)
    {}
    virtual ~MissingOutputs(void) throw()
    {}
  };
  class				UnknownComponent : public std::runtime_error
  {
  public:
    UnknownComponent(const std::string	&msg)
      : std::runtime_error(msg)
    {}
    UnknownComponent(const char		*msg)
      : std::runtime_error(msg)
    {}
    virtual ~UnknownComponent(void) throw()
    {}
  };
  class				FileIsTooBig : public std::runtime_error
  {
  public:
    FileIsTooBig(const std::string	&msg)
      : std::runtime_error(msg)
    {}
    FileIsTooBig(const char		*msg)
      : std::runtime_error(msg)
    {}
    virtual ~FileIsTooBig(void) throw()
    {}
  };
  class				CannotOpenFile : public std::runtime_error
  {
  public:
    CannotOpenFile(const std::string	&msg)
      : std::runtime_error(msg)
    {}
    CannotOpenFile(const char		*msg)
      : std::runtime_error(msg)
    {}
    virtual ~CannotOpenFile(void) throw()
    {}
  };
  class				MemoryOverflow : public std::runtime_error
  {
  public:
    MemoryOverflow(const std::string	&msg)
      : std::runtime_error(msg)
    {}
    MemoryOverflow(const char		*msg)
      : std::runtime_error(msg)
    {}
    virtual ~MemoryOverflow(void) throw()
    {}
  };
}

#endif	//			__EXCEPTION_HPP__
