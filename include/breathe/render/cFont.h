#ifndef CFONT_H
#define CFONT_H

// libopenglmm headers
#include <libopenglmm/cFont.h>

// Spitfire headers

namespace breathe
{
  namespace render
  {
    using opengl::cFont;

    typedef std::shared_ptr<cFont> cFontRef;
  }
}

#endif // CFONT
