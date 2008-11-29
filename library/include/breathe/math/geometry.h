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

      const cVec3& GetPosition() const { return position; }

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





    // Two points that specify a straight line
    class cLine3
    {
      public:
        cLine3();
        cLine3(const cVec3& origin, const cVec3& destination);

        void SetPoints(const cVec3& origin, const cVec3& destination);

        const cVec3& GetOrigin() const { return origin; }
        const cVec3& GetDestination() const { return destination; }

      private:
        cVec3 origin;
        cVec3 destination;
    };

    // A point and direction to travel from that point, the ray itself is infinite length
    class cRay3
    {
    public:
      cRay3();
      cRay3(const cVec3& origin, const cVec3& destination);

      void SetOrigin(const cVec3& origin, const cVec3& destination);
      void SetDirection(const cVec3& direction);

      const cVec3& GetOrigin() const { return origin; }
      const cVec3& GetDirection() const { return direction; }

    private:
      cVec3 origin;
      cVec3 direction;
    };




    class cAABB3
    {
    public:
#ifndef NDEBUG
      bool IsValid() const { return (cornerMin.x <= cornerMax.x) && (cornerMin.y <= cornerMax.y) && (cornerMin.z <= cornerMax.z); }
#endif

      cVec3 GetCentre() const;
      void AddToVolume(const cVec3& point);
      void AddToVolume(const cLine3& line);
      void AddToVolume(const cAABB3& box);

      bool Intersect(const cVec3& point) const;
      bool Intersect(const cLine3& line) const;
      bool Intersect(const cAABB3& box) const;

    private:
      cVec3 cornerMin;
      cVec3 cornerMax;
    };

    inline cVec3 cAABB3::GetCentre() const
    {
      return cVec3(
        cornerMin.x + 0.5 * (cornerMax.x - cornerMin.x),
        cornerMin.y + 0.5 * (cornerMax.y - cornerMin.y),
        cornerMin.z + 0.5 * (cornerMax.z - cornerMin.z)
      );
    }

    inline void cAABB3::AddToVolume(const cVec3& point)
    {
      ASSERT(IsValid());

      if (point.x < cornerMin.x) cornerMin.x = point.x;
      if (point.y < cornerMin.y) cornerMin.y = point.y;
      if (point.z < cornerMin.z) cornerMin.z = point.z;

      if (point.x > cornerMax.x) cornerMax.x = point.x;
      if (point.y > cornerMax.y) cornerMax.y = point.y;
      if (point.z > cornerMax.z) cornerMax.z = point.z;
    }

    inline void cAABB3::AddToVolume(const cLine3& line)
    {
      ASSERT(IsValid());

      AddToVolume(line.GetOrigin());
      AddToVolume(line.GetDestination());
    }

    inline void cAABB3::AddToVolume(const cAABB3& box)
    {
      ASSERT(IsValid());
      ASSERT(box.IsValid());

      if (box.cornerMin.x < cornerMin.x) cornerMin.x = box.cornerMin.x;
      if (box.cornerMin.y < cornerMin.y) cornerMin.y = box.cornerMin.y;
      if (box.cornerMin.z < cornerMin.z) cornerMin.z = box.cornerMin.z;

      if (box.cornerMax.x > cornerMax.x) cornerMax.x = box.cornerMax.x;
      if (box.cornerMax.y > cornerMax.y) cornerMax.y = box.cornerMax.y;
      if (box.cornerMax.z > cornerMax.z) cornerMax.z = box.cornerMax.z;
    }

    inline bool cAABB3::Intersect(const cVec3& point) const
    {
      return (
        (point.x >= cornerMin.x) &&
        (point.y >= cornerMin.y) &&
        (point.z >= cornerMin.z) &&

        (point.x <= cornerMax.x) &&
        (point.y <= cornerMax.y) &&
        (point.z <= cornerMax.z)
      );
    }

    inline bool cAABB3::Intersect(const cLine3& line) const
    {
      return false;
    }

    inline bool cAABB3::Intersect(const cAABB3& box) const
    {
      return false;
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
