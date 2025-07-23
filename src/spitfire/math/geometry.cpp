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
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/geometry.h>

namespace spitfire
{
  namespace math
  {
    POLYGON_TYPE GetPolygonType(const Polygon3& polygon)
    {
      const size_t nPoints = polygon.points.size();

      if (nPoints < 3) {
        // Not enough points to even make a triangle
        return POLYGON_TYPE::INVALID;
      }


      if (nPoints > 3) {
        // Check most of the points
        for (size_t i = 0; i < nPoints - 2; i++) {
          if (IsAngleReflex(polygon.points[i].GetXZ(), polygon.points[i + 1].GetXZ(), polygon.points[i + 2].GetXZ())) {
            return POLYGON_TYPE::CONCAVE;
          }
        }

        // Now check the points that wrap around
        if (IsAngleReflex(polygon.points[nPoints - 2].GetXZ(), polygon.points[nPoints - 1].GetXZ(), polygon.points[0].GetXZ())) {
          return POLYGON_TYPE::CONCAVE;
        }

        if (IsAngleReflex(polygon.points[nPoints - 1].GetXZ(), polygon.points[0].GetXZ(), polygon.points[1].GetXZ())) {
          return POLYGON_TYPE::CONCAVE;
        }
      }

      return POLYGON_TYPE::CONVEX;
    }

    WINDING_ORDER GetPolygonWindingOrder(const Polygon3& polygon)
    {
      if (polygon.points.size() < 3) {
        return WINDING_ORDER::INVALID;
      }

      // Sum up all the edges with (x2 − x1) * (y2 + y1), if the result is positive it is clockwise, otherwise counter clockwise
      // https://stackoverflow.com/a/1165943

      float fRunningTotal = 0.0f;

      for (size_t i = 0; i < polygon.points.size() - 1; i++) {
        fRunningTotal += (polygon.points[i + 1].x - polygon.points[i].x) * (polygon.points[i + 1].z + polygon.points[i].z);
      }

      // Add up the final point linking back to the start
      const size_t last = polygon.points.size() - 1;
      fRunningTotal += (polygon.points[0].x - polygon.points[last].x) * (polygon.points[0].z + polygon.points[last].z);

      return (fRunningTotal > 0.0f) ? WINDING_ORDER::COUNTER_CLOCKWISE : WINDING_ORDER::CLOCKWISE;
    }


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

    bool cRay3::CollideWithPlaneBetterButRequiresChangingPlaneClass(const cVec3& plane_origin, const cVec3& plane_normal, float& fOutDepth) const
    {
      // https://davidjcobb.github.io/articles/ray-plane-intersection
      const float denom = dot(plane_normal, direction);
      if (denom > cEPSILON || denom < -cEPSILON) {
        const float  Hd = dot(plane_origin - origin, plane_normal) / denom;
        if (Hd >= 0.0f) {
          fOutDepth = Hd;
          return true;
        }
      }

      return false;
    }

