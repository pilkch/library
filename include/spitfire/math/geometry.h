#ifndef CGEOMETRY_H
#define CGEOMETRY_H

// Standard headers
#include <cassert>

// Spitfire headers
#include <spitfire/math/math.h>
#include <spitfire/math/cVec3.h>

namespace spitfire
{
  namespace math
  {
    inline float CalculateCircleCircumference(float fRadius)
    {
      return 2.0f * cPI * fRadius;
    }

    inline float CalculateArcAngleFromRadiusAndLengthOfArcCurve(float fRadius, float fLengthOfArcCurve)
    {
      // https://www.allmathtricks.com/circle-formulas-area-circumference/#Arc_and_sector_of_a_circle
      return (fLengthOfArcCurve * 180.0f) / (cPI * fRadius);
    }

    inline cVec2 CalculateCartesianCoordinate(float fAngleDegrees, float fRadius)
    {
      const float fAngleRadians = (cPI / 180.0f) * (fAngleDegrees - 90.0f);
      return cVec2(fRadius * cosf(fAngleRadians), fRadius * sinf(fAngleRadians));
    }


    inline float GetAngleBetweenPoints(const cVec2& point1, const cVec2& point2)
    {
      const float z_delta = (point1.y - point2.y);
      const float x_delta = (point1.x - point2.x);
      if (x_delta == 0.0f) {
        return (z_delta > 0.0f) ? 0.0f : 180.0f;
      }

      float angle = 0.0f;
      if (fabs(x_delta) < fabs(z_delta)) {
        angle = 90.0f - RadiansToDegrees((float)atan(z_delta / x_delta));
        if (x_delta < 0.0f) angle -= 180.0f;
      } else {
        angle = RadiansToDegrees((float)atan(x_delta / z_delta));
        if (z_delta < 0.0f) angle += 180.0f;
      }
      if (angle < 0.0f) angle += 360.0f;

      return angle;
    }

    inline float GetAngleDegreesFromNormal(const cVec2& normal)
    {
      //float fTargetAngleDegrees = 90.0f;
      //if (normal.x != 0.0f) fTargetAngleDegrees += RadiansToDegrees(atanf(normal.y / normal.x));
      return -RadiansToDegrees(atan2f(normal.x, normal.y));
    }

    inline cVec2 GetNormalFromAngleDegrees(float fAngleDegrees)
    {
      return (cVec2(cosf(DegreesToRadians(fAngleDegrees)), sinf(DegreesToRadians(fAngleDegrees)))).GetNormalised();
    }

    inline float GetDifferenceBetweenAngles(float fAngle1, float fAngle2)
    {
      const float result = (float)fmod(fAngle1 - fAngle2, 360.0f);
      if (result > 180.0f) return result - 360.0f;
      if (result < -180.0f) return result + 360.0f;

      return result;
    }


    // Find the closest point on a line
    // https://stackoverflow.com/a/47484153/1074390
    inline cVec3 GetClosestPointOnLine(const cVec3& a, const cVec3& b, const cVec3& point)
    {
      const cVec3 AB = b - a;
      const cVec3 AP = point - a;
      const float lengthSqrAB = AB.GetSquaredLength();
      const float t = (AP.x * AB.x + AP.y * AB.y + AP.z * AB.z) / lengthSqrAB;

      const float t_clamped = clamp(t, 0.0f, 1.0f);

      return a + (t_clamped * AB);
    }


    inline bool IsPointInCircle(const cVec2& point, const cVec2& centre, float radius)
    {
      const float distance = (point.x - centre.x) * (point.x - centre.x) + (point.y - centre.y) * (point.y - centre.y);
      return (distance <= radius * radius);
    }

