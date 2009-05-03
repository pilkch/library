#include <cstdlib>
#include <cmath>

#include <vector>
#include <list>
#include <string>
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>

// Spitfire Includes
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/geometry.h>

namespace spitfire
{
  namespace math
  {
    // *** cLine3

    cVec3 cLine3::GetTangent() const
    {
      ASSERT(false);
      return v3Zero;
    }

    // *** cRay3

    void cRay3::SetOriginAndDirection(const cVec3& _origin, const cVec3& _direction)
    {
      origin = _origin;
      direction = _direction;
    }

    void cRay3::SetLength(float_t _length)
    {
      ASSERT(_length >= 0.0f);

      length = _length;
    }

    bool cRay3::CollideWithPlane(const cPlane& rhs, float& fDepth) const
    {
      fDepth = cINFINITY;

      const cVec3 A = origin;
      const cVec3 B = origin + (length * direction);

      const cVec3 n = rhs.GetNormal();
      const cVec3 I = n * rhs.GetIntercept();
      const cVec3 IA = A - I;
      const cVec3 IB = B - I;
      const cVec3 AB = B - A;

      const float_t nIA = n.DotProduct(IA);

      // Check if intersection
      if (nIA * (n.DotProduct(IB)) > 0.0f) return false;

      // Compute intersection
      const float_t t = -nIA / (AB.DotProduct(n));

      fDepth = (t * AB).GetLength();

      return true;
    }

    bool cRay3::CollideWithSphere(const cSphere& rhs, float& fDepth) const
    {
      fDepth = cINFINITY;

      const float_t r = rhs.GetRadius();
      const cVec3 originInObjectSpace = origin - rhs.GetPosition();

      // Compute A, B and C coefficients
      const float a = direction.DotProduct(direction);
      const float b = 2.0f * direction.DotProduct(originInObjectSpace);
      const float c = originInObjectSpace.DotProduct(originInObjectSpace) - (r * r);

      // Find discriminant
      const float disc = (b * b) - (4.0f * a * c);

      // if discriminant is negative there are no real roots, so return
      // false as ray misses sphere
      if (disc < 0.0f) return false;

      // Compute q as described above
      const float distSqrt = sqrtf(disc);
      float q;
      if (b < 0.0f) q = (-b - distSqrt) * 0.5f;
      else q = (-b + distSqrt) * 0.5f;

      // Compute t0 and t1
      float t0 = q / a;
      float t1 = c / q;

      // Make sure t0 is smaller than t1
      if (t0 > t1) {
          // if t0 is bigger than t1 swap them around
          std::swap(t0, t1);
      }

      // if t1 is less than zero, the object is in the ray's negative direction
      // and consequently the ray misses the sphere
      if (t1 < 0.0f) return false;

      // if t0 is less than zero, the intersection point is at t1
      if (t0 < 0.0f) {
        fDepth = t1;
        //fDepth -= (rhs.GetPosition() - origin).GetLength();
        return true;
      } else {
        // else the intersection point is at t0
        fDepth = t0;
        //fDepth -= (rhs.GetPosition() - origin).GetLength();
        return true;
      }

      return false;
    }


    bool cRay3::CollideWithAABB(const cAABB3& rhs, float& fDepth) const
    {
      fDepth = cINFINITY;

      ASSERT(false);

      return false;
    }

    bool cRay3::CollideWithTriangle(const cVec3& p0, const cVec3& p1, const cVec3& p2, float& fDepth) const
    {
      fDepth = cINFINITY;

      cPlane pl(p0, p1, p2);

      if (!CollideWithPlane(pl, fDepth)) return false;

      const cVec3 I = origin + (fDepth * direction);

      cVec3 N = (p1 - p0).CrossProduct(I - p0);
      cVec3 N2 = (p2 - p1).CrossProduct(I - p1);
      if ((N.DotProduct(N2)) < 0.0) return false;

      N2 = (p0 - p2).CrossProduct(I - p2);
      if ((N.DotProduct(N2)) < 0.0) return false;

      return true;
    }
  }
}


#ifdef BUILD_DEBUG

#include <spitfire/util/unittest.h>

class cSphereUnitTest : protected spitfire::util::cUnitTestBase
{
public:
  cSphereUnitTest() :
    cUnitTestBase(TEXT("cSphereUnitTest"))
  {
  }

  void Test()
  {
    spitfire::math::cSphere a;
    a.position.Set(3.0f, 1.0f, 1.0f);
    a.SetRadius(1.0f);

    spitfire::math::cSphere b;
    b.position.Set(7.0f, 1.0f, 1.0f);
    b.SetRadius(1.0f);

    float f = 0.0f;

    // Should be approximately 2.0f
    f = a.GetDistance(b);
    ASSERT(spitfire::math::IsApproximatelyEqual(f, 2.0f));

    // Should be approximately 4.0f
    f = a.GetDistanceCentreToCentre(b);
    ASSERT(spitfire::math::IsApproximatelyEqual(f, 4.0f));


    // Should not collide
    a.SetRadius(2.9f);
    ASSERT(!a.Collide(b));

    // Should collide
    a.SetRadius(3.5f);
    ASSERT(a.Collide(b));

    // Should collide
    a.SetRadius(4.0f);
    ASSERT(a.Collide(b));

    // Should collide
    a.SetRadius(4.5f);
    ASSERT(a.Collide(b));

    // Should collide
    a.SetRadius(100.0f);
    ASSERT(a.Collide(b));
  }
};

cSphereUnitTest gSphereUnitTest;
#endif // BUILD_DEBUG
