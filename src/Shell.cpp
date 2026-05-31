// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
// EFRITS SAS 2026
//
// RED Spice

#include		<algorithm>
#include		<cctype>
#include		<cstdlib>
#include		<csignal>
#include		<iostream>
#include		<sstream>
#include		<string>
#include		<unistd.h>
#include		"Shell.hpp"

static bool		getout;

static void		handler(int		unused)
{
  (void)unused;
  getout = true;
  std::cerr << std::endl;
}

static std::string	trim(const std::string	&str)
{
  size_t		begin = 0;
  size_t		end = str.size();

  while (begin < end && std::isspace((unsigned char)str[begin]))
    begin += 1;
  while (end > begin && std::isspace((unsigned char)str[end - 1]))
    end -= 1;
  return (str.substr(begin, end - begin));
}

static std::string	line_prefix(size_t	line)
{
  std::stringstream	ss;

  if (line != 0)
    ss << "line " << line << ": ";
  return (ss.str());
}

static std::string	tristate_to_string(hbs::Tristate	value)
{
  std::stringstream	ss;

  ss << value;
  return (ss.str());
}

static bool		parse_tristate(const std::string	&str,
				       hbs::Tristate		&value)
{
  std::string		lower = str;

  std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c){ return (std::tolower(c)); });
  if (lower == "1" || lower == "true" || lower == "t" || lower == "high")
    return (value = hbs::TRUE, true);
  if (lower == "0" || lower == "false" || lower == "f" || lower == "low")
    return (value = hbs::FALSE, true);
  if (lower == "u" || lower == "x" || lower == "undefined" || lower == "unknown")
    return (value = hbs::UNDEFINED, true);
  if (lower == "b" || lower == "broken")
    return (value = hbs::BROKEN, true);
  return (false);
}

static void		compute_outputs(hbs::Circuit	&circuit)
{
  for (size_t i = 1; i <= circuit.GetOutputNum(); ++i)
    circuit.Compute(i);
}

static void		display_outputs(hbs::Circuit	&circuit,
				       std::ostream	&out)
{
  for (size_t i = 1; i <= circuit.GetOutputNum(); ++i)
    if (circuit.GetDisplayable(i))
      out << circuit.GetOutputName(i) << "=" << circuit.Compute(i) << std::endl;
    else
      circuit.Compute(i);
}

static hbs::CommandResult ok(const std::string	&message)
{
  return (hbs::CommandResult(hbs::COMMAND_OK, message));
}

static hbs::CommandResult fail(const std::string	&message)
{
  return (hbs::CommandResult(hbs::COMMAND_FAIL, message));
}

static hbs::CommandResult error(const std::string	&message)
{
  return (hbs::CommandResult(hbs::COMMAND_ERROR, message));
}

static hbs::CommandResult execute_tick(const std::string	&verb,
					       std::istringstream	&iss,
					       hbs::Circuit	&circuit,
					       hbs::Timer	&timer)
{
  int			count = 1;

  if (verb == "tick")
    {
      if (!(iss >> count))
	count = 1;
      if (count < 0)
	return (error("tick count cannot be negative"));
    }
  else if (verb == "simulate")
    {
      if (iss >> count && count < 0)
	return (error("simulate count cannot be negative"));
    }
  for (int i = 0; i < count; ++i)
    {
      timer.Tick();
      compute_outputs(circuit);
    }
  return (ok(verb));
}

static hbs::CommandResult execute_expect_one(hbs::Circuit	&circuit,
					      const std::string	&name,
					      const std::string	&expected)
{
  hbs::Tristate		exp;
  hbs::Tristate		got;

  if (!parse_tristate(expected, exp))
    return (error("bad tristate value '" + expected + "'"));
  if (!circuit.HasOutput(name))
    return (fail("missing output '" + name + "'"));
  got = circuit.GetOutputValue(name);
  if (got != exp)
    return (fail("expect " + name + "=" + tristate_to_string(exp) + " got " + tristate_to_string(got)));
  return (ok("expect " + name + "=" + tristate_to_string(exp)));
}

static hbs::CommandResult execute_expect(std::istringstream	&iss,
					 hbs::Circuit		&circuit)
{
  std::string		token;
  std::string		value;
  bool			seen = false;

  while (iss >> token)
    {
      size_t eq = token.find('=');
      seen = true;
      if (eq != std::string::npos)
	{
	  hbs::CommandResult ret = execute_expect_one(circuit, token.substr(0, eq), token.substr(eq + 1));
	  if (ret.status != hbs::COMMAND_OK)
	    return (ret);
	}
      else
	{
	  if (!(iss >> value))
	    return (error("expected value after output '" + token + "'"));
	  hbs::CommandResult ret = execute_expect_one(circuit, token, value);
	  if (ret.status != hbs::COMMAND_OK)
	    return (ret);
	}
    }
  if (!seen)
    return (error("expect requires at least one output"));
  return (ok("expect"));
}

