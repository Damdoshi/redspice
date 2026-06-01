// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#include		<algorithm>
#include		<cstdlib>
#include		<cstring>
#include		<dirent.h>
#include		<dlfcn.h>
#include		<iostream>
#include		<map>
#include		<set>
#include		<sstream>
#include		<string>
#include		<vector>
#include		"Exception.hpp"
#include		"Circuit.hpp"
#include		"Clock.hpp"
#include		"True.hpp"
#include		"False.hpp"
#include		"4000.hpp"
#include		"4002.hpp"
#include		"4006.hpp"
#include		"4009.hpp"
#include		"4010.hpp"
#include		"4012.hpp"
#include		"4023.hpp"
#include		"4025.hpp"
#include		"4027.hpp"
#include		"4020.hpp"
#include		"4021.hpp"
#include		"4022.hpp"
#include		"4024.hpp"
#include		"4026.hpp"
#include		"4028.hpp"
#include		"4029.hpp"
#include		"4033.hpp"
#include		"4035.hpp"
#include		"4042.hpp"
#include		"4043.hpp"
#include		"4044.hpp"
#include		"4063.hpp"
#include		"4076.hpp"
#include		"4099.hpp"
#include		"4510.hpp"
#include		"4511.hpp"
#include		"4515.hpp"
#include		"4516.hpp"
#include		"4518.hpp"
#include		"4520.hpp"
#include		"4555.hpp"
#include		"4556.hpp"
#include		"74164.hpp"
#include		"74165.hpp"
#include		"74194.hpp"
#include		"74595.hpp"
#include		"4041.hpp"
#include		"4049.hpp"
#include		"4050.hpp"
#include		"4068.hpp"
#include		"4072.hpp"
#include		"4073.hpp"
#include		"4075.hpp"
#include		"4077.hpp"
#include		"4078.hpp"
#include		"4082.hpp"
#include		"4085.hpp"
#include		"4086.hpp"
#include		"4093.hpp"
#include		"4584.hpp"
#include		"4001.hpp"
#include		"4008.hpp"
#include		"4011.hpp"
#include		"4013.hpp"
#include		"4014.hpp"
#include		"4015.hpp"
#include		"4017.hpp"
#include		"4030.hpp"
#include		"4040.hpp"
#include		"4069.hpp"
#include		"4071.hpp"
#include		"4081.hpp"
#include		"4094.hpp"
#include		"4502.hpp"
#include		"4503.hpp"
#include		"4514.hpp"

#include		"2716.hpp"

#include		"Terminal.hpp"
#include		"Track.hpp"

namespace
{
  typedef const char	**(*BcadListComponents)(void);
  typedef hbs::IComponent *(*BcadCreateComponent)(hbs::Timer			&timer,
							 const std::string	&type,
							 const std::string	&name,
							 const std::string	&value,
							 const std::string	&position);

  struct		DynamicComponent
  {
    void		*handle;
    BcadCreateComponent create;
    std::string		path;
  };

  static bool				loaded = false;
  static bool				atexit_registered = false;
  static std::map<std::string, DynamicComponent> dynamic_components;
  static std::map<std::string, std::string> circuit_components;
  static std::set<std::string>		registered_types;
  static std::vector<void*>		dynamic_handles;

  static bool		has_suffix(const std::string &str, const std::string &suffix)
  {
    if (str.size() < suffix.size())
      return (false);
    return (str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0);
  }

  static std::string	join_path(const std::string &dir, const std::string &file)
  {
    if (dir.empty() || dir[dir.size() - 1] == '/')
      return (dir + file);
    return (dir + "/" + file);
  }

  static std::string	basename_without_suffix(const std::string &file, const std::string &suffix)
  {
    std::string		name = file;
    size_t		pos;

    pos = name.find_last_of('/');
    if (pos != std::string::npos)
      name = name.substr(pos + 1);
    if (has_suffix(name, suffix))
      name.resize(name.size() - suffix.size());
    return (name);
  }

  static void		close_dynamic_components(void)
  {
    for (size_t i = 0; i < dynamic_handles.size(); ++i)
      if (dynamic_handles[i] != NULL)
	dlclose(dynamic_handles[i]);
    dynamic_handles.clear();
    dynamic_components.clear();
  }

  static void		warn_collision(const std::string &type, const std::string &path)
  {
    std::cerr << "bcad: component name collision for '" << type << "' in " << path
	      << ", keeping the first registered component." << std::endl;
  }

  static bool		register_type(const std::string &type, const std::string &path)
  {
    if (type.empty())
      return (false);
    if (registered_types.find(type) != registered_types.end())
      {
	warn_collision(type, path);
	return (false);
      }
    registered_types.insert(type);
    return (true);
  }

  static BcadListComponents find_list_function(void *handle)
  {
    BcadListComponents list;

    list = (BcadListComponents)dlsym(handle, "GetComponents");
    if (list != NULL)
      return (list);
    list = (BcadListComponents)dlsym(handle, "ListComponents");
    if (list != NULL)
      return (list);
    return ((BcadListComponents)dlsym(handle, "Components"));
  }

