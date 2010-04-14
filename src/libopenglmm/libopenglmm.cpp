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

// OpenGL headers
#include <GL/GLee.h>
#include <GL/glu.h>

// SDL headers
#include <SDL/SDL_image.h>

// libopenglmm headers
#include <libopenglmm/libopenglmm.h>

namespace opengl
{
  size_t GetBytesForPixelFormat(PIXELFORMAT pixelFormat)
  {
    if (pixelFormat == PIXELFORMAT::R8G8B8A8) return 4;
    if (pixelFormat == PIXELFORMAT::R8G8B8) return 3;

    return 2;
  }

  size_t GetBitsForPixelFormat(PIXELFORMAT pixelFormat)
  {
    return 8 * GetBytesForPixelFormat(pixelFormat);
  }

  // *** cResolution

  cResolution::cResolution() :
    width(0),
    height(0),
    pixelFormat(PIXELFORMAT::R8G8B8A8)
  {
  }
}
