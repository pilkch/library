#ifndef CPLANE_H
#define CPLANE_H

#include <spitfire/math/cVec3.h>

namespace spitfire
{
  namespace math
  {
    // Constants for ClassifyPoint()
    enum PLANE_POINT_CLASSIFICATION
    {
      PLANE_POINT_CLASSIFICATION_POINT_ON_PLANE,
      PLANE_POINT_CLASSIFICATION_POINT_IN_FRONT_OF_PLANE,
      PLANE_POINT_CLASSIFICATION_POINT_BEHIND_PLANE
    };

    class cPlane
    {
    public:
      cPlane();
      cPlane(const cVec3& newNormal, float newIntercept);
      cPlane(const cVec3& p0, const cVec3& p1, const cVec3& p2); // Plane from a triangle
      cPlane(const cPlane& rhs);

      void SetNormal(const cVec3& rhs) { normal = rhs; }
      void SetIntercept(float newIntercept) { intercept = newIntercept; }
      void SetFromPoints(const cVec3& p0, const cVec3& p1, const cVec3& p2);

      void CalculateIntercept(const cVec3& pointOnPlane) { intercept = -normal.DotProduct(pointOnPlane); }

      void Normalise();

      cVec3 GetReflected(const cVec3& v) const;

      const cVec3& GetNormal() const { return normal; }
      float GetIntercept() const { return intercept; }

      // Find point of intersection of 3 cPlanes
      bool Intersect3(const cPlane& p2, const cPlane& p3, cVec3& result) const;

      bool IsFrontFacingToDirection(const cVec3& direction) const;

      // This is the same as GetDistanceToPoint
      //float GetSignedDistanceToPoint(const cVec3& point) const;

      float GetDistanceToPoint(const cVec3& point) const;
      PLANE_POINT_CLASSIFICATION ClassifyPoint(const cVec3& point) const;

      cPlane lerp(const cPlane& p2, float factor) const;

      // Operators
      bool operator==(const cPlane& rhs) const;
      bool operator!=(const cPlane& rhs) const;

      // Unary operators
      cPlane operator-(void) const { return cPlane(-normal, intercept); }
      cPlane operator+(void) const { return (*this); }

    private:
      cVec3 normal;  // X.N + intercept = 0
      float intercept; // Also known as d
    };
  }
}

#endif //CPLANE_H
