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

      fLuminance = (fmax + fmin) / 2.0f; // Luminance

      if (delta == 0.0f) { //This is a gray, no chroma...
        fHue = 0.0f;  // Hue
        fSaturation = 0.0f;  // Saturation
      } else {            //Chromatic data...
        if (fLuminance < 0.5f)
          fSaturation = delta / (fmax + fmin); // Saturation
        else
          fSaturation = delta / (2.0f - fmax - fmin); // Saturation

        float deltaR = (((fmax - r) / 6.0f) + (delta / 2.0f)) / delta;
        float deltaG = (((fmax - g) / 6.0f) + (delta / 2.0f)) / delta;
        float deltaB = (((fmax - b) / 6.0f) + (delta / 2.0f)) / delta;

        if (r == fmax)
          fHue = deltaB - deltaG; // Hue
        else if (g == fmax)
          fHue = (1.0f / 3.0f) + deltaR - deltaB; // Hue
        else if (b == fmax)
          fHue = (2.0f / 3.0f) + deltaG - deltaR; // Hue

        if (fHue < 0.0f) fHue += 1.0f; // Hue
        else if (fHue > 1.0f) fHue -= 1.0f; // Hue
      }
    }



    float cColour::HueToRGBForSetRGBFromHSL(float f1, float f2, float fHue) const
    {
      if (fHue < 0.0f) fHue += 1.0f;
      else if (fHue > 1.0f) fHue -= 1.0f;

      float res = 0.0f;
      if ((6.0f * fHue) < 1.0f) res = f1 + (f2 - f1) * 6.0f * fHue;
      else if ((2.0f * fHue) < 1.0f) res = f2;
      else if ((3.0f * fHue) < 2.0f) res = f1 + (f2 - f1) * ((2.0f / 3.0f) - fHue) * 6.0f;
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


    // HSV/HSB
    // http://en.wikipedia.org/wiki/HSL_and_HSV
    void cColour::GetHSVFromRGB(float& fHue0To360, float& fSaturation, float& fValue) const
    {
       const double maxC = max(r, max(b, g));
       const double minC = min(r, min(b, g));

       fHue0To360 = 0.0f;
       fSaturation = 0.0f;
       fValue = maxC;

       double delta = maxC - minC;
       if (delta == 0.0) {
          fHue0To360 = 0.0f;
          fSaturation = 0.0f;
       } else {
          fSaturation = delta / maxC;
          double dR = 60.0 * (maxC - r) / delta + 180.0;
          double dG = 60.0 * (maxC - g) / delta + 180.0;
          double dB = 60.0 * (maxC - b) / delta + 180.0;
          if (r == maxC) fHue0To360 = dB - dG;
          else if (g == maxC) fHue0To360 = 120.0 + dR - dB;
          else fHue0To360 = 240.0 + dG - dR;
       }

       if (fHue0To360 < 0.0f) fHue0To360 += 360.0f;
       if (fHue0To360 >= 360.0f) fHue0To360 -= 360.0f;
    }

    void cColour::SetRGBFromHSV(float fHue0To360, float fSaturation, float fValue)
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
                  //std::cout<<"cColour::SetRGBFromHSV HSV colour is not defined HSV("<<fHue0To360<<", "<<fSaturation<<", "<<fValue<<")"<<std::endl;
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