static hbs::CommandResult execute_set(std::istringstream	&iss,
				      hbs::Circuit	&circuit)
{
  std::string		name;
  std::string		value;
  hbs::Tristate		tristate;

  if (!(iss >> name >> value))
    return (error("set requires an input and a value"));
  if (!parse_tristate(value, tristate))
    return (error("bad tristate value '" + value + "'"));
  if (!circuit.HasInput(name))
    return (fail("missing input '" + name + "'"));
  circuit.SetValue(name, tristate);
  return (ok("set " + name + "=" + tristate_to_string(tristate)));
}

static hbs::CommandResult execute_assignment(const std::string	&cmd,
					     hbs::Circuit	&circuit)
{
  size_t		eq = cmd.find('=');
  std::string		name;
  std::string		value;
  hbs::Tristate		tristate;

  if (eq == std::string::npos)
    return (error("internal assignment error"));
  name = trim(cmd.substr(0, eq));
  value = trim(cmd.substr(eq + 1));
  if (!parse_tristate(value, tristate))
    return (error("bad tristate value '" + value + "'"));
  if (!circuit.HasInput(name))
    return (fail("missing input '" + name + "'"));
  circuit.SetValue(name, tristate);
  return (ok("set " + name + "=" + tristate_to_string(tristate)));
}

hbs::CommandResult	hbs::ExecuteCommand(const std::string	&raw_cmd,
					    hbs::Circuit	&circuit,
					    hbs::Timer		&timer,
					    std::ostream	&out,
					    std::ostream	&err,
					    bool		batch,
					    size_t		line)
{
  std::string		cmd = trim(raw_cmd);
  std::istringstream	iss(cmd);
  std::string		verb;

  if (cmd.empty())
    return (ok(""));
  if (cmd[0] == '#')
    return (ok(""));
  if (cmd == "exit")
    return (hbs::CommandResult(hbs::COMMAND_EXIT, "exit"));
  if (cmd.find('=') != std::string::npos && cmd.compare(0, 6, "expect") != 0)
    return (execute_assignment(cmd, circuit));
  iss >> verb;
  if (verb == "simulate" || verb == "tick")
    return (execute_tick(verb, iss, circuit, timer));
  if (verb == "display")
    return (display_outputs(circuit, batch ? out : err), ok("display"));
  if (verb == "sdn")
    {
      int count;

      if (!(iss >> count) || count < 0)
	return (error("sdn requires a non-negative count"));
      for (int i = 0; i < count; ++i)
	{
	  timer.Tick();
	  display_outputs(circuit, batch ? out : err);
	}
      return (ok("sdn"));
    }
  if (verb == "map")
    return (circuit.Map(), ok("map"));
  if (verb == "dump")
    return (circuit.Dump(), ok("dump"));
  if (verb == "help")
    {
      err << "Commands are: " << std::endl
	  << "\tsimulate [N]: Start one or N simulation operations of every components." << std::endl
	  << "\ttick [N]: Alias for simulate, clearer in batch tests." << std::endl
	  << "\tdisplay: Display all output values." << std::endl
	  << "\tsdn N: Simulate and display N times." << std::endl
	  << "\tmap: Display inputs of the circuit and output." << std::endl
	  << "\tdump: Make a complete dump of the circuit component status." << std::endl
	  << "\trequire-input NAME: Fail if input NAME is missing." << std::endl
	  << "\trequire-output NAME: Fail if output NAME is missing." << std::endl
	  << "\trequire-component TYPE: Fail if component TYPE is missing." << std::endl
	  << "\tforbid-component TYPE: Fail if component TYPE is present." << std::endl
	  << "\tmax-components N: Fail if there are more than N non-track components." << std::endl
	  << "\tmax-component TYPE N: Fail if there are more than N components of TYPE." << std::endl
	  << "\tset INPUT VALUE: Set VALUE (0, 1, U or B) to INPUT." << std::endl
	  << "\texpect OUT VALUE: Fail if OUT does not have VALUE." << std::endl
	  << "\texpect OUT=VALUE [OUT=VALUE...]: Compact expect form." << std::endl
	  << "\tloop: Loop until the program receives a SIGINT signal." << std::endl
	  << "\texit: Exit the program." << std::endl
	  << "\ta=b: Set b to the input named a." << std::endl << std::endl;
      return (ok("help"));
    }
  if (verb == "loop")
    {
      if (batch)
	return (error("loop is not available in batch mode"));
      getout = false;
      signal(SIGINT, handler);
      while (!getout)
	{
	  timer.Tick();
	  compute_outputs(circuit);
	}
      signal(SIGINT, NULL);
      return (ok("loop"));
    }
  if (verb == "require-input")
    {
      std::string name;
      if (!(iss >> name))
	return (error("require-input requires a name"));
      if (!circuit.HasInput(name))
	return (fail("missing input '" + name + "'"));
      return (ok("require-input " + name));
    }
  if (verb == "require-output")
    {
      std::string name;
      if (!(iss >> name))
	return (error("require-output requires a name"));
      if (!circuit.HasOutput(name))
	return (fail("missing output '" + name + "'"));
      return (ok("require-output " + name));
    }
  if (verb == "require-component")
    {
      std::string name;
      if (!(iss >> name))
	return (error("require-component requires a type or name"));
      if (!circuit.HasComponentType(name) && !circuit.HasComponent(name))
	return (fail("missing component '" + name + "'"));
      return (ok("require-component " + name));
    }
  if (verb == "forbid-component")
    {
      std::string type;
      if (!(iss >> type))
	return (error("forbid-component requires a type"));
      if (circuit.HasComponentType(type))
	return (fail("forbidden component type '" + type + "'"));
      return (ok("forbid-component " + type));
    }
  if (verb == "max-components")
    {
      size_t max;
      if (!(iss >> max))
	return (error("max-components requires a count"));
      if (circuit.GetComponentCount(false) > max)
	{
	  std::stringstream ss;
	  ss << "too many components: " << circuit.GetComponentCount(false) << " > " << max;
	  return (fail(ss.str()));
	}
      return (ok("max-components"));
    }
  if (verb == "max-component")
    {
      std::string type;
      size_t max;
      std::map<std::string, size_t> counts;

      if (!(iss >> type >> max))
	return (error("max-component requires a type and a count"));
      counts = circuit.GetComponentTypeCount();
      if (counts[type] > max)
	{
	  std::stringstream ss;
	  ss << "too many components of type '" << type << "': " << counts[type] << " > " << max;
	  return (fail(ss.str()));
	}
      return (ok("max-component " + type));
    }
  if (verb == "expect")
    return (execute_expect(iss, circuit));
  if (verb == "set")
    return (execute_set(iss, circuit));
  return (error("unrecognized command '" + cmd + "'" + (line != 0 ? "" : "")));
}

