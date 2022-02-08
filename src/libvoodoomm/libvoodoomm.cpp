// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

// SDL headers
#include <SDL2/SDL_image.h>

// libvoodoomm headers
#include <libvoodoomm/libvoodoomm.h>

namespace voodoo
{
  namespace string
  {
    std::string ToUTF8(const string_t& source)
    {
      #ifdef UNICODE
      std::string result(source.size(), char(0));
      typedef std::ctype<wchar_t> ctype_t;
      const ctype_t& ct = std::use_facet<ctype_t>(std::locale());
      #ifdef __LINUX__
      ct.narrow(source.data(), source.data() + source.size(), '0', &(*result.begin()));
      #else
      ct.narrow(source.data(), source.data() + source.size(), '\u00B6', &(*result.begin()));
      #endif
      return result;
      #else
      return source;
      #endif
    }
  }
}
