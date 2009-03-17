#ifndef CPLANE_H
#define CPLANE_H

namespace spitfire
{
  namespace math
  {
    //constants for ClassifyPoint()
    const int POINT_ON_cPlane=0;
    const int POINT_IN_FRONT_OF_cPlane=1;
    const int POINT_BEHIND_cPlane=2;

    class cPlane
    {
    public:
      cPlane(cVec3 newNormal=cVec3(0.0f, 0.0f, 0.0f), float newIntercept=0.0f)
        : normal(newNormal), intercept(newIntercept)
      {}
      cPlane(const cPlane& rhs);
      ~cPlane() {}

      void SetNormal(const cVec3& rhs) { normal=rhs; }
      void SetIntercept(float newIntercept) { intercept=newIntercept; }
      void SetFromPoints(const cVec3& p0, const cVec3 & p1, const cVec3 & p2);

      void CalculateIntercept(const cVec3& pointOnPlane) { intercept = -normal.DotProduct(pointOnPlane); }

      void Normalise();

      cVec3 GetReflected(cVec3 v) const;

      cVec3 GetNormal() const { return normal; }
      float GetIntercept() const { return intercept; }

      //find point of intersection of 3 cPlanes
      bool Intersect3(const cPlane& p2, const cPlane& p3, cVec3& result) const;

      float DistancePoint(const cVec3& point) const;
      int ClassifyPoint(const cVec3& point) const;

      cPlane lerp(const cPlane& p2, float factor) const;

      //operators
      bool operator==(const cPlane& rhs) const;
      bool operator!=(const cPlane& rhs) const;

      //unary operators
      cPlane operator-(void) const { return cPlane(-normal, intercept); }
      cPlane operator+(void) const { return (*this); }

      //member variables
      cVec3 normal;  //X.N+intercept=0
      float intercept;



      //other way of doing things
      float equation[4];
      cVec3 origin;

      cPlane(const cVec3& origin, const cVec3& normal);
      cPlane(const cVec3& p1, const cVec3& p2, const cVec3& p3);

      bool IsFrontFacingTo(const cVec3& direction) const;
      double SignedDistanceTo(const cVec3& point) const;
    };
  }
}

#endif //CPLANE_H
