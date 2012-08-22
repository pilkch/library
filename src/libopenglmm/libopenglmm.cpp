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
#include <SDL/SDL_image.h>

// libopenglmm headers
#include <libopenglmm/libopenglmm.h>
#include <libopenglmm/opengl.h>

namespace opengl
{
  namespace string
  {
    std::string ToUTF8(const std::wstring& source)
    {
      std::string result(source.size(), char(0));
      typedef std::ctype<wchar_t> ctype_t;
      const ctype_t& ct = std::use_facet<ctype_t>(std::locale());
#ifdef __LINUX__
      ct.narrow(source.data(), source.data() + source.size(), '0', &(*result.begin()));
#else
      ct.narrow(source.data(), source.data() + source.size(), '\u00B6', &(*result.begin()));
#endif
      return result;
    }
  }


  // *** cResolution

  cResolution::cResolution() :
    width(0),
    height(0),
    pixelFormat(PIXELFORMAT::R8G8B8A8)
  {
  }

#if 0
    bool cResolution::IsWideScreen() const
    {
      const float fWideScreenRatio = 16.0f / 9.0f;

      const float fRatio = (float(width) / float(height));

      return (fRatio >= fWideScreenRatio);
    }

    bool cResolution::ResolutionCompare(const cResolution& lhs, const cResolution& rhs)
    {
      if (lhs.IsWideScreen() > rhs.IsWideScreen()) return true;
      if (lhs.GetColourDepth() > rhs.GetColourDepth()) return true;

      if (lhs.GetWidth() > rhs.GetWidth()) return true;

      return (lhs.GetHeight() > rhs.GetHeight());
    }
#endif
}
