#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <list>

// Boost includes
#include <boost/shared_ptr.hpp>


#ifdef BUILD_PHYSICS_3D
#include <ode/ode.h>
#endif

#include <breathe/breathe.h>

#include <breathe/util/cSmartPtr.h>
#include <breathe/util/cString.h>

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

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cMaterial.h>


#include <breathe/game/cLevel.h>

#include <breathe/physics/physics.h>

#include <breathe/game/cItem.h>

namespace breathe
{
  cItem::cItem()
    : cUpdateable()
  {

  }

  cItem::~cItem()
  {

  }
}
