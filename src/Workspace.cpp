// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<algorithm>
#include		<cctype>
#include		<chrono>
#include		<filesystem>
#include		<fstream>
#include		<iostream>
#include		<sstream>
#include		<unistd.h>
#include		"Circuit.hpp"
#include		"Screen.hpp"
#include		"Track.hpp"

// static const char	*empty_circuit = ".chipsets:\n\n.tracks:\n\n.links:\n";

static std::string	canonical_filename(const std::string &file)
{
  namespace fs = std::filesystem;
  std::error_code	ec;
  fs::path		p = fs::absolute(fs::path(file), ec);

  if (file.empty())
    return ("");
  if (ec)
    p = fs::path(file);
  fs::path weak = fs::weakly_canonical(p, ec);
  if (!ec)
    p = weak;
  return (p.lexically_normal().string());
}

static std::string	parent_directory(const std::string &file)
{
  namespace fs = std::filesystem;
  std::error_code	ec;

  if (file.empty())
    return (fs::current_path(ec).string());
  fs::path		p(file);
  if (p.has_parent_path())
    return (p.parent_path().string());
  return (fs::current_path(ec).string());
}

static std::string	basename(const std::string &path)
{
  std::filesystem::path p(path);
  std::string		name = p.filename().string();

  if (name.empty())
    return (path);
  return (name);
}

static bool		is_rs_save_file(const std::filesystem::path &path)
{
  return (path.has_extension() &&
	  (path.extension() == ".rs" || path.extension() == ".rs~"));
}

static std::string	with_default_rs_extension(std::filesystem::path path)
{
  if (!path.has_extension())
    path += ".rs";
  return (path.string());
}

static std::string	trim(const std::string &str)
{
  size_t		begin = 0;
  size_t		end = str.size();

  while (begin < end && std::isspace((unsigned char)str[begin]))
    begin += 1;
  while (end > begin && std::isspace((unsigned char)str[end - 1]))
    end -= 1;
  return (str.substr(begin, end - begin));
}

static bool		parse_position_value(const std::string &line,
				     hbs::Position &pos)
{
  size_t		open = line.find('[');
  size_t		comma;
  size_t		close;

  if (open == std::string::npos)
    {
      std::stringstream ss(line);
      std::string word;

      if (!(ss >> word >> pos.x >> pos.y))
	return (false);
      return (true);
    }
  comma = line.find(',', open + 1);
  close = line.find(']', open + 1);
  if (comma == std::string::npos || close == std::string::npos || comma > close)
    return (false);
  try
    {
      pos.x = std::stod(line.substr(open + 1, comma - open - 1));
      pos.y = std::stod(line.substr(comma + 1, close - comma - 1));
    }
  catch (...)
    {
      return (false);
    }
  return (true);
}

static bool		load_document_view(const std::string &file,
				   hbs::Position &camera,
				   int &pin_size)
{
  std::ifstream		is(file.c_str(), std::ios::in | std::ios::binary);
  std::string		line;
  bool			misc = false;
  bool			found = false;

  if (!is)
    return (false);
  while (std::getline(is, line))
    {
      size_t comment = line.find('#');

      if (comment != std::string::npos)
	line.erase(comment);
      line = trim(line);
      if (line.empty())
	continue ;
      if (line[0] == '.')
	{
	  misc = (line == ".misc:");
	  continue ;
	}
      if (!misc)
	continue ;
      std::stringstream ss(line);
      std::string word;

      ss >> word;
      if (word == "camera")
	{
	  hbs::Position loaded = camera;

	  if (parse_position_value(line, loaded))
	    camera = loaded, found = true;
	}
      else if (word == "zoom" || word == "pin_size")
	{
	  int loaded;

	  if (ss >> loaded && loaded > 0)
	    pin_size = loaded, found = true;
	}
    }
  return (found);
}

static bool		save_circuit_with_view(hbs::Circuit &circuit,
				       const std::string &file,
				       const hbs::Position &camera,
				       int pin_size)
{
  std::ofstream		os;

  if (!circuit.Save(file))
    return (false);
  os.open(file.c_str(), std::ios::out | std::ios::binary | std::ios::app);
  if (!os)
    return (false);
  os << "\n.misc:\n";
  os << "camera [" << camera.x << "," << camera.y << "]\n";
  os << "zoom " << pin_size << "\n";
  return ((bool)os);
}

static std::string	make_temporary_filename(void)
{
  namespace fs = std::filesystem;
  static unsigned int counter = 0;
  std::stringstream ss;
  std::error_code ec;

  ss << "untitled-" << (long)getpid() << "-"
     << std::chrono::steady_clock::now().time_since_epoch().count()
     << "-" << counter++ << ".rs~";
  return ((fs::current_path(ec) / ss.str()).string());
}

