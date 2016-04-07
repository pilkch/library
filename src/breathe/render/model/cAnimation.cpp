#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cmath>

#include <list>
#include <sstream>
#include <vector>
#include <map>

#include <iostream>
#include <fstream>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>

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
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>
#include <breathe/render/model/cAnimation.h>

namespace breathe
{
  namespace render
  {
    namespace model
    {
      cAnimation::cAnimation() : cModel()
      {
        LOG.Error("Animation", "Don't use this class");
      }

      cAnimation::~cAnimation()
      {
        LOG.Error("Animation", "Don't use this class");
      }

      int cAnimation::Load(const std::string& sFilename)
      {
        LOG.Error("Animation", "Don't use this class");

        return 0;
      }

      void cAnimation::Update(durationms_t currentTime)
      {
        LOG.Error("Animation", "Don't use this class");
      }
    }
  }
}
