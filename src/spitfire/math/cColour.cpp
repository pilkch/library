#include <cassert>
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
    // ** cColour3

    cColour3::cColour3() :
      r(0.0f),
      g(0.0f),
      b(0.0f)
    {
    }

    cColour3::cColour3(float newRed, float newGreen, float newBlue) :
      r(newRed),
      g(newGreen),
      b(newBlue)
    {
    }

    cColour3::cColour3(const cColour3& rhs) :
      r(rhs.r),
      g(rhs.g),
      b(rhs.b)
    {
    }

    void cColour3::SetRGB(float newRed, float newGreen, float newBlue)
    {
      r = newRed;
      g = newGreen;
      b = newBlue;
    }


    // ** cColour4

    cColour4::cColour4() :
      r(0.0f),
      g(0.0f),
      b(0.0f),
      a(1.0f)
    {
    }

    cColour4::cColour4(float newR, float newG, float newB) :
      r(newR),
      g(newG),
      b(newB),
      a(1.0f)
    {
    }

    cColour4::cColour4(float newR, float newG, float newB, float newA) :
      r(newR),
      g(newG),
      b(newB),
      a(newA)
    {
    }

    cColour4::cColour4(const float* rhs)
    {
      r = rhs[0];
      g = rhs[1];
      b = rhs[2];
      a = rhs[3];
    }

    cColour4::cColour4(const cColour4& rhs) :
      r(rhs.r),
      g(rhs.g),
      b(rhs.b),
      a(rhs.a)
    {
    }

    void cColour4::SetRGB(float newR, float newG, float newB)
    {
      r = newR;
      g = newG;
      b = newB;
      a = 1.0f;
    }

    void cColour4::SetRGBA(float newR, float newG, float newB, float newA)
    {
      r = newR;
      g = newG;
      b = newB;
      a = newA;
    }

    void cColour4::Clamp()
    {
      r = clamp(r, 0.0f, 1.0f);
      g = clamp(g, 0.0f, 1.0f);
      b = clamp(b, 0.0f, 1.0f);
      a = clamp(a, 0.0f, 1.0f);
    }

    void cColour4::Negative()
    {
      r = 1.0f - r;
      g = 1.0f - g;
      b = 1.0f - b;
    }

    cColour4 cColour4::lerp(const cColour4& c2, float factor) const
    {
      return ((*this) * factor) + (c2 * (1.0f - factor));
    }

    // http://en.wikipedia.org/wiki/Grayscale
    // From Wikipedia:
    // GreyScale = 0.3 R + 0.59 G + 0.11 B;

    float cColour4::GetGreyScale() const
    {
      return (0.3f * r) + (0.59f * g) + (0.11f * b);
    }

    // http://en.wikipedia.org/wiki/Luma_(video)
    // http://en.wikipedia.org/wiki/Luminance_(relative)
    // From Wikipedia:
    // Y = 0.2126 R + 0.7152 G + 0.0722 B;
    float cColour4::GetLuminance0To1() const
    {
      return (0.2126f * r) + (0.7152f * g) + (0.0722f * b);
    }

    cColour4 cColour4::operator +(const cColour4& rhs) const
    {
      return cColour4(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
    }

    cColour4 cColour4::operator -(const cColour4& rhs) const
    {
      return cColour4(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
    }

    cColour4 cColour4::operator *(const cColour4& rhs) const
    {
      cColour4 result(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);

      return result;
    }

    cColour4 cColour4::operator /(const cColour4& rhs) const
    {
      cColour4 result(r / rhs.r, g / rhs.g, b / rhs.b, a / rhs.a);

      return result;
    }

    cColour4 cColour4::operator *(const float rhs) const
    {
      cColour4 result;

      result.r = r * rhs;
      result.g = g * rhs;
      result.b = b * rhs;
      result.a = a * rhs;

      return result;
    }

    cColour4 cColour4::operator /(const float rhs) const
    {
      cColour4 result;

      const float fOneOverRhs = rhs / 1.0f;
      result.r = r * fOneOverRhs;
      result.g = g * fOneOverRhs;
      result.b = b * fOneOverRhs;
      result.a = a * fOneOverRhs;

      return result;
    }

    bool cColour4::operator ==(const cColour4& rhs) const
    {
      return ((r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a));
    }

    bool cColour4::operator !=(const cColour4& rhs) const
    {
      return !((*this) == rhs);
    }

    cColour4 cColour4::operator +=(const cColour4& rhs)
    {
      (*this) = (*this) + rhs;

      return (*this);
    }

    cColour4 cColour4::operator -=(const cColour4& rhs)
    {
      (*this) = (*this) - rhs;

      return (*this);
    }

    cColour4 cColour4::operator *=(const cColour4& rhs)
    {
      (*this) = (*this) * rhs;

      return (*this);
    }

    cColour4 cColour4::operator /=(const cColour4& rhs)
    {
      (*this) = (*this) / rhs;

      return (*this);
    }

    cColour4 cColour4::operator *=(const float rhs)
    {
      (*this) = (*this) * rhs;

      return (*this);
    }


    cColour4 cColour4::operator /=(const float rhs)
    {
      (*this) = (*this) / rhs;

      return (*this);
    }


    // Hue, saturation, luminance
    // http://en.wikipedia.org/wiki/HSL_and_HSV

    void cColour4::GetHSLFromRGB(float& fHue0To360, float& fSaturation0To1, float& fLuminance0To1) const
    {
      const float fmin = min(min(r, g), b);    // Min. value of RGB
      const float fmax = max(max(r, g), b);    // Max. value of RGB
      const float delta = fmax - fmin;         // Delta RGB value

      // Luminance
      fLuminance0To1 = (fmax + fmin) / 2.0;

      if (delta == 0.0) {
        // Grey no chroma

        // Set hue and saturation
        fHue0To360 = 0.0;
        fSaturation0To1 = 0.0;
      } else {
        // This colour has chroma

        // Saturation
        if (fLuminance0To1 < 0.5) fSaturation0To1 = delta / (fmax + fmin);
        else fSaturation0To1 = delta / (2.0 - fmax - fmin); 

        const float deltaR = (((fmax - r) / 6.0) + (delta / 2.0)) / delta;
        const float deltaG = (((fmax - g) / 6.0) + (delta / 2.0)) / delta;
        const float deltaB = (((fmax - b) / 6.0) + (delta / 2.0)) / delta;

        // Hue
        if (r == fmax) fHue0To360 = deltaB - deltaG;
        else if (g == fmax) fHue0To360 = (1.0 / 3.0) + deltaR - deltaB;
        else if (b == fmax) fHue0To360 = (2.0 / 3.0) + deltaG - deltaR;

        if (fHue0To360 < 0.0) fHue0To360 += 1.0;
        else if (fHue0To360 > 1.0) fHue0To360 -= 1.0;
      }
    }



    float cColour4::HueToRGBForSetRGBFromHSL(float f1, float f2, float fHue) const
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

    void cColour4::SetRGBFromHSL(float fHue, float fSaturation, float fLuminance)
    {
      if (fSaturation == 0.0f) SetRGB(fLuminance, fLuminance, fLuminance); // Luminance
      else {
        float f2;

        if (fLuminance < 0.5f) f2 = fLuminance * (1.0f + fSaturation);
        else f2 = (fLuminance + fSaturation) - (fSaturation * fLuminance);

        const float f1 = 2.0f * fHue - f2;

        r = HueToRGBForSetRGBFromHSL(f1, f2, fHue + (1.0f / 3.0f));
        g = HueToRGBForSetRGBFromHSL(f1, f2, fHue);
        b = HueToRGBForSetRGBFromHSL(f1, f2, fHue - (1.0f / 3.0f));
      }
    }


    // HSV/HSB
    // http://en.wikipedia.org/wiki/HSL_and_HSV
    void cColour4::GetHSVFromRGB(float& fHue0To360, float& fSaturation, float& fValue) const
    {
       const double maxC = max(r, max(b, g));
       const double minC = min(r, min(b, g));

       fHue0To360 = 0.0f;
       fSaturation = 0.0f;
       fValue = maxC;

       const double delta = maxC - minC;
       if (delta == 0.0) {
          fHue0To360 = 0.0f;
          fSaturation = 0.0f;
       } else {
          fSaturation = delta / maxC;
          const double dR = 60.0 * (maxC - r) / delta + 180.0;
          const double dG = 60.0 * (maxC - g) / delta + 180.0;
          const double dB = 60.0 * (maxC - b) / delta + 180.0;
          if (r == maxC) fHue0To360 = dB - dG;
          else if (g == maxC) fHue0To360 = 120.0 + dR - dB;
          else fHue0To360 = 240.0 + dG - dR;
       }

       if (fHue0To360 < 0.0f) fHue0To360 += 360.0f;
       else if (fHue0To360 >= 360.0f) fHue0To360 -= 360.0f;
    }

    void cColour4::SetRGBFromHSV(float fHue0To360, float fSaturation, float fValue)
    {
       r = 0.0f;
       g = 0.0f;
       b = 0.0f;

       if (fValue > 0.0f) {
          if (fSaturation <= 0) {
             r = fValue;
             g = fValue;
             b = fValue;
          } else {
             while (fHue0To360 < 0) fHue0To360 += 360;
             while (fHue0To360 >= 360) fHue0To360 -= 360;

             const double hf = fHue0To360 / 60.0;
             const int i = (int)floor(hf);
             const double f = hf - i;
             const double pv = fValue * (1 - fSaturation);
             const double qv = fValue * (1 - fSaturation * f);
             const double tv = fValue * (1 - fSaturation * (1 - f));
             switch (i) {

                // Red is the dominant colour

                case 0: {
                  r = fValue;
                  g = tv;
                  b = pv;
                  break;
                }

                // Green is the dominant colour

                case 1: {
                  r = qv;
                  g = fValue;
                  b = pv;
                  break;
                }
                case 2: {
                  r = pv;
                  g = fValue;
                  b = tv;
                  break;
                }

                // Blue is the dominant colour

                case 3: {
                  r = pv;
                  g = qv;
                  b = fValue;
                  break;
                }
                case 4: {
                  r = tv;
                  g = pv;
                  b = fValue;
                  break;
                }

                // Red is the dominant colour

                case 5: {
                  r = fValue;
                  g = pv;
                  b = qv;
                  break;
                }

                // Just in case we overshoot on our math by a little, we put these here. Since its a switch it won't slow us down at all to put these here.

                case 6: {
                  r = fValue;
                  g = tv;
                  b = pv;
                  break;
                }
                case -1: {
                  r = fValue;
                  g = pv;
                  b = qv;
                  break;
                }

                default: {
                  //std::cout<<"cColour4::SetRGBFromHSV HSV colour is not defined HSV("<<fHue0To360<<", "<<fSaturation<<", "<<fValue<<")"<<std::endl;
                  assert(false);

                  // Set to black/white
                  r = fValue;
                  g = fValue;
                  b = fValue;

                  break;
                }
             }
          }
       }

       r = clamp(r, 0.0f, 1.0f);
       g = clamp(g, 0.0f, 1.0f);
       b = clamp(b, 0.0f, 1.0f);
    }
  }
}
