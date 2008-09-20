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
      const cVec3 result(p.x - rhs.p.x, p.y - rhs.p.y, p.z - rhs.p.z);
      return result.GetLength();
    }


    class cBox
    {
    public:
      cBox();

      void SetHalfWidth(float fHalfWidth);

      bool Collide(const cBox& rhs);
      float GetDistance(const cBox& rhs) const;
      float GetDistanceCentreToCentre(const cBox& rhs) const;
      float GetDistanceCentreToCentre(const cSphere& rhs) const;

      // Position and size
      cVec3 p;
      float fHalfWidth;
    };

    inline cBox::cBox() :
      fHalfWidth(1.0f)
    {
    }

    inline void cBox::SetHalfWidth(float _fHalfWidth)
    {
      fHalfWidth = _fHalfWidth;
    }


    // -halfwidth, +halfwidth        +halfwidth, +halfwidth
    //
    //                         x, y
    //
    // -halfwidth, -halfwidth        +halfwidth, -halfwidth

    class cRectangle
    {
    public:
      cRectangle(float x, float y, float width, float height);

      float GetWidth() const { return right - x; }
      float GetHeight() const { return bottom - y; }
      float GetAspect() const { return GetWidth() / GetHeight(); }

      // Enlarge the rectangle if necessary so that another rectangle is
      // completely enclosed.
      const cRectangle& AddRectangleToVolume(const cRectangle& rhs);

    private:
      float x;
      float y;
      float right;
      float bottom;
    };


    inline cRectangle::cRectangle(float _x, float _y, float _width, float _height) :
      x(_x),
      y(_y),
      right(_x + _width),
      bottom(_y + _height)
    {
    }

    // Enlarge the cRectangle if necessary so that another rectangle is completely enclosed.
    inline const cRectangle& cRectangle::AddRectangleToVolume(const cRectangle& rhs)
    {
      x = std::min(x, rhs.x);
      x = std::max(x, rhs.x);
      right = std::max(right, rhs.right);
      bottom = std::min(bottom, rhs.bottom);
      return *this;
    }
  }
}

#endif // CGEOMETRY_H