    bool cRay3::CollideWithDisc(const cVec3& disc_origin, const cVec3& disc_normal, float radius, float& fOutDepth) const
    {
      // https://davidjcobb.github.io/articles/ray-disc-intersection

      // Collide with the disc as a plane
      if (!CollideWithPlaneBetterButRequiresChangingPlaneClass(disc_origin, disc_normal, fOutDepth)) {
        return false;
      }

      // Now check that the collision point is within the disc radius
      const cVec3 Hp = origin + direction * fOutDepth;
      const cVec3 Dd = Hp - disc_origin;
      if (dot(Dd, Dd) > radius * radius) {
        return false;
      }

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

    // Given coefficients in a quadratic equation, this function gives you the roots 
    // and returns the number of roots. If there is only one root, then both root 
    // variables are set to the same value.
    //
    int CalculateQuadraticRoots(const float a, const float b, const float c, float& fOutRootLower, float& fOutRootUpper)
    {
      const float discriminant = (b * b) - (4.0 * a * c);
      if (discriminant > cEPSILON) {
        float b_term = b < cEPSILON ? -b + sqrt(discriminant) : -b - sqrt(discriminant);

        fOutRootLower = b_term / (2.0 * a); // Quadratic formula
        fOutRootUpper = (2.0 * c) / b_term; // Reverse quadratic formula

        if (fOutRootLower > fOutRootUpper) {
          std::swap(fOutRootLower, fOutRootUpper); // Use of both formulae, plus this, avoids catastrophic cancellation due to floating-point limits
        }

        return 2;
      } else if (discriminant > -cEPSILON && discriminant <= cEPSILON) {
        fOutRootLower = -(b / 2.0 * a);
        fOutRootUpper = fOutRootLower;
        return 1;
      }

      fOutRootLower = NAN;
      fOutRootUpper = NAN;

      return 0;
    }

    bool RayCylinderIntersection(const cRay3& ray, const cVec3& p0, const cVec3& p1, float cylinder_radius, float& hit_distance
    )
    {
      // https://davidjcobb.github.io/articles/ray-cylinder-intersection

      // First, identify intersections between a line and an infinite cylinder. An infinite 
      // cylinder has no base and extends in both directions.
      const cVec3 Rl = ray.origin - p1;        // Ray origin local to centerpoint
      const cVec3 Cs = p0 - p1;                // Cylinder spine
      const float Ch = length(Cs);             // Cylinder height
      const cVec3 Ca = Cs / Ch;                // Cylinder axis

      const float Ca_dot_Rd = dot(Ca, ray.direction);
      const float Ca_dot_Rl = dot(Ca, Rl);
      const float Rl_dot_Rl = dot(Rl, Rl);

      const float a = 1 - (Ca_dot_Rd * Ca_dot_Rd);
      const float b = 2 * (dot(ray.direction, Rl) - Ca_dot_Rd * Ca_dot_Rl);
      const float c = Rl_dot_Rl - Ca_dot_Rl * Ca_dot_Rl - (cylinder_radius * cylinder_radius);

      float hit_near;
      float hit_away;
      auto  count = CalculateQuadraticRoots(a, b, c, hit_near, hit_away);
      if (count == 0) {
        // There is no intersection between a line (i.e. a "double-sided" ray) and the 
        // infinite cylinder that matches our finite cylinder. This means that we cannot 
        // be hitting any part of the cylinder: if we were hitting the base from the 
        // inside, for example, then the "back of our ray" would be hitting the upper 
        // part of the infinite cylinder.
        return false;
      }

      //
      // Now, we need to take our intersection points and ensure that they lie on the 
      // surface of a finite cylinder. If one of them is past the edges of the finite 
      // cylinder, then we need to check for a valid intersection with the endcaps.
      //
      if (count > 2) {
        std::cerr<<"Error: Incorrect number of quadtratic roots"<<std::endl;
        return false;
      }

      bool valid1 = true;
      bool valid2 = true;

      cVec3 Hp1 = ray.origin + ray.direction * hit_near;
      const cVec3 Hp2 = ray.origin + ray.direction * hit_away;
      float Ho1 = dot(p0 - Hp1, Ca); // height offset
      const float Ho2 = dot(p0 - Hp2, Ca);
      int valid_count = count;
      if (hit_near < 0.0 || Ho1 < 1.0e-8 || Ho1 > Ch) {
        valid1 = false;
        --valid_count;
      }
      if (hit_away < 0.0 || Ho2 < 1.0e-8 || Ho2 > Ch) {
        valid2 = false;
        if (count > 1) {
          --valid_count;
        }
      }

      if (valid_count == 0) {
        // The ray never hits the bounded cylinder's curved surface. If we're looking 
        // along the cylinder's axis -- whether from inside or outside -- then the ray 
        // could still hit an endcap.
        // 
        // Let's project the ray origin onto the cylinder's axis, and figure out which 
        // endcap we're nearer to. (Well, actually, we already have that value: it's Ca_dot_Rl.
        if (Ca_dot_Rl <= 0.0) { // above
          valid1 = ray.CollideWithDisc(p0, Ca, cylinder_radius, hit_near);
        } else if (Ca_dot_Rl >= Ch) { // below
          valid1 = ray.CollideWithDisc(p1, Ca, cylinder_radius, hit_away);
        } else {
          // Inside, don't count as a collision
          return false;
        }

        if (valid1) {
          hit_distance = hit_near;
          return true;
        }

        return false;
      }
      if (valid_count == 1) {
        // The ray hits the cylinder's curved surface only once. This can only happen under 
        // two cases: the ray originates from inside the cylinder, and points outward; or 
        // the ray passes through the bounded cylinder once and then through an endcap.
        if (valid2) {
          Hp1 = Hp2;
          Ho1 = Ho2;
          valid1   = true;
          hit_near = hit_away;
        }

        float disc_near;
        float disc_away;
        bool disc1 = ray.CollideWithDisc(p0, Ca, cylinder_radius, disc_near);
        const bool disc2 = ray.CollideWithDisc(p1, Ca, cylinder_radius, disc_away);
        if (disc1) {
          if (disc2) {
            if (disc_away < disc_near) {
              disc_near = disc_away;
            }
          }
        } else if (disc2) {
          disc_near = hit_away;
          disc1 = disc2;
        }

        if (disc1) {
          if (disc_near < hit_near) {
            hit_distance = disc_near;
            return true;
          }
        } else {
          // Inside, don't count as a collision
          return false;
        }
      }

      hit_distance = std::min(hit_near, hit_away);
      return true;
    }

    bool cRay3::CollideWithCylinder(const cVec3& p0, const cVec3& p1, float fRadius, cVec3& outCollision) const
    {
      float fDepth = 0.0f;

      if (RayCylinderIntersection(*this, p0, p1, fRadius, fDepth)) {
        outCollision = origin + (fDepth * direction);
        return true;
      }

      return false;
    }

    bool Raycast(const cRay3& ray, const cAABB3& aabb, float& fDepth)
    {
      fDepth = cINFINITY;

      const cVec3 dirNormalised(ray.direction.GetNormalised());

      const float t1 = (aabb.cornerMin.x - ray.origin.x) / dirNormalised.x;
      const float t2 = (aabb.cornerMax.x - ray.origin.x) / dirNormalised.x;
      const float t3 = (aabb.cornerMin.y - ray.origin.y) / dirNormalised.y;
      const float t4 = (aabb.cornerMax.y - ray.origin.y) / dirNormalised.y;
      const float t5 = (aabb.cornerMin.z - ray.origin.z) / dirNormalised.z;
      const float t6 = (aabb.cornerMax.z - ray.origin.z) / dirNormalised.z;

      const float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
      const float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

      // if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
      if (tmax < 0.0f) {
        fDepth = 0.0f;
        return false;
      }

      // if tmin > tmax, ray doesn't intersect AABB
      if (tmin > tmax) {
        fDepth = 0.0f;
        return false;
      }

      if (tmin < 0.0f) {
        fDepth = tmax;
        return true;
      }

      fDepth = tmin;
      return true;
    }

    bool cRay3::CollideWithAABB(const cAABB3& rhs, float& fOutDepth) const
    {
      // NOTE: This doesn't check the length, should the ray even have a length?
      return Raycast(*this, rhs, fOutDepth);
    }

    bool cRay3::CollideWithTriangle(const cVec3& p0, const cVec3& p1, const cVec3& p2, float& fOutDepth) const
    {
      fOutDepth = cINFINITY;

      const cPlane plane(p0, p1, p2);

      if (!CollideWithPlane(plane, fOutDepth)) return false;

      const cVec3 I = origin + (fOutDepth * direction);

      cVec3 N = (p1 - p0).CrossProduct(I - p0);
      cVec3 N2 = (p2 - p1).CrossProduct(I - p1);
      if ((N.DotProduct(N2)) < 0.0) return false;

      N2 = (p0 - p2).CrossProduct(I - p2);
      if ((N.DotProduct(N2)) < 0.0) return false;

      return true;
    }

    bool cRay3::CollideRayWithTriangles(const std::vector<cVec3>& collisionTrianglePoints, cVec3& outCollision) const
    {
      float fClosestDepth = cINFINITY;
      float fDepth = cINFINITY;
      bool found = false;

      const size_t triangles = collisionTrianglePoints.size();
      for (size_t i = 0; i < triangles; i += 3) {
        if (CollideWithTriangle(collisionTrianglePoints[i], collisionTrianglePoints[i + 1], collisionTrianglePoints[i + 2], fDepth) && (fDepth < fClosestDepth)) {
          // We found a (closer) collision
          outCollision = origin + (fDepth * direction);
          fClosestDepth = fDepth;
          found = true;
        }
      }

      return found;
    }

    bool cRay3::CollideRayWithOctreeNode(const cOctree* pOctree, cVec3& outCollision) const
    {
      if (pOctree == nullptr) {
        return false;
      }

      const float fHalfWidth = pOctree->m_Width;
      const cVec3 min(pOctree->m_vCenter - cVec3(fHalfWidth, fHalfWidth, fHalfWidth));
      const cVec3 max(pOctree->m_vCenter + cVec3(fHalfWidth, fHalfWidth, fHalfWidth));

      cAABB3 aabb;
      aabb.SetMinMax(min, max);

      float fClosestDepth = cINFINITY;
      float fDepth = cINFINITY;

      if (!CollideWithAABB(aabb, fDepth)) {
        return false;
      }


      if (pOctree->IsSubDivided()) {
        // Recurse to the bottom of these nodes and draw the end node's vertices
        // Like creating the octree, we need to recurse through each of the 8 nodes.
        return (
          CollideRayWithOctreeNode(pOctree->m_pOctreeNodes[TOP_LEFT_FRONT], outCollision) ||
          CollideRayWithOctreeNode(pOctree->m_pOctreeNodes[TOP_LEFT_BACK], outCollision) ||
          CollideRayWithOctreeNode(pOctree->m_pOctreeNodes[TOP_RIGHT_BACK], outCollision) ||
          CollideRayWithOctreeNode(pOctree->m_pOctreeNodes[TOP_RIGHT_FRONT], outCollision) ||
          CollideRayWithOctreeNode(pOctree->m_pOctreeNodes[BOTTOM_LEFT_FRONT], outCollision) ||
          CollideRayWithOctreeNode(pOctree->m_pOctreeNodes[BOTTOM_LEFT_BACK], outCollision) ||
          CollideRayWithOctreeNode(pOctree->m_pOctreeNodes[BOTTOM_RIGHT_BACK], outCollision) ||
          CollideRayWithOctreeNode(pOctree->m_pOctreeNodes[BOTTOM_RIGHT_FRONT], outCollision)
        );
      }


      bool found = false;

      // Make sure we have valid vertices assigned to this node
      if (pOctree->m_pVertices != nullptr) {
        cVec3* pVertices = pOctree->m_pVertices;

        // Go through all of the vertices (the number of triangles * 3)
        const size_t nVertices = pOctree->GetTriangleCount() * 3;
        for (size_t i = 0; i < nVertices; i += 3) {
          if (CollideWithTriangle(pVertices[i], pVertices[i + 1], pVertices[i + 2], fDepth) && (fDepth < fClosestDepth)) {
            // We found a (closer) collision
            outCollision = origin + (fDepth * direction);
            fClosestDepth = fDepth;
            found = true;
          }
        }
      }

      return found;
    }

    bool cRay3::CollideRayWithOctree(const cOctree& octree, cVec3& outCollision) const
    {
      return CollideRayWithOctreeNode(&octree, outCollision);
    }



    // ** cRectangle

    bool cRectangle::ContainsPoint(const cVec2& point) const
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
