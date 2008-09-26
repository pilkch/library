#ifndef CGEOMETRY_H
#define CGEOMETRY_H

namespace breathe
{
  namespace math
  {
    class cSphere
    {
    public:
      friend class cCube;

      cSphere();

      void SetRadius(float fRadius);

      bool Collide(const cSphere& rhs);
      float GetDistance(const cSphere& rhs) const;
      float GetDistanceCentreToCentre(const cSphere& rhs) const;

      float_t GetHalfWidth() const { return fRadius; }
      float_t GetWidth() const { return fRadius + fRadius; }

      float_t GetHalfHeight() const { return fRadius; }
      float_t GetHeight() const { return fRadius + fRadius; }

      float_t GetHalfDepth() const { return fRadius; }
      float_t GetDepth() const { return fRadius + fRadius; }

      // Position and size
      cVec3 position;
      float_t fRadius;
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
      return (position - rhs.position).GetLength();
    }



    class cCube
    {
    public:
      cCube();
      //cCube(const cCube& rhs);
      cCube(const cSphere& rhs);

      void SetHalfWidth(float fHalfWidth);

      bool Collide(const cCube& rhs);
      float GetDistance(const cCube& rhs) const;
      float GetDistanceCentreToCentre(const cCube& rhs) const;
      float GetDistanceCentreToCentre(const cSphere& rhs) const;


      float_t GetHalfWidth() const { return fHalfWidth; }
      float_t GetWidth() const { return fHalfWidth + fHalfWidth; }

      float_t GetHalfHeight() const { return fHalfWidth; }
      float_t GetHeight() const { return fHalfWidth + fHalfWidth; }

      float_t GetHalfDepth() const { return fHalfWidth; }
      float_t GetDepth() const { return fHalfWidth + fHalfWidth; }

      // Position and size
      cVec3 position;
      float fHalfWidth;
    };

    inline cCube::cCube() :
      fHalfWidth(1.0f)
    {
    }

    //inline cCube::cCube(const cCube& rhs)
    //{
    //  position = rhs.position;
    //  fHalfWidth = rhs.fHalfWidth;
    //}

    inline cCube::cCube(const cSphere& rhs)
    {
      position = rhs.position;
      fHalfWidth = rhs.fRadius;
    }

    inline void cCube::SetHalfWidth(float _fHalfWidth)
    {
      fHalfWidth = _fHalfWidth;
    }


    typedef cCube cBox;


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
