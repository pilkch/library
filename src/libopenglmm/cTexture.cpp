// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>

#include <string>
#include <sstream>
#include <map>
#include <vector>

// OpenGL headers
#include <GL/GLee.h>
#include <GL/glu.h>

// SDL headers
#include <SDL/SDL_image.h>

// libopenglmm headers
#include <libopenglmm/cTexture.h>

namespace opengl
{
  cTexture::cTexture()
  {
  }

  cTexture::~cTexture()
  {
  }

  bool cTexture::IsValid() const
  {
    return false;
  }

  bool cTexture::LoadFromFile(const std::string& sFileName)
  {
    return false;
  }
}
