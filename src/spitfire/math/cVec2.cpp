#include <cstdlib>
#include <cmath>
#include <cassert>

#include <vector>
#include <limits>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cColour.h>

namespace spitfire
{
  namespace math
  {
    cVec2::cVec2(const float* rhs) :
      x(rhs[0]),
      y(rhs[1])
    {
    }

    void cVec2::Set(float newX, float newY)
    {
      x=newX;
      y=newY;
    }

    void cVec2::LoadZero(void)
    {
      x=0.0f;
      y=0.0f;
    }

    void cVec2::LoadOne(void)
    {
      x=1.0f;
      y=1.0f;
    }

    void cVec2::Normalise()
    {
      const float length = GetLength();

      if (length == 1 || length == 0)      //return if length is 1 or 0
        return;

      const float scalefactor = 1.0f / length;
      x *= scalefactor;
      y *= scalefactor;
    }

    void cVec2::SetLength(float length)
    {
      if (length == 1 || length == 0)      //return if length is 1 or 0
        return;

      const float scalefactor = 1.0f/length;
      x *= scalefactor;
      y *= scalefactor;
    }

    cVec2 cVec2::SetLength(float length) const
    {
      cVec2 result(*this);

      result.SetLength(length);

      return result;
    }

    cVec2 cVec2::GetNormalised() const
    {
      cVec2 result(*this);

      result.Normalise();

      return result;
    }

    float cVec2::GetMagnitude() const
    {
      return GetLength();
    }

    float cVec2::dot(const cVec2& rhs) const
    {
      return (x * rhs.x) + (y * rhs.y);
    }

    float cVec2::cross(const cVec2& rhs) const
    {
      return (x * rhs.y) - (y * rhs.x);
    }

    void cVec2::PackTo01()
    {
      (*this)=GetPackedTo01();
    }

    cVec2 cVec2::GetPackedTo01() const
    {
      cVec2 temp(*this);

      temp.Normalise();

      temp=temp*0.5f+cVec2(0.5f, 0.5f);

      return temp;
    }

    cVec2 cVec2::lerp(const cVec2 & v2, float factor)
    {
      cVec2 result;

      result=(*this)*factor + v2*(1.0f-factor);

      return result;
    }

    cVec2 cVec2::operator * (const float rhs) const
    {
      float newX = x * rhs;
      float newY = y * rhs;

      return cVec2(newX, newY);
    }

    bool cVec2::operator==(const cVec2 & rhs) const
    {
      if (x==rhs.x && y==rhs.y)
        return true;

      return false;
    }

    bool cVec2::operator!=(const cVec2 & rhs) const
    {
      return !((*this)==rhs);
    }

    void cVec2::operator*=(const float rhs)
    {
      x *= rhs;
      y *= rhs;
    }

    void cVec2::operator/=(const float rhs)
    {
      x /= rhs;
      y /= rhs;
    }

    cVec2& cVec2::operator=(const cVec3& rhs)
    {
      x = rhs.x;
      y = rhs.y;
      return *this;
    }
  }
}
