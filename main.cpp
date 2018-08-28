// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		<signal.h>
#include		<stdlib.h>
#include		"Circuit.hpp"

static bool		getout;

void			handler(int		unused)
{
  (void)unused;
  getout = true;
  std::cout << std::endl;
}

static int		Shell(hbs::Circuit	&circuit,
			      hbs::Timer	&timer)
{
  std::string		str;
  int			l;
  size_t		i;

  do
    {
      std::cout << "> " << std::flush;
      if (!getline(std::cin, str))
	{
	  std::cout << std::endl;
	  return (EXIT_SUCCESS);
	}
      else if (str == "simulate")
	{
	  timer.Tick();
	  for (i = 1; i <= circuit.GetOutputNum(); ++i)
	    circuit.Compute(i);
	}
      else if (str == "display")
	for (i = 1; i <= circuit.GetOutputNum(); ++i)
	  if (circuit.GetDisplayable(i))
	    std::cout << circuit.GetOutputName(i) << "=" << circuit.Compute(i) << std::endl;
	  else
	    circuit.Compute(i);
      else if (str.compare(0, 3, "sdn") == 0)
	for (l = atoi(&str.c_str()[3]); l > 0; --l)
	  {
	    timer.Tick();
	    for (i = 1; i <= circuit.GetOutputNum(); ++i)
	      if (circuit.GetDisplayable(i))
		std::cout << circuit.GetOutputName(i) << "=" << circuit.Compute(i) << std::endl;
	      else
		circuit.Compute(i);
	  }
      else if (str == "dump")
	circuit.Dump();
      else if (str == "loop")
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
      else if ((i = str.find('=', 0)) != std::string::npos)
	circuit.SetValue(str.substr(0, i), atoi(&str.c_str()[i + 1]) ? hbs::TRUE : hbs::FALSE);
      else if (str != "exit")
	std::cout << "Unrecognized command '" << str << "'" << std::endl;
    }
  while (str != "exit");
  return (EXIT_SUCCESS);
}

int			main(int		argc,
			     char		**argv)
{
  hbs::Timer		timer;
  hbs::Circuit		circuit(timer);
  int			i;
  int			j;

  if (argc < 2)
    {
      std::cout << argv[0] << " circuit.hbs [input=value]*" << std::endl;
      return (EXIT_FAILURE);
    }
  if (circuit.Load(argv[1]) == false)
    return (EXIT_FAILURE);
  for (i = 2; i < argc; ++i)
    {
      for (j = 0; argv[i][j] && argv[i][j] != '='; ++j);
      if (argv[i][j] != '=')
	throw hbs::InvalidCommandLine("Expected '=' after input name.");
      circuit.SetValue(std::string(argv[i]).substr(0, j), atoi(&argv[i][j + 1]) ? hbs::TRUE : hbs::FALSE);
    }
  for (i = 1; i <= (int)circuit.GetOutputNum(); ++i)
    if (circuit.GetDisplayable(i))
      std::cout << circuit.GetOutputName(i) << "=" << circuit.Compute(i) << std::endl;
    else
      circuit.Compute(i);
  return (Shell(circuit, timer));
}


