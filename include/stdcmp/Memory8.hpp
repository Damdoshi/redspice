// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_MEMORY8_HPP__
# define                        __HBS_MEMORY8_HPP__
# include                       <stdint.h>
# include                       <string>
# include                       <vector>
# include                       "AComponent.hpp"

namespace                       hbs
{
  class                         Memory8Utility
  {
  protected:
    static bool                 LoadFile(const std::string &file,
                                         std::vector<uint8_t> &memory,
                                         uint8_t fill);
  };
}

#endif //                       __HBS_MEMORY8_HPP__