static void		reset_document_interaction(hbs::Screen &screen)
{
  screen.panning = false;
  screen.selecting = false;
  screen.dragging_selection = false;
  screen.context_menu = false;
  screen.rename_mode = false;
  screen.placing_component = false;
  screen.component_to_place = NULL;
  screen.placing_type = "";
  screen.rename_buffer = "";
  screen.search_panel = false;
  screen.search_offset = 0;
  screen.search_query = "";
  screen.grabbed = NULL;
  screen.grabbed_step = hbs::Packet();
  screen.active_track = NULL;
  screen.active_node = hbs::Track::NoNode;
  screen.selected_components.clear();
  screen.selected_tracks.clear();
}

static std::string	build_browser_target_path(const LoopData &ld)
{
  namespace fs = std::filesystem;
  fs::path		path(trim(ld.browser_target));

  if (path.empty())
    return ("");
  if (path.is_relative())
    path = fs::path(ld.browser_directory) / path;
  return (canonical_filename(with_default_rs_extension(path)));
}

LoopDocument::LoopDocument(hbs::Circuit &c,
			   hbs::Timer &t,
			   const std::string &file)
  : file_name(canonical_filename(file)),
    timer(&t),
    circuit(&c),
    camera({0, 0}),
    pin_size(PINSIZE_DEFAULT),
    owned(false),
    dirty(false),
    temporary(false)
{
  load_document_view(file_name, camera, pin_size);
}

LoopDocument::LoopDocument(const std::string &file,
			   bool create_if_missing,
			   bool tmp)
  : file_name(canonical_filename(file)),
    timer(new hbs::Timer()),
    circuit(NULL),
    camera({0, 0}),
    pin_size(PINSIZE_DEFAULT),
    owned(true),
    dirty(tmp),
    temporary(tmp)
{
  try
    {
      circuit = new hbs::Circuit(*timer);
      circuit->Load(file_name, create_if_missing);
      load_document_view(file_name, camera, pin_size);
    }
  catch (...)
    {
      delete circuit;
      delete timer;
      circuit = NULL;
      timer = NULL;
      throw ;
    }
}

LoopDocument::~LoopDocument(void)
{
  if (owned)
    {
      delete circuit;
      delete timer;
    }
}

LoopData::LoopData(hbs::Circuit &c,
		   hbs::Timer &t,
		   hbs::Screen &s)
  : screen(s),
    active_document(0),
    file_browser(false),
    file_browser_mode(FILE_BROWSER_OPEN),
    browser_directory(parent_directory(canonical_filename(s.file_name))),
    browser_offset(0),
    opened_offset(0),
    browser_error(""),
    browser_target(""),
    quit_requested(false)
{
  if (!s.file_name.empty())
    {
      documents.push_back(new LoopDocument(c, t, s.file_name));
      screen.file_name = documents[0]->file_name;
      ApplyActiveView();
    }
  else
    {
      file_browser = true;
      screen.file_name = "";
      reset_document_interaction(screen);
    }
  RefreshBrowser();
}

LoopData::~LoopData(void)
{
  for (size_t i = 0; i < documents.size(); ++i)
    delete documents[i];
}

bool			LoopData::HasDocument(void) const
{
  return (!documents.empty() && active_document < documents.size() &&
	  documents[active_document]->circuit != NULL && documents[active_document]->timer != NULL);
}

hbs::Circuit		&LoopData::CurrentCircuit(void)
{
  return (*documents[active_document]->circuit);
}

hbs::Timer		&LoopData::CurrentTimer(void)
{
  return (*documents[active_document]->timer);
}

const hbs::Circuit	&LoopData::CurrentCircuit(void) const
{
  return (*documents[active_document]->circuit);
}

const hbs::Timer	&LoopData::CurrentTimer(void) const
{
  return (*documents[active_document]->timer);
}

void			LoopData::MarkDirty(void)
{
  if (HasDocument())
    documents[active_document]->dirty = true;
}

void			LoopData::SaveActiveView(void)
{
  if (!HasDocument())
    return ;
  documents[active_document]->camera = screen.camera;
  documents[active_document]->pin_size = screen.pin_size;
}

void			LoopData::ApplyActiveView(void)
{
  if (!HasDocument())
    {
      screen.file_name = "";
      reset_document_interaction(screen);
      return ;
    }
  screen.camera = documents[active_document]->camera;
  screen.pin_size = documents[active_document]->pin_size;
  screen.file_name = documents[active_document]->file_name;
  reset_document_interaction(screen);
}

