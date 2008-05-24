#include <cstdlib>
#include <cmath>

#include <vector>
#include <limits>

// Breathe
#include <breathe/breathe.h>

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

#ifdef max
#undef max
#endif

namespace breathe
{
  namespace math
  {
    const float cINFINITY = std::numeric_limits<float>::max(); //1e30f //std::numeric_limits<float>::infinity();
  }
}
