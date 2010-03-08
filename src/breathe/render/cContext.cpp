// Standard headers
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <cmath>

#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <vector>
#include <map>
#include <list>
#include <algorithm>

// Boost headers
#include <boost/shared_ptr.hpp>

// Anything else
#include <GL/GLee.h>
#include <GL/glu.h>

#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/cTimer.h>

#include <spitfire/algorithm/algorithm.h>

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/settings.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/geometry.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/util/base.h>
#include <breathe/util/cVar.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cContext.h>
#include <breathe/render/cResourceManager.h>
#include <breathe/render/cRender.h>
#include <breathe/render/cVertexBufferObject.h>

#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

namespace breathe
{
  namespace render
  {
    // *** cContext

    cContext::cContext()
    {
    }

    cContext::~cContext()
    {
    }

    bool cContext::CreateSharedContextFromWindow(const cWindow& window)
    {
      LOG<<"cContext::CreateSharedContextFromWindow"<<std::endl;

      return false;
    }

    bool cContext::CreateSharedContextFromContext(const cContext& context)
    {
      LOG<<"cContext::CreateSharedContextFromContext"<<std::endl;

      return false;
    }

    void cContext::Destroy()
    {
    }
  }
}
