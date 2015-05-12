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

    void cColour3::Invert()
    {
      cColourHSV colourHSV;
      colourHSV.SetFromRGB(*this);
      colourHSV.Invert();
      *this = colourHSV.GetRGB();
    }

    cColour3 cColour3::GetComplimentary() const
    {
      cColourHSV colourHSV;
      colourHSV.SetFromRGB(*this);
      colourHSV.ShiftHue(180);
      return colourHSV.GetRGB();
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

    void cColour4::Invert()
    {
      cColourHSV colourHSV;
      colourHSV.SetFromRGBA(*this);
      colourHSV.Invert();
      *this = colourHSV.GetRGBA();
    }

    cColour4 cColour4::GetComplimentary() const
    {
      cColourHSV colourHSV;
      colourHSV.SetFromRGBA(*this);
      colourHSV.ShiftHue(180);
      return colourHSV.GetRGBA();
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


    // HSL
    // Hue, saturation, luminance
    // http://en.wikipedia.org/wiki/HSL_and_HSV

    cColourHSL::cColourHSL() :
      fHue0To360(0.0f),
      fSaturation0To1(0.0f),
      fLuminance0To1(0.0f)
    {
    }

    float cColourHSL::HueToRGB(float f1, float f2, float fHue) const
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

    cColour3 cColourHSL::GetRGB() const
    {
      cColour3 colour;
      if (fSaturation0To1 == 0.0f) colour.SetRGB(fLuminance0To1, fLuminance0To1, fLuminance0To1); // Luminance
      else {
        float f2;

        if (fLuminance0To1 < 0.5f) f2 = fLuminance0To1 * (1.0f + fSaturation0To1);
        else f2 = (fLuminance0To1 + fSaturation0To1) - (fSaturation0To1 * fLuminance0To1);

        const float f1 = 2.0f * fHue0To360 - f2;

        colour.r = HueToRGB(f1, f2, fHue0To360 + (1.0f / 3.0f));
        colour.g = HueToRGB(f1, f2, fHue0To360);
        colour.b = HueToRGB(f1, f2, fHue0To360 - (1.0f / 3.0f));
      }

      return colour;
    }

    cColour4 cColourHSL::GetRGBA() const
    {
      const cColour3 colourRGB = GetRGB();
      return cColour4(colourRGB.r, colourRGB.g, colourRGB.b);
    }

    void cColourHSL::SetFromRGB(const cColour3& colour)
    {
      const float fmin = min(min(colour.r, colour.g), colour.b);    // Min. value of RGB
      const float fmax = max(max(colour.r, colour.g), colour.b);    // Max. value of RGB
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

        const float deltaR = (((fmax - colour.r) / 6.0) + (delta / 2.0)) / delta;
        const float deltaG = (((fmax - colour.g) / 6.0) + (delta / 2.0)) / delta;
        const float deltaB = (((fmax - colour.b) / 6.0) + (delta / 2.0)) / delta;

        // Hue
        if (colour.r == fmax) fHue0To360 = deltaB - deltaG;
        else if (colour.g == fmax) fHue0To360 = (1.0 / 3.0) + deltaR - deltaB;
        else if (colour.b == fmax) fHue0To360 = (2.0 / 3.0) + deltaG - deltaR;

        if (fHue0To360 < 0.0) fHue0To360 += 1.0;
        else if (fHue0To360 > 1.0) fHue0To360 -= 1.0;
      }
    }

    void cColourHSL::SetFromRGBA(const cColour4& colour)
    {
      const cColour3 colourRGB(colour.r, colour.g, colour.b);
      return SetFromRGB(colourRGB);
    }



    // HSV/HSB
    // Hue, saturation, value
    // http://en.wikipedia.org/wiki/HSL_and_HSV
    
    cColourHSV::cColourHSV() :
      fHue0To360(0.0f),
      fSaturation0To1(0.0f),
      fValue0To1(0.0f)
    {
    }

    cColour3 cColourHSV::GetRGB() const
    {
      cColour3 colour;

      if (fValue0To1 > 0.0f) {
        if (fSaturation0To1 <= 0) {
          colour.r = fValue0To1;
          colour.g = fValue0To1;
          colour.b = fValue0To1;
        } else {
          const double hf = fHue0To360 / 60.0;
          const int i = (int)floor(hf);
          const double f = hf - i;
          const double pv = fValue0To1 * (1 - fSaturation0To1);
          const double qv = fValue0To1 * (1 - fSaturation0To1 * f);
          const double tv = fValue0To1 * (1 - fSaturation0To1 * (1 - f));
          switch (i) {

            // Red is the dominant colour

            case 0: {
              colour.r = fValue0To1;
              colour.g = tv;
              colour.b = pv;
              break;
            }

            // Green is the dominant colour

            case 1: {
              colour.r = qv;
              colour.g = fValue0To1;
              colour.b = pv;
              break;
            }
            case 2: {
              colour.r = pv;
              colour.g = fValue0To1;
              colour.b = tv;
              break;
            }

            // Blue is the dominant colour

            case 3: {
              colour.r = pv;
              colour.g = qv;
              colour.b = fValue0To1;
              break;
            }
            case 4: {
              colour.r = tv;
              colour.g = pv;
              colour.b = fValue0To1;
              break;
            }

            // Red is the dominant colour

            case 5: {
              colour.r = fValue0To1;
              colour.g = pv;
              colour.b = qv;
              break;
            }

            // Just in case we overshoot on our math by a little, we put these here. Since its a switch it won't slow us down at all to put these here.

            case 6: {
              colour.r = fValue0To1;
              colour.g = tv;
              colour.b = pv;
              break;
            }
            case -1: {
              colour.r = fValue0To1;
              colour.g = pv;
              colour.b = qv;
              break;
            }

            default: {
              //std::cout<<"cColour4::SetRGBFromHSV HSV colour is not defined HSV("<<fHue0To360<<", "<<fSaturation<<", "<<fValue<<")"<<std::endl;
              assert(false);

              // Set to black/white
              colour.r = fValue0To1;
              colour.g = fValue0To1;
              colour.b = fValue0To1;

              break;
            }
          }
        }
      }

      colour.r = clamp(colour.r, 0.0f, 1.0f);
      colour.g = clamp(colour.g, 0.0f, 1.0f);
      colour.b = clamp(colour.b, 0.0f, 1.0f);

      return colour;
    }

    cColour4 cColourHSV::GetRGBA() const
    {
      const cColour3 colourRGB = GetRGB();
      return cColour4(colourRGB.r, colourRGB.g, colourRGB.b);
    }

    void cColourHSV::SetFromRGB(const cColour3& colour)
    {
       const double maxC = max(colour.r, max(colour.b, colour.g));
       const double minC = min(colour.r, min(colour.b, colour.g));

       fHue0To360 = 0.0f;
       fSaturation0To1 = 0.0f;
       fValue0To1 = maxC;

       const double delta = maxC - minC;
       if (delta == 0.0) {
          fHue0To360 = 0.0f;
          fSaturation0To1 = 0.0f;
       } else {
          fSaturation0To1 = delta / maxC;
          const double dR = 60.0 * (maxC - colour.r) / delta + 180.0;
          const double dG = 60.0 * (maxC - colour.g) / delta + 180.0;
          const double dB = 60.0 * (maxC - colour.b) / delta + 180.0;
          if (colour.r == maxC) fHue0To360 = dB - dG;
          else if (colour.g == maxC) fHue0To360 = 120.0 + dR - dB;
          else fHue0To360 = 240.0 + dG - dR;
       }

       if (fHue0To360 < 0.0f) fHue0To360 += 360.0f;
       else if (fHue0To360 >= 360.0f) fHue0To360 -= 360.0f;
    }

    void cColourHSV::SetFromRGBA(const cColour4& colour)
    {
      const cColour3 colourRGB(colour.r, colour.g, colour.b);
      return SetFromRGB(colourRGB);
    }

    void cColourHSV::Invert()
    {
      fHue0To360 = 360 - fHue0To360;
    }

    void cColourHSV::ShiftHue(float fDegrees)
    {
      fHue0To360 = math::Mod0ToMax(fHue0To360 + fDegrees, 360);
    }


    // ** cColourYUV

    cColourYUV::cColourYUV() :
      y(0.0f),
      u(0.0f),
      v(0.0f)
    {
    }

    cColour3 cColourYUV::GetRGB() const
    {
      const float Wr = 0.299f;
      const float Wb = 0.114f;
      const float Wg = 1 - Wr - Wb;

      cColour3 colour;
      colour.r = -1 - 2 * v * (-1 + Wr) + Wr + y;
      colour.g = (Wb - 2 * u * Wb + (-1 + 2) * (Wb * Wb) + Wr - 2 * v * Wr + (-1 + 2 * v) * (Wr * Wr) + Wg * y) / Wg;
      colour.b = -1 - 2 * u * (-1 + Wb) + Wb + y;
      return colour;
    }

    cColour4 cColourYUV::GetRGBA() const
    {
      const cColour3 colourRGB = GetRGB();
      return cColour4(colourRGB.r, colourRGB.g, colourRGB.b);
    }

    void cColourYUV::SetFromRGB(const cColour3& colour)
    {
      const float Wr = 0.299f;
      const float Wb = 0.114f;
      const float Wg = 1 - Wr - Wb;

      y = Wr * colour.r + Wg * colour.g + Wb * colour.b;
      u = (-1 - colour.b + Wb + y) / ((2 * Wb) - 2);
      v = (-1 - colour.r + Wr + y) / ((2 * Wr) - 2);
    }

    void cColourYUV::SetFromRGBA(const cColour4& colour)
    {
      const cColour3 colourRGB(colour.r, colour.g, colour.b);
      return SetFromRGB(colourRGB);
    }
  }
}
