#pragma once

#include <cmath>

namespace spitfire
{
  namespace math
  {
    class cVec3;

    class cVec2
    {
    public:
      inline cVec2() : x(0), y(0), u(x), v(y) {}
      explicit cVec2(const float* rhs);
      inline cVec2(float newX, float newY) : x(newX), y(newY), u(x), v(y) {}
      inline cVec2(const cVec2& rhs) : x(rhs.x), y(rhs.y), u(x), v(y) {}

      void Set(float newX, float newY); //set member variables

      //Accessors kept for compatibility
      void SetX(float newX) { x = newX; }
      void SetY(float newY) { y = newY; }

      float GetX() const { return x; } //public accessor functions
      float GetY() const { return y; } //inline, const

      void LoadZero(void);
      void LoadOne(void); //fill with (1, 1, 1)

      void Normalise();
      cVec2 GetNormalised() const;

      void SetLength(float length);
      cVec2 SetLength(float length) const;

      float GetLength() const
      { return sqrtf((x*x)+(y*y)); }

      float GetSquaredLength() const
      { return (x*x)+(y*y); }

      float GetMagnitude() const;

      float dot(const cVec2& rhs) const;
      float cross(const cVec2& rhs) const;

      //pack to [0,1] for color
      void PackTo01();
      cVec2 GetPackedTo01() const;

      // Linear interpolation
      cVec2 lerp(const cVec2& v2, float factor);

      //overloaded operators
      //binary operators
      cVec2 operator+(const cVec2 & rhs) const
      { return cVec2(x + rhs.x, y + rhs.y); }

      cVec2 operator-(const cVec2 & rhs) const
      { return cVec2(x - rhs.x, y - rhs.y); }

      cVec2 operator*(const float rhs) const;

      friend cVec2 operator*(float rhs, const cVec2& v)
      {
        return cVec2(rhs * v.x, rhs * v.y);
      }

      cVec2 operator/(const float rhs) const
      { return (rhs==0) ? cVec2(0.0f, 0.0f) : cVec2(x / rhs, y / rhs); }

      cVec2 operator/(const cVec2 & rhs) const
      { return cVec2(x / rhs.x, y / rhs.y); }

      cVec2 operator*(const cVec2 & rhs) const
      { return cVec2(x * rhs.x, y * rhs.y); }

      bool operator==(const cVec2 & rhs) const;
      bool operator!=(const cVec2 & rhs) const;

      //self-add etc
      void operator+=(const cVec2 & rhs) { x +=rhs.x; y +=rhs.y; }
      void operator-=(const cVec2 & rhs) { x -=rhs.x; y -=rhs.y; }
      void operator*=(const cVec2& rhs) { x *= rhs.x; y *= rhs.y; }
      void operator/=(const cVec2& rhs) { x /= rhs.x; y /= rhs.y; }

      void operator*=(const float rhs);
      void operator/=(const float rhs);

      //unary operators
      cVec2 operator-(void) const { return cVec2(-x, -y); }
      cVec2 operator+(void) const { return *this; }

      // Cast to pointer to a (float *) for glVertex3fv etc
      const float* GetPointerConst() const { return (const float*)this; }

      cVec2& operator=(const cVec2& rhs) { x = rhs.x; y = rhs.y; return *this; };
      cVec2& operator=(const cVec3& rhs);

      // We do this so that x maps to u and y maps to v
      // for use in texture mapping as well as position coords
      // TODO: Maybe don't do this? Provide u() and v() functions instead?
      float x;
      float y;

      float& u;
      float& v;
    };

    const cVec2 v2Zero(0.0f, 0.0f);

    const cVec2 v2Up(0.0f, 1.0f);
    const cVec2 v2Down(0.0f, -1.0f);
    const cVec2 v2Left(1.0f, 0.0f);
    const cVec2 v2Right(-1.0f, 0.0f);


    // GLSL/glm style helper functions
    inline constexpr float length(const cVec2& v) { return v.GetLength(); }
    inline constexpr float distance(const cVec2& p0, const cVec2& p1) { return (p1 - p0).GetLength(); }
    inline constexpr cVec2 normalize(const cVec2& a) { return a.GetNormalised(); }
    inline constexpr float cross(const cVec2& a, const cVec2& b) { return a.cross(b); }
    inline constexpr float dot(const cVec2& a, const cVec2& b) { return a.dot(b); }
  }
}
