#ifndef CCOLOUR_H
#define CCOLOUR_H

namespace spitfire
{
  namespace math
  {
    class cColour
    {
    public:
      cColour(); // Defaults to 0.0f, 0.0f, 0.0f, 1.0f
      cColour(float newR, float newG, float newB); // Alpha defaults to 1.0f
      cColour(float newR, float newG, float newB, float newA);
      explicit cColour(const float* rhs);
      cColour(const cColour& rhs);

      inline void Clear() { r = g = b = 0.0f; a = 1.0f; }

      void SetRGB(float newR, float newG, float newB); // Set values, a is set to 1.0f
      void SetRGBA(float newR, float newG, float newB, float newA); // Set values

      inline void SetBlack() { r = g = b = a = 1.0f; }
      inline void SetWhite() { r = g = b = 0.0f; a = 1.0f; }
      inline void SetGrey(float fShade) { r = g = b = a = fShade; }

      // accessors kept for compatability
      inline void SetR(float newR) { r = newR; }
      inline void SetG(float newG) { g = newG; }
      inline void SetB(float newB) { b = newB; }
      inline void SetA(float newA) { a = newA; }

      inline float GetR() const { return r; } // public accessor functions
      inline float GetG() const { return g; } // inline, const
      inline float GetB() const { return b; }
      inline float GetA() const { return a; }

      float GetGreyScale() const;
      float GetLuminance0To1() const;


      // Hue, saturation, luminance
      // http://en.wikipedia.org/wiki/HSL_and_HSV

      void GetHSLFromRGB(float& fHue, float& fSaturation, float& fLuminance) const;
      void SetRGBFromHSL(float fHue, float fSaturation, float fLuminance);

      // HSV/HSB
      // http://en.wikipedia.org/wiki/HSL_and_HSV
      // h are 0..360
      // s and v are 0..1
      void GetHSVFromRGB(float& fHue0To360, float& fSaturation, float& fValue) const;
      void SetRGBFromHSV(float fHue0To360, float fSaturation, float fValue);

      void Clamp(); // clamp all components to [0,1]

      // linear interpolate
      cColour lerp(const cColour& c2, float factor) const;

      // Binary operators
      cColour operator+(const cColour& rhs) const;
      cColour operator-(const cColour& rhs) const;
      cColour operator*(const cColour& rhs) const;
      cColour operator/(const cColour& rhs) const;
      cColour operator*(const float rhs) const;
      cColour operator/(const float rhs) const;


      // Overloaded operators
      // Binary operators
      friend cColour operator*(const float lhs, const cColour& rhs)
      {
        return cColour(lhs * rhs.r, lhs * rhs.g, lhs * rhs.b, lhs * rhs.a);
      }
      friend cColour operator/(const float lhs, const cColour& rhs)
      {
        return cColour(lhs / rhs.r, lhs / rhs.g, lhs / rhs.b, lhs / rhs.a);
      }


      bool operator==(const cColour& rhs) const;
      bool operator!=(const cColour& rhs) const;

      // Self-add etc
      cColour operator+=(const cColour& rhs);
      cColour operator-=(const cColour& rhs);
      cColour operator*=(const cColour& rhs);
      cColour operator/=(const cColour& rhs);
      cColour operator*=(const float rhs);
      cColour operator/=(const float rhs);

      // Unary operators
      cColour operator-() const { return cColour(-r, -g, -b, -a); }
      cColour operator+() const { return (*this); }


      // Get a pointer to float for glColor4fv etc.
      const float* GetPointerConst() const { return (const float*)this; }


      float r;
      float g;
      float b;
      float a;

    private:
      float HueToRGBForSetRGBFromHSL(float f1, float f2, float hue) const;
    };
  }
}

#endif // CCOLOUR_H
