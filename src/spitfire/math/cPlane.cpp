#include <cstdlib>
#include <cmath>

#include <vector>
#include <limits>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cColour.h>

namespace spitfire
{
  namespace math
  {
    cPlane::cPlane() :
      normal(),
      intercept(0.0f)
    {
    }

    cPlane::cPlane(const cVec3& newNormal, float newIntercept) :
      normal(newNormal),
      intercept(newIntercept)
    {
    }

    // Plane from a triangle
    cPlane::cPlane(const cVec3& p0, const cVec3& p1, const cVec3& p2)
    {
      normal = (p1 - p0).CrossProduct(p2 - p0);

      const double length = normal.GetLength();

      normal /= length;
      intercept = p0.DotProduct(normal);
    }

    cPlane::cPlane(const cPlane & rhs)
    {
      normal = rhs.normal;
      intercept = rhs.intercept;
    }

    void cPlane::SetFromPoints(const cVec3 & p0, const cVec3 & p1, const cVec3 & p2)
    {
      normal = (p1 - p0).CrossProduct(p2 - p0);

      normal.Normalise();

      CalculateIntercept(p0);
    }

    void cPlane::Normalise()
    {
      const float normalLength = normal.GetLength();
      normal /= normalLength;
      intercept /= normalLength;
    }

    // Find point of intersection of 3 planes
    bool cPlane::Intersect3(const cPlane& p2, const cPlane& p3, cVec3& result) const
    {
      // Scalar triple product of normals
      const float denominator = normal.DotProduct((p2.normal).CrossProduct(p3.normal));

      // If zero, there is no intersection
      if (denominator == 0.0f) return false;

      const cVec3 temp1 = (p2.normal.CrossProduct(p3.normal)) * intercept;
      const cVec3 temp2 = (p3.normal.CrossProduct(normal)) * p2.intercept;
      const cVec3 temp3 = (normal.CrossProduct(p2.normal)) * p3.intercept;

      result = (temp1 + temp2 + temp3) / (-denominator);

      return true;
    }

    cVec3 cPlane::GetReflected(const cVec3& v) const
    {
      return cVec3(-2.0f * (v.DotProduct(normal) * normal + v));
    }

    bool cPlane::IsFrontFacingToDirection(const cVec3& direction) const
    {
      return (normal.DotProduct(direction) <= 0.0);
    }

    float cPlane::GetDistanceToPoint(const cVec3& point) const
    {
      return point.DotProduct(normal) + intercept;
    }

    PLANE_POINT_CLASSIFICATION cPlane::ClassifyPoint(const cVec3& point) const
    {
      if ((point.DotProduct(normal) + intercept) == 0.0f) return PLANE_POINT_CLASSIFICATION::POINT_ON_PLANE;

      if ((point.DotProduct(normal) + intercept) > 0.0f) return PLANE_POINT_CLASSIFICATION::POINT_IN_FRONT_OF_PLANE;

      return PLANE_POINT_CLASSIFICATION::POINT_BEHIND_PLANE;
    }

    cPlane cPlane::lerp(const cPlane& p2, float factor) const
    {
      cPlane result;
      result.normal = normal * factor + p2.normal * (1.0f - factor);
      result.normal.Normalise();

      result.intercept = intercept * factor + p2.intercept * (1.0f - factor);

      return result;
    }


    bool cPlane::operator ==(const cPlane& rhs) const
    {
      return ((normal == rhs.normal) && (intercept == rhs.intercept));
    }

    bool cPlane::operator !=(const cPlane& rhs) const
    {
      return !((*this) == rhs);
    }
  }
}
