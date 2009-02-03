#ifndef CVEC4_H
#define CVEC4_H

namespace breathe
{
  namespace math
  {
    class cVec4
    {
    public:
      // constructors
      cVec4(float newX = 0.0f, float newY = 0.0f, float newZ = 0.0f, float newW=0.0f);
      cVec4(const float * rhs);
      cVec4(const cVec4 & rhs);
      cVec4(const cVec3 & rhs); // convert v3d to v4d

      void Set(float newX, float newY, float newZ, float newW); // set member variables

      // accessors kept for compatability
      inline void SetX(float newX) { x = newX; }
      inline void SetY(float newY) { y = newY; }
      inline void SetZ(float newZ) { z = newZ; }
      inline void SetW(float newW) { w = newW; }

      inline float GetX() const { return x; } // public accessor functions
      inline float GetY() const { return y; } // inline, const
      inline float GetZ() const { return z; }
      inline float GetW() const { return w; }

      inline cVec3 GetXYZ() const { return cVec3(x, y, z); }

      inline void Clear() { x = y = z = w = 0.0f; }

      void SetZero(void);
      void SetOne(void);

      float GetLength() const
      {  return sqrtf((x*x)+(y*y)+(z*z)); }

      float GetSquaredLength() const
      {  return (x*x)+(y*y)+(z*z); }

      //vector algebra
      float DotProduct(const cVec4 & rhs) const;
      void Normalise();

      //rotations
      void RotateX(double angle);
      cVec4 GetRotatedX(double angle) const;
      void RotateY(double angle);
      cVec4 GetRotatedY(double angle) const;
      void RotateZ(double angle);
      cVec4 GetRotatedZ(double angle) const;
      void RotateAxis(double angle, const cVec3 & axis);
      cVec4 GetRotatedAxis(double angle, const cVec3 & axis) const;

      // Linear Interpolation
      cVec4 lerp(const cVec4& v2, float factor);

      //binary operators
      cVec4 operator+(const cVec4 & rhs) const; // overloaded operators
      cVec4 operator-(const cVec4 & rhs) const;
      cVec4 operator*(const float rhs) const;
      cVec4 operator/(const float rhs) const;

      bool operator==(const cVec4 & rhs) const;
      bool operator!=(const cVec4 & rhs) const;

      //self-add etc
      void operator+=(const cVec4 & rhs);
      void operator-=(const cVec4 & rhs);
      void operator*=(const float rhs);
      void operator/=(const float rhs);

      //unary operators
      cVec4 operator-(void) const {return cVec4(-x, -y, -z, -w);}
      cVec4 operator+(void) const {return (*this);}

      //cast to pointer to float for glVertex4fv etc
      operator float* () const {return (float*) this;}
      operator const float* () const {return (const float*) this;}

      operator cVec3 ()
      {
        if (w!=0)
          return cVec3(x/w, y/w, z/w);
        else
          return cVec3(x, y, z);
      }

      //member variables
      float x;
      float y;
      float z;
      float w;
    };
  }
}

#endif //CVEC4_H