bool			LoopData::SelectDocument(size_t index)
{
  if (index >= documents.size())
    return (false);
  SaveActiveView();
  active_document = index;
  ApplyActiveView();
  browser_error = "";
  return (true);
}

bool			LoopData::OpenFile(const std::string &file)
{
  std::string		name = canonical_filename(file);

  for (size_t i = 0; i < documents.size(); ++i)
    if (documents[i]->file_name == name)
      return (SelectDocument(i));
  try
    {
      LoopDocument *doc = new LoopDocument(name);

      SaveActiveView();
      documents.push_back(doc);
      active_document = documents.size() - 1;
      ApplyActiveView();
      file_browser = false;
      file_browser_mode = FILE_BROWSER_OPEN;
      browser_error = "";
    }
  catch (const std::exception &e)
    {
      browser_error = e.what();
      return (false);
    }
  return (true);
}

bool			LoopData::NewDocument(void)
{
  std::string		name = make_temporary_filename();

  try
    {
      LoopDocument *doc = new LoopDocument(name, true, true);

      SaveActiveView();
      documents.push_back(doc);
      active_document = documents.size() - 1;
      ApplyActiveView();
      file_browser = false;
      browser_error = "";
      browser_directory = parent_directory(name);
      RefreshBrowser();
    }
  catch (const std::exception &e)
    {
      browser_error = e.what();
      file_browser = true;
      file_browser_mode = FILE_BROWSER_OPEN;
      return (false);
    }
  return (true);
}

void			LoopData::BeginOpenFileBrowser(void)
{
  file_browser = true;
  file_browser_mode = FILE_BROWSER_OPEN;
  browser_target = "";
  RefreshBrowser();
}

void			LoopData::BeginFileMenu(void)
{
  file_browser = !file_browser;
  file_browser_mode = FILE_BROWSER_OPEN;
  browser_target = "";
  if (!HasDocument())
    file_browser = true;
  if (file_browser)
    RefreshBrowser();
}

void			LoopData::BeginSaveAs(bool copy_only)
{
  BeginSavePrompt(copy_only ? FILE_BROWSER_SAVE_COPY : FILE_BROWSER_SAVE_ACTIVE);
}

void			LoopData::BeginSavePrompt(FileBrowserMode mode)
{
  if (!HasDocument())
    {
      BeginFileMenu();
      return ;
    }
  SaveActiveView();
  file_browser = true;
  file_browser_mode = mode;
  browser_directory = parent_directory(documents[active_document]->file_name);
  if (documents[active_document]->temporary)
    browser_target = "untitled.rs";
  else if (mode == FILE_BROWSER_SAVE_COPY)
    {
      std::filesystem::path p(documents[active_document]->file_name);
      browser_target = p.stem().string() + "-copy.rs";
    }
  else
    browser_target = basename(documents[active_document]->file_name);
  RefreshBrowser();
}

bool			LoopData::SaveCurrentDocumentAs(const std::string &file,
						 bool copy_only)
{
  std::string		name = canonical_filename(file);
  std::string		old_tmp;
  bool			adopt;

  if (!HasDocument() || name.empty())
    return (false);
  SaveActiveView();
  if (!save_circuit_with_view(*documents[active_document]->circuit,
			      name,
			      documents[active_document]->camera,
			      documents[active_document]->pin_size))
    {
      browser_error = std::string("Cannot save ") + name;
      return (false);
    }
  adopt = (!copy_only || documents[active_document]->temporary);
  if (adopt)
    {
      if (documents[active_document]->temporary)
	old_tmp = documents[active_document]->file_name;
      documents[active_document]->file_name = name;
      documents[active_document]->temporary = false;
      documents[active_document]->dirty = false;
      screen.file_name = name;
      browser_directory = parent_directory(name);
      if (!old_tmp.empty() && old_tmp != name)
	std::filesystem::remove(old_tmp);
    }
  browser_error = "";
  RefreshBrowser();
  return (true);
}

bool			LoopData::SaveCurrentDocument(void)
{
  if (!HasDocument())
    return (false);
  if (documents[active_document]->temporary)
    {
      BeginSavePrompt(FILE_BROWSER_SAVE_ACTIVE);
      return (false);
    }
  return (SaveCurrentDocumentAs(documents[active_document]->file_name, false));
}