int			hbs::ExecuteScript(hbs::Circuit	&circuit,
					   hbs::Timer		&timer,
					   std::istream		&input,
					   std::ostream		&out,
					   std::ostream		&err)
{
  std::string		line;
  size_t		lineno = 0;

  while (std::getline(input, line))
    {
      hbs::CommandResult result;
      std::string trimmed;

      lineno += 1;
      if (line.find('#') != std::string::npos)
	line.resize(line.find('#'));
      trimmed = trim(line);
      if (trimmed.empty() || trimmed[0] == '#')
	continue ;
      try
	{
	  result = hbs::ExecuteCommand(trimmed, circuit, timer, out, err, true, lineno);
	}
      catch (const std::exception &e)
	{
	  out << "ERROR " << line_prefix(lineno) << e.what() << std::endl;
	  return (2);
	}
      if (result.status == hbs::COMMAND_EXIT)
	return (0);
      if (result.status == hbs::COMMAND_OK)
	out << "OK " << line_prefix(lineno) << result.message << std::endl;
      else if (result.status == hbs::COMMAND_FAIL)
	{
	  out << "FAIL " << line_prefix(lineno) << result.message << std::endl;
	  return (1);
	}
      else
	{
	  out << "ERROR " << line_prefix(lineno) << result.message << std::endl;
	  return (2);
	}
    }
  return (0);
}

int			hbs::Shell(hbs::Circuit	&circuit,
				   hbs::Timer	&timer)
{
  char			buffer[4096];
  std::string		str;
  int			i;

  do
    {
      std::cerr << "> " << std::flush;
      if ((i = read(0, &buffer[0], sizeof(buffer) - 1)) == 0)
	{
	  std::cerr << std::endl;
	  return (EXIT_SUCCESS);
	}
      if (i > 1 && buffer[i - 1])
	buffer[i - 1] = '\0';
      buffer[i] = '\0';
      str = buffer;
      if (str != "exit")
	{
	  hbs::CommandResult result = hbs::ExecuteCommand(str, circuit, timer, std::cout, std::cerr, false, 0);
	  if (result.status == hbs::COMMAND_FAIL || result.status == hbs::COMMAND_ERROR)
	    std::cerr << result.message << std::endl;
	}
    }
  while (str != "exit");
  return (EXIT_SUCCESS);
}

int			hbs::Command(const std::string	&cmd,
				     hbs::Circuit	&circuit,
				     hbs::Timer	&timer)
{
  hbs::CommandResult	result = hbs::ExecuteCommand(cmd, circuit, timer, std::cout, std::cerr, false, 0);

  if (result.status == hbs::COMMAND_FAIL || result.status == hbs::COMMAND_ERROR)
    std::cerr << result.message << std::endl;
  return (result.status == hbs::COMMAND_OK || result.status == hbs::COMMAND_EXIT ? 0 : 1);
}
