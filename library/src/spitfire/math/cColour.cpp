#include <cmath>

#include <vector>

// Spitfire Includes
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>

namespace spitfire
{
  namespace math
  {
    cColour::cColour() :
      r(0.0f),
      g(0.0f),
      b(0.0f),
      a(1.0f)
    {
    }

    cColour::cColour(float newR, float newG, float newB) :
      r(newR),
      g(newG),
      b(newB),
      a(1.0f)
    {
      Clamp();
    }

    cColour::cColour(float newR, float newG, float newB, float newA) :
      r(newR),
      g(newG),
      b(newB),
      a(newA)
    {
      Clamp();
    }

    cColour::cColour(const float* rhs)
    {
      r=*rhs;
      g=*(rhs+1);
      b=*(rhs+2);
      a=*(rhs+3);

      Clamp();
    }

    cColour::cColour(const cColour& rhs) :
      r(rhs.r),
      g(rhs.g),
      b(rhs.b),
      a(rhs.a)
    {
    }

    void cColour::Set(float newR, float newG, float newB, float newA)
    {
      r = newR;
      g = newG;
      b = newB;
      a = newA;

      Clamp();
    }

    void cColour::Clamp()
    {
      clamp(r);
      clamp(g);
      clamp(b);
      clamp(a);
    }

    cColour cColour::lerp(const cColour & c2, float factor)
    {
      cColour result;
      result = ((*this) * factor) + (c2 * (1.0f - factor));

      return result;
    }

    // http://en.wikipedia.org/wiki/Grayscale
    // From Wikipedia:
    // GreyScale = 0.3 R + 0.59 G + 0.11 B;

    float cColour::GetGreyScale() const
    {
      return (0.3f * r) + (0.59f * g) + (0.11f * b);
    }

    // http://en.wikipedia.org/wiki/Luma_(video)
    // http://en.wikipedia.org/wiki/Luminance_(relative)
    // From Wikipedia:
    // Y = 0.2126 R + 0.7152 G + 0.0722 B;
    float cColour::GetLuminance0To1() const
    {
      return (0.2126f * r) + (0.7152f * g) + (0.0722f * b);
    }

    cColour cColour::operator +(const cColour & rhs) const
    {
      cColour result(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);

      result.Clamp();

      return result;
    }

    cColour cColour::operator -(const cColour & rhs) const
    {
      cColour result(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);

      result.Clamp();

      return result;
    }

    cColour cColour::operator *(const cColour & rhs) const
    {
      cColour result(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);

      result.Clamp();

      return result;
    }

    cColour cColour::operator /(const cColour & rhs) const
    {
      cColour result(r / rhs.r, g / rhs.g, b / rhs.b, a / rhs.a);

      result.Clamp();

      return result;
    }

    cColour cColour::operator *(const float rhs) const
    {
      cColour result;

      result.r=r*rhs;
      result.g=g*rhs;
      result.b=b*rhs;
      result.a=a*rhs;

      result.Clamp();

      return result;
    }

    cColour cColour::operator /(const float rhs) const
    {
      cColour result;

      result.r=r/rhs;
      result.g=g/rhs;
      result.b=b/rhs;
      result.a=a/rhs;

      result.Clamp();

      return result;
    }

    bool cColour::operator ==(const cColour & rhs) const
    {
      if (r != rhs.r)
        return false;
      if (g != rhs.g)
        return false;
      if (b != rhs.b)
        return false;
      if (a != rhs.a)
        return false;

      return true;
    }

    bool cColour::operator !=(const cColour & rhs) const
    {
      return !((*this)==rhs);
    }

    cColour cColour::operator +=(const cColour & rhs)
    {
      (*this)=(*this)+rhs;

      return (*this);
    }

    cColour cColour::operator -=(const cColour & rhs)
    {
      (*this)=(*this)-rhs;

      return (*this);
    }

    cColour cColour::operator *=(const cColour & rhs)
    {
      (*this)=(*this)*rhs;

      return (*this);
    }

    cColour cColour::operator /=(const cColour & rhs)
    {
      (*this)=(*this)/rhs;

      return (*this);
    }

    cColour cColour::operator *=(const float rhs)
    {
      (*this)=(*this)*rhs;

      return (*this);
    }


    cColour cColour::operator /=(const float rhs)
    {
      (*this)=(*this)/rhs;

      return (*this);
    }
  }
}
