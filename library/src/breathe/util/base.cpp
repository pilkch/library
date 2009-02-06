#include <cmath>

#include <string>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/geometry.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cColour.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/util/base.h>

namespace breathe
{
  // cObject
  cObject::cObject()
  {
    fWidth=1.0f;
    fHeight=1.0f;
    fLength=1.0f;

    fWeight=1.0f;
  }

  void cObject::SetDimensions(float fInWidth, float fInLength, float fInHeight)
  {
    fWidth = fInWidth;
    fLength = fInLength;
    fHeight = fInHeight;

    fRadius = fWidth;
    if (fInLength > fRadius) fRadius = fInLength;
    if (fInHeight > fRadius) fRadius = fInHeight;
  }

  void cObject::SetDimensions(float fInWidth, float fInHeight)
  {
    fWidth = fInWidth;
    fLength = 0.0f;
    fHeight = fInHeight;

    fRadius = fWidth;
    if (fInHeight > fRadius) fRadius = fInHeight;
  }

  bool cObject::Collide(cObject& rhs)
  {
    if (cSphere::Collide(rhs))
      return true;



    return false;
  }

  float cObject::GetDistance(cObject& rhs) const
  {
    float fDistance = cSphere::GetDistance(rhs);



    return fDistance;
  }
}
