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
    }

    cColour::cColour(float newR, float newG, float newB, float newA) :
      r(newR),
      g(newG),
      b(newB),
      a(newA)
    {
    }

    cColour::cColour(const float* rhs)
    {
      r = rhs[0];
      g = rhs[1];
      b = rhs[2];
      a = rhs[3];
    }

    cColour::cColour(const cColour& rhs) :
      r(rhs.r),
      g(rhs.g),
      b(rhs.b),
      a(rhs.a)
    {
    }

    void cColour::SetRGB(float newR, float newG, float newB)
    {
      r = newR;
      g = newG;
      b = newB;
      a = 1.0f;
    }

    void cColour::SetRGBA(float newR, float newG, float newB, float newA)
    {
      r = newR;
      g = newG;
      b = newB;
      a = newA;
    }

    void cColour::Clamp()
    {
      r = clamp(r, 0.0f, 1.0f);
      g = clamp(g, 0.0f, 1.0f);
      b = clamp(b, 0.0f, 1.0f);
      a = clamp(a, 0.0f, 1.0f);
    }

    cColour cColour::lerp(const cColour& c2, float factor) const
    {
      return ((*this) * factor) + (c2 * (1.0f - factor));
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

    cColour cColour::operator +(const cColour& rhs) const
    {
      return cColour(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
    }

    cColour cColour::operator -(const cColour& rhs) const
    {
      return cColour(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
    }

    cColour cColour::operator *(const cColour& rhs) const
    {
      cColour result(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);

      return result;
    }

    cColour cColour::operator /(const cColour& rhs) const
    {
      cColour result(r / rhs.r, g / rhs.g, b / rhs.b, a / rhs.a);

      return result;
    }

    cColour cColour::operator *(const float rhs) const
    {
      cColour result;

      result.r = r * rhs;
      result.g = g * rhs;
      result.b = b * rhs;
      result.a = a * rhs;

      return result;
    }

    cColour cColour::operator /(const float rhs) const
    {
      cColour result;

      const float fOneOverRhs = rhs / 1.0f;
      result.r = r * fOneOverRhs;
      result.g = g * fOneOverRhs;
      result.b = b * fOneOverRhs;
      result.a = a * fOneOverRhs;

      return result;
    }

    bool cColour::operator ==(const cColour& rhs) const
    {
      return ((r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a));
    }

    bool cColour::operator !=(const cColour& rhs) const
    {
      return !((*this) == rhs);
    }

    cColour cColour::operator +=(const cColour& rhs)
    {
      (*this) = (*this) + rhs;

      return (*this);
    }

    cColour cColour::operator -=(const cColour& rhs)
    {
      (*this) = (*this) - rhs;

      return (*this);
    }

    cColour cColour::operator *=(const cColour& rhs)
    {
      (*this) = (*this) * rhs;

      return (*this);
    }

    cColour cColour::operator /=(const cColour& rhs)
    {
      (*this) = (*this) / rhs;

      return (*this);
    }

    cColour cColour::operator *=(const float rhs)
    {
      (*this) = (*this) * rhs;

      return (*this);
    }


    cColour cColour::operator /=(const float rhs)
    {
      (*this) = (*this) / rhs;

      return (*this);
    }


    // Hue, saturation, luminance
    // http://en.wikipedia.org/wiki/HSL_and_HSV

    void cColour::GetHSLFromRGB(float& fHue, float& fSaturation, float& fLuminance) const
    {
      float fmin = min(min(r, g), b);    //Min. value of RGB
      float fmax = max(max(r, g), b);    //Max. value of RGB
      float delta = fmax - fmin;             //Delta RGB value

      fLuminance = (fmax + fmin) / 2.0; // Luminance

      if (delta == 0.0) { //This is a gray, no chroma...
        fHue = 0.0;  // Hue
        fSaturation = 0.0;  // Saturation
      } else {            //Chromatic data...
        if (fLuminance < 0.5)
          fSaturation = delta / (fmax + fmin); // Saturation
        else
          fSaturation = delta / (2.0 - fmax - fmin); // Saturation

        float deltaR = (((fmax - r) / 6.0) + (delta / 2.0)) / delta;
        float deltaG = (((fmax - g) / 6.0) + (delta / 2.0)) / delta;
        float deltaB = (((fmax - b) / 6.0) + (delta / 2.0)) / delta;

        if (r == fmax)
          fHue = deltaB - deltaG; // Hue
        else if (g == fmax)
          fHue = (1.0 / 3.0) + deltaR - deltaB; // Hue
        else if (b == fmax)
          fHue = (2.0 / 3.0) + deltaG - deltaR; // Hue

        if (fHue < 0.0) fHue += 1.0; // Hue
        else if (fHue > 1.0) fHue -= 1.0; // Hue
      }
    }



    float cColour::HueToRGBForSetRGBFromHSL(float f1, float f2, float fHue) const
    {
      if (fHue < 0.0) fHue += 1.0;
      else if (fHue > 1.0) fHue -= 1.0;

      float res = 0.0f;
      if ((6.0 * fHue) < 1.0) res = f1 + (f2 - f1) * 6.0 * fHue;
      else if ((2.0 * fHue) < 1.0) res = f2;
      else if ((3.0 * fHue) < 2.0) res = f1 + (f2 - f1) * ((2.0 / 3.0) - fHue) * 6.0;
      else res = f1;

      return res;
    }

    void cColour::SetRGBFromHSL(float fHue, float fSaturation, float fLuminance)
    {
      if (fSaturation == 0.0f) SetRGB(fLuminance, fLuminance, fLuminance); // Luminance
      else {
        float f2;

        if (fLuminance < 0.5f) f2 = fLuminance * (1.0f + fSaturation);
        else f2 = (fLuminance + fSaturation) - (fSaturation * fLuminance);

        float f1 = 2.0f * fHue - f2;

        r = HueToRGBForSetRGBFromHSL(f1, f2, fHue + (1.0f / 3.0f));
        g = HueToRGBForSetRGBFromHSL(f1, f2, fHue);
        b = HueToRGBForSetRGBFromHSL(f1, f2, fHue - (1.0f / 3.0f));
      }
    }
  }
}
