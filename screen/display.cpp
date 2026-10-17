// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#include		"Screen.hpp"

extern "C"
t_bunny_response	screen_display(LoopData			&ld)
try
{
  if (ld.file_browser || !ld.HasDocument())
    hbs::DrawFileBrowser(ld);
  else
    ld.screen.Draw(ld.CurrentCircuit());
  return (GO_ON);
}
catch (const std::exception &error)
{
  hbs::ReportScreenError(ld, error);
  return (GO_ON);
}
