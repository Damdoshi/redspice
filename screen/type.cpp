// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		<cctype>
#include		"Circuit.hpp"
#include		"Screen.hpp"

static bool		valid_name_char(uint32_t unicode)
{
  return ((unicode >= 'a' && unicode <= 'z') ||
	  (unicode >= 'A' && unicode <= 'Z') ||
	  (unicode >= '0' && unicode <= '9') ||
	  unicode == '_');
}

extern "C"
t_bunny_response	screen_type(uint32_t		unicode,
				    LoopData		&ld)
{
  if (ld.screen.rename_mode)
    {
      if (valid_name_char(unicode))
	ld.screen.rename_buffer.push_back((char)unicode);
      return (GO_ON);
    }
  if (ld.screen.search_panel)
    {
      if (unicode >= 32 && unicode < 127)
	{
	  ld.screen.search_query.push_back((char)std::tolower((int)unicode));
	  ld.screen.search_offset = 0;
	}
    }
  return (GO_ON);
}
