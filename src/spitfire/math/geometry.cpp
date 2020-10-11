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

#include <spitfire/util/string.h>
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

    // *** cRay2

    void cRay2::SetOriginAndDirection(const cVec2& _origin, const cVec2& _direction)
    {
      origin = _origin;
      direction = _direction;
    }

    void cRay2::SetLength(float_t _length)
    {
      ASSERT(_length >= 0.0f);

      length = _length;
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


    // ** cRectangle

    bool cRectangle::ContainsPoint(const spitfire::math::cVec2& point) const
    {
      return (point.x > x) && (point.y > y) && (point.x < x + width) && (point.y < y + height);
    }


    // ** cLineGenerator

    void cLineGenerator::GenerateLine(int x0, int y0, int x1, int y1)
    {
      points.clear();

      // We want to try going left to right if possible, if the coordinates are going right to left, swap them
      if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
      }

      int Dx = x1 - x0;
      int Dy = y1 - y0;
      bool bIsSteep = (abs(Dy) >= abs(Dx)) != 0;
      if (bIsSteep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        // Recompute Dx, Dy after swap
        Dx = x1 - x0;
        Dy = y1 - y0;
      }
      int xstep = 1;
      if (Dx < 0) {
        xstep = -1;
        Dx = -Dx;
      }
      int ystep = 1;
      if (Dy < 0) {
        ystep = -1;
        Dy = -Dy;
      }
      int TwoDy = 2 * Dy;
      int TwoDyTwoDx = TwoDy - (2 * Dx); // (2 * Dy) - (2 * Dx)
      int error = TwoDy - Dx; // (2 * Dy) - Dx
      int y = y0;
      int xVisit = 0;
      int yVisit = 0;
      bool bHasVisitedLastPoint = false; // This tells us if we have been to x1, y1
      for (int x = x0; x != x1; x += xstep) {
        xVisit = x;
        yVisit = y;

        if (bIsSteep) std::swap(xVisit, yVisit);

        // If this is the last point set our flag
        if ((xVisit == x1) && (yVisit == y1)) bHasVisitedLastPoint = true;

        // Visit this point
        cVec2 p(static_cast<float>(xVisit), static_cast<float>(yVisit));
        points.push_back(p);

        // Next
        if (error > 0) {
          error += TwoDyTwoDx; // error += 2*Dy - 2*Dx;
          y += ystep;
        } else {
          error += TwoDy; // error += 2*Dy;
        }
      }

      // Make sure that we always visit the last point too
      if (!bHasVisitedLastPoint) {
        if (bIsSteep) std::swap(x1, y1);

        cVec2 p(static_cast<float>(x1), static_cast<float>(y1));
        points.push_back(p);
      }
    }

  }
}
