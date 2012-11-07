#ifndef CRENDERSYSTEM_H
#define CRENDERSYSTEM_H

// libopenglmm headers
#include <libopenglmm/libopenglmm.h>
#include <libopenglmm/cContext.h>
#include <libopenglmm/cSystem.h>
#include <libopenglmm/cWindow.h>

namespace breathe
{
  namespace render
  {
    using opengl::cResolution;
    using opengl::cCapabilities;

    using opengl::cSystem;

    class cResourceManager;
  }
}

#endif // CRENDERSYSTEM_H
