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
    cPlane::cPlane(const cVec3& origin, const cVec3& normal)
    {
      this->normal = normal;
      this->origin = origin;
      equation[0] = normal.x;
      equation[1] = normal.y;
      equation[2] = normal.z;
      equation[3] = -(normal.x*origin.x+normal.y*origin.y+normal.z*origin.z);
    }

    // Construct from triangle:
    cPlane::cPlane(const cVec3& p1,const cVec3& p2,const cVec3& p3)
    {
      normal = (p2-p1).CrossProduct(p3-p1);
      normal.Normalise();
      origin = p1;
      equation[0] = normal.x;
      equation[1] = normal.y;
      equation[2] = normal.z;
      equation[3] = -(normal.x*origin.x+normal.y*origin.y+normal.z*origin.z);
    }

    bool cPlane::IsFrontFacingTo(const cVec3& direction) const
    {
      double dot = normal.DotProduct(direction);
      return (dot <= 0);
    }

    double cPlane::SignedDistanceTo(const cVec3& point) const
    {
      return (point.DotProduct(normal)) + equation[3];
    }






    cPlane::cPlane(const cPlane & rhs)
    {
      normal=rhs.normal;
      intercept=rhs.intercept;
    }

    void cPlane::SetFromPoints(const cVec3 & p0, const cVec3 & p1, const cVec3 & p2)
    {
      normal=(p1-p0).CrossProduct(p2-p0);

                                                normal.Normalise();

      CalculateIntercept(p0);
    }

                                void cPlane::Normalise()
    {
      float normalLength=normal.GetLength();
      normal/=normalLength;
      intercept/=normalLength;
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

    cVec3 cPlane::GetReflected(cVec3 v) const
    {
      return cVec3(-2.0f * (v.DotProduct(normal) * normal + v));
    }

    float cPlane::DistancePoint(const cVec3& point) const
    {
      return point.DotProduct(normal)+intercept;
    }

    int cPlane::ClassifyPoint(const cVec3& point) const
    {
      if (point.DotProduct(normal)+intercept==0.0f)
        return POINT_ON_cPlane;

      if (point.DotProduct(normal)+intercept>0.0f)
        return POINT_IN_FRONT_OF_cPlane;

      return POINT_BEHIND_cPlane;  //otherwise
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
