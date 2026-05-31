// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
// EFRITS SAS 2026
//
// RED Spice

#ifndef				__HBS_SHELL_HPP__
# define				__HBS_SHELL_HPP__
# include				<iosfwd>
# include				<string>
# include				"Circuit.hpp"

namespace				hbs
{
  enum					CommandStatus
    {
      COMMAND_OK,
      COMMAND_FAIL,
      COMMAND_ERROR,
      COMMAND_EXIT
    };

  struct				CommandResult
  {
    hbs::CommandStatus			status;
    std::string				message;

    CommandResult(hbs::CommandStatus status = hbs::COMMAND_OK,
		  const std::string &message = "") : status(status), message(message) {}
  };

  hbs::CommandResult			ExecuteCommand(const std::string	&cmd,
					       hbs::Circuit		&circuit,
					       hbs::Timer		&timer,
					       std::ostream		&out,
					       std::ostream		&err,
					       bool			batch = false,
					       size_t			line = 0);
  int					ExecuteScript(hbs::Circuit	&circuit,
						      hbs::Timer	&timer,
						      std::istream	&input,
						      std::ostream	&out,
						      std::ostream	&err);
  int					Shell(hbs::Circuit	&circuit,
					      hbs::Timer	&timer);
  int					Command(const std::string	&cmd,
						hbs::Circuit		&circuit,
						hbs::Timer		&timer);
}

#endif	//				__HBS_SHELL_HPP__