bool			LoopData::CompleteSavePrompt(bool discard)
{
  FileBrowserMode	mode = file_browser_mode;
  std::string		path;

  if (!HasDocument())
    {
      BeginFileMenu();
      return (false);
    }
  if (discard)
    {
      browser_error = "";
      if (mode == FILE_BROWSER_SAVE_THEN_CLOSE || mode == FILE_BROWSER_SAVE_THEN_QUIT)
	{
	  if (documents[active_document]->temporary)
	    std::filesystem::remove(documents[active_document]->file_name);
	  documents[active_document]->temporary = false;
	  documents[active_document]->dirty = false;
	  if (mode == FILE_BROWSER_SAVE_THEN_CLOSE)
	    return (CloseCurrentDocument(true));
	  return (RequestQuit());
	}
      file_browser = false;
      return (true);
    }
  path = build_browser_target_path(*this);
  if (path.empty())
    {
      browser_error = "Nom de fichier vide.";
      return (false);
    }
  if (!SaveCurrentDocumentAs(path, mode == FILE_BROWSER_SAVE_COPY))
    return (false);
  if (mode == FILE_BROWSER_SAVE_THEN_CLOSE)
    return (CloseCurrentDocument(true));
  if (mode == FILE_BROWSER_SAVE_THEN_QUIT)
    return (RequestQuit());
  file_browser = false;
  return (true);
}

bool			LoopData::CloseCurrentDocument(bool discard)
{
  LoopDocument		*doc;
  std::string		tmp;

  if (!HasDocument())
    {
      BeginFileMenu();
      return (true);
    }
  if (!discard && (documents[active_document]->dirty || documents[active_document]->temporary))
    {
      BeginSavePrompt(FILE_BROWSER_SAVE_THEN_CLOSE);
      return (false);
    }
  doc = documents[active_document];
  if (doc->temporary)
    tmp = doc->file_name;
  documents.erase(documents.begin() + active_document);
  delete doc;
  if (!tmp.empty())
    std::filesystem::remove(tmp);
  if (documents.empty())
    {
      active_document = 0;
      screen.file_name = "";
      file_browser = true;
      file_browser_mode = FILE_BROWSER_OPEN;
      reset_document_interaction(screen);
    }
  else
    {
      if (active_document >= documents.size())
	active_document = documents.size() - 1;
      ApplyActiveView();
    }
  RefreshBrowser();
  return (true);
}

bool			LoopData::RequestQuit(void)
{
  if (!documents.empty())
    for (size_t i = 0; i < documents.size(); ++i)
      if (documents[i]->dirty || documents[i]->temporary)
	{
	  SelectDocument(i);
	  BeginSavePrompt(FILE_BROWSER_SAVE_THEN_QUIT);
	  return (false);
	}
  quit_requested = true;
  return (true);
}

void			LoopData::RefreshBrowser(void)
{
  namespace fs = std::filesystem;
  std::vector<FileBrowserEntry> dirs;
  std::vector<FileBrowserEntry> files;
  std::error_code	ec;
  fs::path		current = fs::absolute(fs::path(browser_directory), ec);

  browser_entries.clear();
  if (ec)
    {
      browser_error = ec.message();
      return ;
    }
  current = fs::weakly_canonical(current, ec);
  if (!ec)
    browser_directory = current.string();
  else
    current = fs::path(browser_directory);
  if (current.has_parent_path())
    dirs.push_back(FileBrowserEntry{"..", current.parent_path().string(), true});
  for (fs::directory_iterator it(current, ec), end; !ec && it != end; it.increment(ec))
    {
      const fs::directory_entry &entry = *it;
      fs::path path = entry.path();
      std::string name = path.filename().string();

      if (!name.empty() && name[0] == '.')
	continue ;
      std::error_code type_ec;
      if (entry.is_directory(type_ec))
	dirs.push_back(FileBrowserEntry{name + "/", path.string(), true});
      else if (entry.is_regular_file(type_ec) && is_rs_save_file(path))
	files.push_back(FileBrowserEntry{name, path.string(), false});
    }
  if (ec)
    {
      browser_error = ec.message();
      return ;
    }
  std::sort(dirs.begin(), dirs.end(), [](const FileBrowserEntry &a, const FileBrowserEntry &b) {
    return (a.name < b.name);
  });
  std::sort(files.begin(), files.end(), [](const FileBrowserEntry &a, const FileBrowserEntry &b) {
    return (a.name < b.name);
  });
  browser_entries.insert(browser_entries.end(), dirs.begin(), dirs.end());
  browser_entries.insert(browser_entries.end(), files.begin(), files.end());
  browser_error = "";
  if (browser_offset >= (int)browser_entries.size())
    browser_offset = std::max(0, (int)browser_entries.size() - 1);
}
