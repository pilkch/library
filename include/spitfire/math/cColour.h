#ifndef CCOLOUR_H
#define CCOLOUR_H

namespace spitfire
{
  namespace math
  {
    // ** cColour3

    class cColour3
    {
    public:
      cColour3(); // Defaults to 0.0f, 0.0f, 0.0f
      cColour3(float newR, float newG, float newB);
      explicit cColour3(const float* rhs);
      cColour3(const cColour3& rhs);

      inline void Clear() { r = g = b = 0.0f; }

      void SetRGB(float newR, float newG, float newB); // Set values

      inline void SetBlack() { r = g = b = 0.0f; }
      inline void SetWhite() { r = g = b = 1.0f; }
      inline void SetGrey(float fShade) { r = g = b = fShade; }

      // accessors kept for compatability
      inline void SetR(float newR) { r = newR; }
      inline void SetG(float newG) { g = newG; }
      inline void SetB(float newB) { b = newB; }

      inline float GetR() const { return r; } // public accessor functions
      inline float GetG() const { return g; } // inline, const
      inline float GetB() const { return b; }

      float GetGreyScale() const;
      float GetLuminance0To1() const;

      void Clamp(); // clamp all components to [0,1]

      void Negative(); // Inverts each component

      // linear interpolate
      cColour3 lerp(const cColour3& c2, float factor) const;

      // Binary operators
      cColour3 operator+(const cColour3& rhs) const;
      cColour3 operator-(const cColour3& rhs) const;
      cColour3 operator*(const cColour3& rhs) const;
      cColour3 operator/(const cColour3& rhs) const;
      cColour3 operator*(const float rhs) const;
      cColour3 operator/(const float rhs) const;


      // Overloaded operators
      // Binary operators
      friend cColour3 operator*(const float lhs, const cColour3& rhs)
      {
        return cColour3(lhs * rhs.r, lhs * rhs.g, lhs * rhs.b);
      }
      friend cColour3 operator/(const float lhs, const cColour3& rhs)
      {
        return cColour3(lhs / rhs.r, lhs / rhs.g, lhs / rhs.b);
      }


      bool operator==(const cColour3& rhs) const;
      bool operator!=(const cColour3& rhs) const;

      // Self-add etc
      cColour3 operator+=(const cColour3& rhs);
      cColour3 operator-=(const cColour3& rhs);
      cColour3 operator*=(const cColour3& rhs);
      cColour3 operator/=(const cColour3& rhs);
      cColour3 operator*=(const float rhs);
      cColour3 operator/=(const float rhs);

      // Unary operators
      cColour3 operator-() const { return cColour3(-r, -g, -b); }
      cColour3 operator+() const { return (*this); }


      float r;
      float g;
      float b;
    };


    // ** cColour4

    class cColour4
    {
    public:
      cColour4(); // Defaults to 0.0f, 0.0f, 0.0f, 1.0f
      cColour4(float newR, float newG, float newB); // Alpha defaults to 1.0f
      cColour4(float newR, float newG, float newB, float newA);
      explicit cColour4(const float* rhs);
      cColour4(const cColour4& rhs);

      inline void Clear() { r = g = b = 0.0f; a = 1.0f; }

      void SetRGB(float newR, float newG, float newB); // Set values, a is set to 1.0f
      void SetRGBA(float newR, float newG, float newB, float newA); // Set values

      inline void SetBlack() { r = g = b = 0.0f; a = 1.0f; }
      inline void SetWhite() { r = g = b = 1.0f; a = 1.0f; }
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

      void Clamp(); // clamp all components to [0,1]
      
      void Negative(); // Inverts each RGB component, doesn't change alpha

      // linear interpolate
      cColour4 lerp(const cColour4& c2, float factor) const;

      // Binary operators
      cColour4 operator+(const cColour4& rhs) const;
      cColour4 operator-(const cColour4& rhs) const;
      cColour4 operator*(const cColour4& rhs) const;
      cColour4 operator/(const cColour4& rhs) const;
      cColour4 operator*(const float rhs) const;
      cColour4 operator/(const float rhs) const;


      // Overloaded operators
      // Binary operators
      friend cColour4 operator*(const float lhs, const cColour4& rhs)
      {
        return cColour4(lhs * rhs.r, lhs * rhs.g, lhs * rhs.b, lhs * rhs.a);
      }
      friend cColour4 operator/(const float lhs, const cColour4& rhs)
      {
        return cColour4(lhs / rhs.r, lhs / rhs.g, lhs / rhs.b, lhs / rhs.a);
      }


      bool operator==(const cColour4& rhs) const;
      bool operator!=(const cColour4& rhs) const;

      // Self-add etc
      cColour4 operator+=(const cColour4& rhs);
      cColour4 operator-=(const cColour4& rhs);
      cColour4 operator*=(const cColour4& rhs);
      cColour4 operator/=(const cColour4& rhs);
      cColour4 operator*=(const float rhs);
      cColour4 operator/=(const float rhs);

      // Unary operators
      cColour4 operator-() const { return cColour4(-r, -g, -b, -a); }
      cColour4 operator+() const { return (*this); }


      float r;
      float g;
      float b;
      float a;
    };

    // For backwards compatibility
    typedef cColour4 cColour;


    // ** cColourHSL
    //
    // HSL
    // http://en.wikipedia.org/wiki/HSL_and_HSV
    // http://www.rapidtables.com/convert/color/rgb-to-hsl.htm
    // Hue is 0..360
    // Saturation and luminance 0..1

    class cColourHSL
    {
    public:
      cColourHSL();

      cColour3 GetRGB() const;
      cColour4 GetRGBA() const;
      void SetFromRGB(const cColour3& colour);
      void SetFromRGBA(const cColour4& colour);

      float fHue0To360;
      float fSaturation0To1;
      float fLuminance0To1;

    private:
      float HueToRGB(float f1, float f2, float hue) const;
    };


    // ** cColourHSV
    //
    // HSV/HSB
    // http://en.wikipedia.org/wiki/HSL_and_HSV
    // Hue is 0..360
    // Saturation and value are 0..1

    class cColourHSV
    {
    public:
      cColourHSV();

      cColour3 GetRGB() const;
      cColour4 GetRGBA() const;
      void SetFromRGB(const cColour3& colour);
      void SetFromRGBA(const cColour4& colour);

      float fHue0To360;
      float fSaturation0To1;
      float fValue0To1;
    };
  }
}

#endif // CCOLOUR_H
