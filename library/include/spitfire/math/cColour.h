#ifndef CCOLOUR_H
#define CCOLOUR_H

namespace breathe
{
  namespace math
  {
    class cColour
    {
    public:
      // constructors
      cColour(float newR = 0.0f, float newG = 0.0f, float newB = 0.0f, float newA=0.0f);
      explicit cColour(const float* rhs);
      cColour(const cColour& rhs);

      void Set(float newR, float newG, float newB, float newA); // set member variables

      // accessors kept for compatability
      inline void SetR(float newR) { r = newR; }
      inline void SetG(float newG) { g = newG; }
      inline void SetB(float newB) { b = newB; }
      inline void SetA(float newA) { a = newA; }

      inline float GetR() const { return r; } // public accessor functions
      inline float GetG() const { return g; } // inline, const
      inline float GetB() const { return b; }
      inline float GetA() const { return a; }

      inline void Clear() { r = g = b = a = 0.0f; }

      inline void SetBlack(void) { r = g = b = a = 1.0f; }
      inline void SetWhite(void) { r = g = b = a = 0.0f; }
      inline void SetGrey(float fShade) { r = g = b = a = fShade; Clamp(); }

      void Clamp(void); // clamp all components to [0,1]

      // linear interpolate
      cColour lerp(const cColour & c2, float factor);

      // binary operators
      cColour operator+(const cColour & rhs) const;
      cColour operator-(const cColour & rhs) const;
      cColour operator*(const cColour & rhs) const;
      cColour operator/(const cColour & rhs) const;
      cColour operator*(const float rhs) const;
      cColour operator/(const float rhs) const;

      bool operator==(const cColour & rhs) const;
      bool operator!=(const cColour & rhs) const;

      // self-add etc
      cColour operator+=(const cColour & rhs);
      cColour operator-=(const cColour & rhs);
      cColour operator*=(const cColour & rhs);
      cColour operator/=(const cColour & rhs);
      cColour operator*=(const float rhs);
      cColour operator/=(const float rhs);

      // unary operators
      cColour operator-(void) const {return cColour(-r,-g, -b, -a);}
      cColour operator+(void) const {return (*this);}

      // cast to pointer to float for glColor4fv etc
      operator float* () const {return (float*) this;}
      operator const float* () const {return (const float*) this;}

      // member variables
      float r;
      float g;
      float b;
      float a;
    };
  }
}

#endif //CCOLOUR_H