#ifndef CFONT_H
#define CFONT_H

// libopenglmm headers
#include <libopenglmm/cFont.h>

// Spitfire headers
#include <spitfire/util/cSmartPtr.h>

namespace breathe
{
  namespace render
  {
    using opengl::cFont;

    typedef cSmartPtr<cFont> cFontRef;
  }
}

#endif // CFONT
