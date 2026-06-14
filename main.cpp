// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		<cerrno>
#include		<cstdlib>
#include		<cstring>
#include		<ctime>
#include		<fstream>
#include		<iostream>
#include		<string>
#include		<sys/stat.h>
#include		"Circuit.hpp"
#include		"Screen.hpp"
#include		"Shell.hpp"

static void		usage(const char	*prog)
{
  std::cerr << prog << " [circuit.rs] [--simulate] [--screen] [input=value]*" << std::endl;
  std::cerr << prog << " --batch circuit.bc [input=value]* < scenario.bcadtest" << std::endl;
  std::cerr << prog << " --test scenario.bcadtest circuit.bc [input=value]*" << std::endl;
}

static void		set_input_argument(hbs::Circuit	&circuit,
					   const char	*arg)
{
  int			j;

  for (j = 0; arg[j] && arg[j] != '='; ++j);
  if (arg[j] != '=')
    throw hbs::InvalidCommandLine("Expected '=' after input name.");
  circuit.SetValue(std::string(arg).substr(0, j), atoi(&arg[j + 1]) ? hbs::TRUE : hbs::FALSE);
}

static int		load_circuit(hbs::Circuit	&circuit,
				     const std::string	&file,
				     bool		create_if_missing = false)
{
  try
    {
      circuit.Load(file, create_if_missing);
    }
  catch (const std::exception &e)
    {
      std::cerr << e.what() << std::endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

static int		parse_input_arguments(hbs::Circuit	&circuit,
					    int			argc,
					    char		**argv,
					    int			start)
{
  try
    {
      for (int i = start; i < argc; ++i)
	set_input_argument(circuit, argv[i]);
    }
  catch (const std::exception &e)
    {
      std::cerr << e.what() << std::endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

static int		Batch(int		argc,
			      char		**argv)
{
  hbs::Timer		timer;
  hbs::Circuit		circuit(timer);

  if (argc < 3)
    return (usage(argv[0]), EXIT_FAILURE);
  if (load_circuit(circuit, argv[2]) != EXIT_SUCCESS)
    return (EXIT_FAILURE);
  if (parse_input_arguments(circuit, argc, argv, 3) != EXIT_SUCCESS)
    return (EXIT_FAILURE);
  return (hbs::ExecuteScript(circuit, timer, std::cin, std::cout, std::cerr));
}

static int		Test(int		argc,
			     char		**argv)
{
  hbs::Timer		timer;
  hbs::Circuit		circuit(timer);
  std::ifstream		script;

  if (argc < 4)
    return (usage(argv[0]), EXIT_FAILURE);
  if (load_circuit(circuit, argv[3]) != EXIT_SUCCESS)
    return (EXIT_FAILURE);
  if (parse_input_arguments(circuit, argc, argv, 4) != EXIT_SUCCESS)
    return (EXIT_FAILURE);
  script.open(argv[2], std::ios::in | std::ios::binary);
  if (!script)
    {
      std::cerr << "Cannot open test script '" << argv[2] << "'." << std::endl;
      return (EXIT_FAILURE);
    }
  return (hbs::ExecuteScript(circuit, timer, script, std::cout, std::cerr));
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
  bool			simulate_only = false;
  std::string		initial_file = "";
  int			arg_start = 1;
  int			i;

  srand(clock());
  if (argc >= 2 && strcmp(argv[1], "--batch") == 0)
    return (Batch(argc, argv));
  if (argc >= 2 && strcmp(argv[1], "--test") == 0)
    return (Test(argc, argv));
  if (argc >= 2 && argv[1][0] != '-')
    {
      initial_file = argv[1];
      arg_start = 2;
      if (load_circuit(circuit, initial_file, true) != EXIT_SUCCESS)
	return (EXIT_FAILURE);
    }
  try
    {
      for (i = arg_start; i < argc; ++i)
	{
	  if (strcmp(argv[i], "--screen") == 0)
	    continue ;
	  if (strcmp(argv[i], "--simulate") == 0)
	    {
	      simulate_only = true;
	      continue ;
	    }
	  if (initial_file.empty())
	    throw hbs::InvalidCommandLine("Input=value arguments require an opened circuit file.");
	  set_input_argument(circuit, argv[i]);
	}
    }
  catch (const std::exception &e)
    {
      std::cerr << e.what() << std::endl;
      return (EXIT_FAILURE);
    }
  if (!simulate_only)
    {
      screen = new hbs::Screen(initial_file);
      int ret = Loop(circuit, timer, *screen);
      delete screen;
      return (ret);
    }
  if (initial_file.empty())
    return (usage(argv[0]), EXIT_FAILURE);
  for (i = 1; i <= (int)circuit.GetOutputNum(); ++i)
    if (circuit.GetDisplayable(i))
      std::cerr << circuit.GetOutputName(i) << "=" << circuit.Compute(i) << std::endl;
    else
      circuit.Compute(i);
  return (hbs::Shell(circuit, timer));
}
