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
  // *** cResolution

  cResolution::cResolution() :
    width(0),
    height(0),
    pixelFormat(PIXELFORMAT::R8G8B8A8)
  {
  }
}
