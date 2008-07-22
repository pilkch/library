#ifndef CGEOMETRY_H
#define CGEOMETRY_H

namespace breathe
{
  namespace math
  {
    class cSphere
    {
    public:
      cSphere();

      void SetRadius(float fRadius);

      bool Collide(const cSphere& rhs);
      float GetDistance(const cSphere& rhs) const;
      float GetDistanceCentreToCentre(const cSphere& rhs) const;

      // Position and size
      cVec3 p;
      float fRadius;
    };

    inline cSphere::cSphere() :
      fRadius(1.0f)
    {
    }

    inline void cSphere::SetRadius(float _fRadius)
    {
      fRadius = _fRadius;
    }

    inline bool cSphere::Collide(const cSphere& rhs)
    {
      return (GetDistance(rhs) < 0.0f);
    }

    inline float cSphere::GetDistance(const cSphere& rhs) const
    {
      float distance = GetDistanceCentreToCentre(rhs);
      float radiusPlusRadius = fRadius + rhs.fRadius;
      return distance - radiusPlusRadius;
    }

    inline float cSphere::GetDistanceCentreToCentre(const cSphere& rhs) const
    {
      cVec3 result;
      result.x = p.x - rhs.p.x;
      result.y = p.y - rhs.p.y;
      result.z = p.z - rhs.p.z;

      return result.GetLength();
    }
  }
}

#endif // CGEOMETRY_H