    inline bool IsPointInTriangle(const cVec2& point, const cVec2& p0, const cVec2& p1, const cVec2& p2)
    {
      // https://jsfiddle.net/PerroAZUL/zdaY8/1/
      const float A = 0.5f * (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
      const float sign = A < 0.0f ? -1.0f : 1.0f;
      const float s = (p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * point.x + (p0.x - p2.x) * point.y) * sign;
      const float t = (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * point.x + (p1.x - p0.x) * point.y) * sign;

      return (s > 0) && (t > 0) && (s + t) < 2.0f * A * sign;
    }


    // This is for 2 dimensions
    class cCircle
    {
    public:
      // Position and size
      cVec2 position;
      float_t fRadius;
    };

    // This is for 3 dimensions
    class cSphere
    {
    public:
      friend class cCube;

      cSphere();

      void SetPosition(const cVec3& position);
      void SetRadius(float fRadius);

      bool Collide(const cSphere& rhs) const;
      float GetDistance(const cSphere& rhs) const;
      float GetDistanceCentreToCentre(const cSphere& rhs) const;

      const cVec3& GetPosition() const { return position; }

      float_t GetRadius() const { return fRadius; }
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

    inline void cSphere::SetPosition(const cVec3& _position)
    {
      position = _position;
    }

    inline void cSphere::SetRadius(float _fRadius)
    {
      fRadius = _fRadius;
    }

    inline bool cSphere::Collide(const cSphere& rhs) const
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


    // A cylinder with a dome on each end
    // NOTE: This is axis aligned, it is always standing on it's end
    class cCapsule
    {
    public:
      cCapsule();

      void SetBase(const cVec3& base);
      void SetTip(const cVec3& tip);
      void SetRadius(float fRadius);

      bool Collide(const cVec3& point) const;

      cVec3 base; // The internal centre point of the lower dome
      cVec3 tip;  // The internal centre point of the upper dome
      float_t fRadius;
    };

    inline cCapsule::cCapsule() :
      fRadius(1.0f)
    {
    }

    inline void cCapsule::SetBase(const cVec3& _base)
    {
      base = _base;
    }

    inline void cCapsule::SetTip(const cVec3& _tip)
    {
      tip = _tip;
    }

    inline void cCapsule::SetRadius(float _fRadius)
    {
      fRadius = _fRadius;
    }

    inline bool cCapsule::Collide(const cVec3& point) const
    {
      // https://arrowinmyknee.com/2021/03/15/some-math-about-capsule-collision/

      // Check if the point is inside an infinite cylinder centered around the capsule's base
      const cVec2 capsuleCentre2D(base.x, base.z);
      const cVec2 pointCentre2D(point.x, point.z);
      const float fDistance2D = (capsuleCentre2D - pointCentre2D).GetLength();
      if (fDistance2D <= fRadius) {
        // We are inside the infinite capsule, check if we are within the cylinder part
        if ((point.y >= base.y) && (point.y >= base.y)) return true;

        // Check if we are within one of the domes on the ends
        const float fDistanceToBase = (base - point).GetLength();
        if (fDistanceToBase < fRadius) return true;

        const float fDistanceToTip = (tip - point).GetLength();
        if (fDistanceToTip < fRadius) return true;
      }

      return false;
    }


    class cCube
    {
    public:
      cCube();
      //cCube(const cCube& rhs);
      cCube(const cSphere& rhs);

      void SetHalfWidth(float fHalfWidth);

      bool Collide(const cCube& rhs) const;
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



    class cAABB3;
    class cPlane;

    // Two points that specify a straight line
    class cLine3
    {
    public:
      cLine3();
      cLine3(const cVec3& origin, const cVec3& destination);

      void SetPoints(const cVec3& origin, const cVec3& destination);

      const cVec3& GetOrigin() const { return origin; }
      const cVec3& GetDestination() const { return destination; }
      cVec3 GetTangent() const;

    private:
      cVec3 origin;
      cVec3 destination;
    };

    // A point and direction to travel from that point, the ray itself is infinite length
    class cRay2
    {
    public:
      cRay2();

      void SetOriginAndDirection(const cVec2& origin, const cVec2& direction);
      void SetLength(float_t length);

      const cVec2& GetOrigin() const { return origin; }
      const cVec2& GetDirection() const { return direction; }
      const float_t& GetLength() const { return length; }

      //bool CollideWithPlane(const cPlane& rhs, float& fDepth) const;
      //bool CollideWithAABB(const cAABB2& rhs, float& fDepth) const;
      //bool CollideWithSphere(const cSphere& rhs, float& fDepth) const;
      //bool CollideWithTriangle(const cVec2& p0, const cVec2& p1, const cVec2& p2, float& fDepth) const;

    private:
      cVec2 origin;
      cVec2 direction;
      float_t length;
    };

    inline cRay2::cRay2() :
      origin(v2Zero),
      direction(v2Down),
      length(100000.0f) // Just a big enough number to get to the other side of the scene
    {
    }


    // A point and direction to travel from that point, the ray itself is infinite length
    class cRay3
    {
    public:
      cRay3();

      void SetOriginAndDirection(const cVec3& origin, const cVec3& direction);
      void SetLength(float_t length);

      const cVec3& GetOrigin() const { return origin; }
      const cVec3& GetDirection() const { return direction; }
      const float_t& GetLength() const { return length; }

      bool CollideWithPlane(const cPlane& rhs, float& fDepth) const;
      bool CollideWithAABB(const cAABB3& rhs, float& fDepth) const;
      bool CollideWithSphere(const cSphere& rhs, float& fDepth) const;
      bool CollideWithTriangle(const cVec3& p0, const cVec3& p1, const cVec3& p2, float& fDepth) const;

      cVec3 origin;
      cVec3 direction;
      float_t length;
    };

    inline cRay3::cRay3() :
      origin(v3Zero),
      direction(v3Down),
      length(100000.0f) // Just a big enough number to get to the other side of the scene
    {
    }




    class cAABB3
    {
    public:
#ifndef NDEBUG
      bool IsValid() const { return (cornerMin.x <= cornerMax.x) && (cornerMin.y <= cornerMax.y) && (cornerMin.z <= cornerMax.z); }
#endif

      //void Translate(const cVec3& translation);

      //void SetWidth(float width);
      //void SetHeight(float height);
      //void SetDepth(float depth);
      void SetMinMax(const cVec3& min, const cVec3& max);

      cVec3 GetCentre() const;
      void AddToVolume(const cVec3& point);
      void AddToVolume(const cLine3& line);
      void AddToVolume(const cAABB3& box);

      bool Intersect(const cVec3& point) const;
      bool Intersect(const cLine3& line) const;
      bool Intersect(const cAABB3& box) const;

      cVec3 cornerMin;
      cVec3 cornerMax;
    };

    inline cVec3 cAABB3::GetCentre() const
    {
      return cVec3(
        cornerMin.x + 0.5f * (cornerMax.x - cornerMin.x),
        cornerMin.y + 0.5f * (cornerMax.y - cornerMin.y),
        cornerMin.z + 0.5f * (cornerMax.z - cornerMin.z)
      );
    }

    inline void cAABB3::SetMinMax(const cVec3& min, const cVec3& max)
    {
      cornerMin = min;
      cornerMax = max;
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

    inline bool cAABB3::Intersect(const cAABB3& rhs) const
    {
      const cVec3 p1(GetCentre());
      const cVec3 p2(rhs.GetCentre());
      const cVec3 d(GetCentre() - rhs.GetCentre());

      if (fabs(d.x) < 0.5 * (p1.x + p2.x)) return true;
      if (fabs(d.y) < 0.5 * (p1.y + p2.y)) return true;
      if (fabs(d.z) < 0.5 * (p1.z + p2.z)) return true;

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
      cRectangle();
      cRectangle(float x, float y, float width, float height);

      float GetLeft() const { return x; }
      float GetRight() const { return x + width; }
      float GetTop() const { return y; }
      float GetBottom() const { return y + height; }
      float GetWidth() const { return width; }
      float GetHeight() const { return height; }
      float GetAspect() const { return GetWidth() / GetHeight(); }

      // Enlarge the rectangle if necessary so that another rectangle is
      // completely enclosed.
      const cRectangle& AddRectangleToVolume(const cRectangle& rhs);

      bool ContainsPoint(const spitfire::math::cVec2& point) const;

      float x;
      float y;
      float width;
      float height;
    };

    inline cRectangle::cRectangle() :
      x(0),
      y(0),
      width(0),
      height(0)
    {
    }

    inline cRectangle::cRectangle(float _x, float _y, float _width, float _height) :
      x(_x),
      y(_y),
      width(_width),
      height(_height)
    {
    }

    // Enlarge the cRectangle if necessary so that another rectangle is completely enclosed.
    inline const cRectangle& cRectangle::AddRectangleToVolume(const cRectangle& rhs)
    {
      x = std::min(x, rhs.x);
      x = std::max(x, rhs.x);
      const float fRightOfRHS = rhs.x + rhs.width;
      if (fRightOfRHS > x + width) width = fRightOfRHS - x;

      const float fBottomOfRHS = rhs.y + rhs.height;
      if (fBottomOfRHS > y + height) height = fBottomOfRHS - y;

      return *this;
    }




    // Bresenham Line Algorithm
    // http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    // The Bresenham line algorithm is an algorithm which determines which points in an n-dimensional raster should be plotted in order to form a close approximation to a straight line between two given points.
    // Wu's algorithm is an improvement in terms of quality (Supporting antialiasing), however for most purposes we actually prefer a hard edge, boolean "is the point in the line, yes/no".

    class cLineGenerator
    {
    public:
      void GenerateLine(const math::cVec2& start, const math::cVec2& end) { GenerateLine(int(start.x), int(start.y), int(end.x), int(end.y)); }
      void GenerateLine(int x0, int y0, int x1, int y1);

      const std::vector<cVec2>& GetPoints() const { return points; }

    private:
      std::vector<cVec2> points;
    };
  }
}

#endif // CGEOMETRY_H
