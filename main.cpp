// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		<fcntl.h>
#include		<signal.h>
#include		<stdlib.h>
#include		"Circuit.hpp"

static bool		getout;

void			handler(int		unused)
{
  (void)unused;
  getout = true;
  std::cerr << std::endl;
}

int			hbs::Command(const std::string	&cmd,
				     hbs::Circuit	&circuit,
				     hbs::Timer		&timer)
{
  int			l;
  size_t		i;

  if (cmd == "simulate")
    {
      timer.Tick();
      for (i = 1; i <= circuit.GetOutputNum(); ++i)
	circuit.Compute(i);
    }
  else if (cmd == "display")
    {
      for (i = 1; i <= circuit.GetOutputNum(); ++i)
	if (circuit.GetDisplayable(i))
	  std::cerr << circuit.GetOutputName(i) << "=" << circuit.Compute(i) << std::endl;
	else
	  circuit.Compute(i);
    }
  else if (cmd.compare(0, 3, "sdn") == 0)
    {
      for (l = atoi(&cmd.c_str()[3]); l > 0; --l)
	{
	  timer.Tick();
	  for (i = 1; i <= circuit.GetOutputNum(); ++i)
	    if (circuit.GetDisplayable(i))
	      std::cerr << circuit.GetOutputName(i) << "=" << circuit.Compute(i) << std::endl;
	    else
	      circuit.Compute(i);
	}
    }
  else if (cmd == "map")
    circuit.Map();
  else if (cmd == "dump")
    circuit.Dump();
  else if (cmd == "help")
    {
      std::cerr << "Commands are: "
		<< std::endl
		<< "\tsimulate: Start a single simulation operation of every components."
		<< std::endl
		<< "\tdisplay: Display all output values on stdout."
		<< std::endl
		<< "\tsdn N: Simulate and display N times."
		<< std::endl
		<< "\tmap: Display inputs of the circuit and output."
		<< std::endl
		<< "\tdump: Make a complete dump of the circuit component status."
		<< std::endl
		<< "\thelp: Display an explicative list of all commands."
		<< std::endl
		<< "\tloop: Loop until the program receive a SIGINT signal."
		<< std::endl
		<< "\texit: Exit the program."
		<< std::endl
		<< "\ta=b: Set b (0 or 1) to the input named a."
		<< std::endl
		<< std::endl;
    }
  else if (cmd == "loop")
    {
      getout = false;
      signal(SIGINT, handler);
      while (!getout)
	{
	  timer.Tick();
	  for (i = 1; i <= circuit.GetOutputNum(); ++i)
	    circuit.Compute(i);
	}
      signal(SIGINT, NULL);
    }
  else if ((i = cmd.find('=', 0)) != std::string::npos)
    circuit.SetValue(cmd.substr(0, i), atoi(&cmd.c_str()[i + 1]) ? hbs::TRUE : hbs::FALSE);
  else if (cmd != "")
    std::cerr << "Unrecognized command '" << cmd << "'" << std::endl;
  return (0);
}

static int		Shell(hbs::Circuit	&circuit,
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
	Command(str, circuit, timer);
    }
  while (str != "exit");
  return (EXIT_SUCCESS);
}

static int		Loop(hbs::Circuit	&circuit,
			     hbs::Timer		&timer,
			     hbs::Screen	&screen)
{
  screen.Loop(circuit, timer);
  return (0);
}

int			main(int		argc,
			     char		**argv)
{
  hbs::Timer		timer;
  hbs::Circuit		circuit(timer);
  hbs::Screen		*screen = NULL;
  int			i;
  int			j;

  if (argc < 2)
    {
      std::cerr << argv[0] << " circuit.hbs [input=value]*" << std::endl;
      return (EXIT_FAILURE);
    }
  try
    {
      if (circuit.Load(argv[1]) == false)
	return (EXIT_FAILURE);
    }
  catch (std::invalid_argument &e)
    {
      std::cerr << e.what() << std::endl;
      return (EXIT_FAILURE);
    }
  for (i = 2; i < argc; ++i)
    {
      if (strcmp(argv[i], "--screen") == 0)
	screen = new hbs::Screen;
      for (j = 0; argv[i][j] && argv[i][j] != '='; ++j);
      if (argv[i][j] != '=')
	throw hbs::InvalidCommandLine("Expected '=' after input name.");
      circuit.SetValue(std::string(argv[i]).substr(0, j), atoi(&argv[i][j + 1]) ? hbs::TRUE : hbs::FALSE);
    }
  for (i = 1; i <= (int)circuit.GetOutputNum(); ++i)
    if (circuit.GetDisplayable(i))
      std::cerr << circuit.GetOutputName(i) << "=" << circuit.Compute(i) << std::endl;
    else
      circuit.Compute(i);
  if (screen)
    return (Loop(circuit, timer, *screen));
  return (Shell(circuit, timer));
}
