#ifndef CMAT3_H
#define CMAT3_H

// Spitfire headers
#include <spitfire/math/cVec3.h>

namespace spitfire
{
  namespace math
  {
    class cMat4;
    class cQuaternion;

    // NOTE: cMat3 contains a 3x3 matrix stored in column major order
    // OpenGL stores matrix in column major order
    // DirectX stores matrices in row major order

    class cMat3
    {
    public:
      friend class cMat4;

      cMat3();
      cMat3(const cMat3& rhs);
      explicit cMat3(const cMat4& rhs);
      ~cMat3() {}

      const float& operator[](size_t i) const;
      float& operator[](size_t i);

      const float& GetValue(size_t x, size_t y) const;
      float& GetValue(size_t x, size_t y);
      void SetValue(size_t x, size_t y, float value);

      void SetEntry(size_t position, float value);
      float GetEntry(size_t position) const;
      cVec3 GetRow(size_t position) const;
      cVec3 GetColumn(size_t position) const;

      void LoadIdentity();
      void LoadZero();

      // Assignment operator
      cMat3& operator=(const cMat3& rhs);
      cMat3& operator=(const cMat4& rhs);

      // Operations
      cMat3 operator/(float rhs) const;
      void operator/=(float rhs);

      // Other methods
      void Invert();
      cMat3 GetInverse() const;
      void Transpose();
      cMat3 GetTranspose() const;
      void InvertTranspose();
      cMat3 GetInverseTranspose() const;

      void SetFromMatrix(const cMat3& rhs);
      void SetFromMatrix(const cMat4& rhs);

      // Get a pointer to a (float*) for glMultMatrixf etc
      const float* GetOpenGLMatrixPointer() const { return &entries[0]; }

    private:
      // Convenience functions
      const float& m(size_t x, size_t y) const { return GetValue(x, y); }
      float& m(size_t x, size_t y) { return GetValue(x, y); }

      float entries[9];
    };
  }
}

#endif // CMAT3_H
