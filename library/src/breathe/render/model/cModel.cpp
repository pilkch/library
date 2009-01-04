#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <cmath>

#include <list>
#include <vector>
#include <map>

// writing on a text file
#include <iostream>
#include <fstream>
#include <sstream>

// Boost includes
#include <boost/shared_ptr.hpp>


#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>


// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cOctree.h>
#include <breathe/math/cColour.h>
#include <breathe/math/geometry.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>

namespace breathe
{
  namespace render
  {
    namespace model
    {
      cModel::cModel()
      {
      }

      cModel::~cModel()
      {
      }
    }
  }
}
