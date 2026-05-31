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
#include		"4001.hpp"
#include		"4008.hpp"
#include		"4011.hpp"
#include		"4013.hpp"
#include		"4017.hpp"
#include		"4030.hpp"
#include		"4040.hpp"
#include		"4069.hpp"
#include		"4071.hpp"
#include		"4081.hpp"
#include		"4094.hpp"
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

  if (type == "4001")
    return (new hbs::CMP_4001(timer, name, position));
  if (type == "4008")
    return (new hbs::CMP_4008(timer, name, position));
  if (type == "4011")
    return (new hbs::CMP_4011(timer, name, position));
  if (type == "4013")
    return (new hbs::CMP_4013(timer, name, position));
  if (type == "4017")
    return (new hbs::CMP_4017(timer, name, position));
  if (type == "4030")
    return (new hbs::CMP_4030(timer, name, position));
  if (type == "4040")
    return (new hbs::CMP_4040(timer, name, position));
  if (type == "4069")
    return (new hbs::CMP_4069(timer, name, position));
  if (type == "4071")
    return (new hbs::CMP_4071(timer, name, position));
  if (type == "4081")
    return (new hbs::CMP_4081(timer, name, position));
  if (type == "4094")
    return (new hbs::CMP_4094(timer, name, position));
  if (type == "4514")
    return (new hbs::CMP_4514(timer, name, position));

  if (type == "2716")
    return (new hbs::CMP_2716(timer, name, value, position));

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