  static void		load_dynamic_library(const std::string &path)
  {
    void		*handle;
    BcadListComponents list;
    BcadCreateComponent create;
    const char		**components;
    std::vector<std::string> accepted;

    handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (handle == NULL)
      {
	std::cerr << "bcad: cannot load component library '" << path << "': "
		  << dlerror() << std::endl;
	return ;
      }
    list = find_list_function(handle);
    create = (BcadCreateComponent)dlsym(handle, "Create");
    if (list == NULL || create == NULL)
      {
	std::cerr << "bcad: component library '" << path
		  << "' misses GetComponents/ListComponents/Components or Create." << std::endl;
	dlclose(handle);
	return ;
      }
    components = list();
    if (components == NULL)
      {
	dlclose(handle);
	return ;
      }
    for (size_t i = 0; components[i] != NULL; ++i)
      if (register_type(components[i], path))
	accepted.push_back(components[i]);
    if (accepted.empty())
      {
	dlclose(handle);
	return ;
      }
    dynamic_handles.push_back(handle);
    for (size_t i = 0; i < accepted.size(); ++i)
      dynamic_components[accepted[i]] = {handle, create, path};
  }

  static void		load_rs_component(const std::string &path, const std::string &file)
  {
    std::string		type = basename_without_suffix(file, ".rs");

    if (register_type(type, path))
      circuit_components[type] = path;
  }

  static std::vector<std::string> component_paths(void)
  {
    std::vector<std::string> paths;
    const char		*env = std::getenv("BCAD_PATH");

    paths.push_back("/usr/share/bcad");
    if (env != NULL)
      {
	std::string all = env;
	size_t start = 0;
	while (start <= all.size())
	  {
	    size_t end = all.find(':', start);
	    std::string path = all.substr(start, end == std::string::npos ? std::string::npos : end - start);
	    if (!path.empty())
	      paths.push_back(path);
	    if (end == std::string::npos)
	      break;
	    start = end + 1;
	  }
      }
    return (paths);
  }

  static void		scan_component_directory(const std::string &path)
  {
    DIR			*dir;
    struct dirent	*ent;

    dir = opendir(path.c_str());
    if (dir == NULL)
      return ;
    while ((ent = readdir(dir)) != NULL)
      {
	std::string file = ent->d_name;
	std::string full = join_path(path, file);

	if (has_suffix(file, ".so"))
	  load_dynamic_library(full);
	else if (has_suffix(file, ".rs"))
	  load_rs_component(full, file);
      }
    closedir(dir);
  }

  static void		load_all_external_components(const std::vector<std::string> &builtins)
  {
    std::vector<std::string> paths;

    if (loaded)
      return ;
    loaded = true;
    registered_types.insert(builtins.begin(), builtins.end());
    if (!atexit_registered)
      {
	atexit(close_dynamic_components);
	atexit_registered = true;
      }
    paths = component_paths();
    for (size_t i = 0; i < paths.size(); ++i)
      scan_component_directory(paths[i]);
  }
}

void			hbs::Circuit::LoadExternalComponents(void)
{
  load_all_external_components(creatable_types);
  for (std::map<std::string, DynamicComponent>::const_iterator it = dynamic_components.begin();
       it != dynamic_components.end(); ++it)
    if (std::find(creatable_types.begin(), creatable_types.end(), it->first) == creatable_types.end())
      creatable_types.push_back(it->first);
  for (std::map<std::string, std::string>::const_iterator it = circuit_components.begin();
       it != circuit_components.end(); ++it)
    if (std::find(creatable_types.begin(), creatable_types.end(), it->first) == creatable_types.end())
      creatable_types.push_back(it->first);
}

hbs::IComponent		*hbs::Circuit::Create(const std::string		&type,
					      const std::string		&name,
					      const std::string		&value,
					      const std::string		&pos)
{
  std::stringstream	ss;
  std::string		position;
  hbs::IComponent	*tmp;

  if (pos == "")
    {
      // Pin size is 10 by default
      ss << (10 + rand() % (int)((1440 / PINSIZE_DEFAULT) * 0.8)) << "," << (10 + rand() % (int)((900 / PINSIZE_DEFAULT) * 0.8)) << " ";
      position = ss.str();
    }
  else
    position = pos;

  if (type == "input")
    return (new hbs::Input(timer, name, value, position));
  if (type == "clock")
    return (new hbs::Clock(timer, name, value, position));
  if (type == "output")
    return (new hbs::Output(timer, name, position));
  if (type == "true")
    return (new hbs::True(timer, name, position));
  if (type == "false")
    return (new hbs::False(timer, name, position));

  if ((tmp = hbs::CMP_4000::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4002::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4006::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4009::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4010::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4012::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4023::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4025::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4027::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4020::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4021::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4022::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4024::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4026::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4028::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4029::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4033::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4035::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4042::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4043::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4044::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4063::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4076::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4099::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4041::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4049::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4050::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4068::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4072::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4073::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4075::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4077::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4078::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4082::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4085::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4086::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4093::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4584::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4001::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4008::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4011::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4013::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4014::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4015::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4017::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4030::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4040::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4069::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4071::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4081::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4094::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4502::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4503::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4514::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4510::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4511::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4515::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4516::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4518::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4520::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4555::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_4556::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_74164::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_74165::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_74194::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_74595::Create(timer, type, name, value, position)) != NULL)
    return (tmp);
  if ((tmp = hbs::CMP_2716::Create(timer, type, name, value, position)) != NULL)
    return (tmp);

  if (type == "terminal")
    return (new hbs::Terminal(timer, name, position));

  if (type == "wire" || type == "track")
    return (new hbs::Track(timer, name, value));

  std::map<std::string, DynamicComponent>::iterator dyn = dynamic_components.find(type);
  if (dyn != dynamic_components.end())
    return (dyn->second.create(timer, type, name, value, position));

  std::map<std::string, std::string>::iterator rs = circuit_components.find(type);
  if (rs != circuit_components.end())
    {
      hbs::Circuit *component = new hbs::Circuit(timer);

      component->Load(rs->second);
      component->ConfigureAsComponent(type, name, position);
      return (component);
    }

  throw hbs::BadComponent(std::string("Unknown component ") + type);
  return (NULL);
}
