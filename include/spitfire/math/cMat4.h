#ifndef CMAT4_H
#define CMAT4_H

#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>

namespace spitfire
{
  namespace math
  {
    class cVec4;
    class cQuaternion;

    // NOTE: cMat4 contains a 4x4 matrix stored in column major order
    // OpenGL stores matrix in column major order
    // DirectX stores matrices in row major order

    class cMat4
    {
    public:
      cMat4();
      cMat4(const cMat4& rhs);
      ~cMat4() {}

      const float& operator[](const size_t i) const;
      float& operator[](const size_t i);

      const float& GetValue(const size_t x, const size_t y) const;
      float& GetValue(const size_t x, const size_t y);

      void SetEntry(size_t position, float value);
      float GetEntry(size_t position) const;
      cVec4 GetRow(size_t position) const;
      cVec4 GetColumn(size_t position) const;

      void LoadIdentity();
      void LoadZero();

      // Get parts of the matrix
      cVec3 GetTranslation() const;
      void ClearTranslation();
      cQuaternion GetRotation() const;
      void ClearRotation();
      cVec3 GetScale() const;
      void ClearScale(); // Sets scale x, y, z to 1.0f, 1.0f, 1.0f

      cVec3 GetRight() const;
      cVec3 GetFront() const;
      cVec3 GetUp() const;

      // Set parts of the matrix
      void SetTranslation(const cVec3& translation);
      void SetRotation(const cQuaternion& rhs);
      void SetRotationAxis(float angle, const cVec3& axis);
      void SetRotationX(float angle);
      void SetRotationY(float angle);
      void SetRotationZ(float angle);
      void SetRotationEuler(float angleX, float angleY, float angleZ);
      void SetRotationEuler(const cVec3& angles);
      void SetScale(const cVec3& scaleFactor);
      void SetScale(float scaleFactor); // Set the scale in all directions to a uniform value

      // Binary operators
      cMat4 operator+(const cMat4& rhs) const;
      cMat4 operator-(const cMat4& rhs) const;
      cMat4 operator*(const cMat4& rhs) const;
      cMat4 operator*(float rhs) const;
      cMat4 operator/(float rhs) const;

      bool operator==(const cMat4& rhs) const;
      bool operator!=(const cMat4& rhs) const;

      // Self-add etc
      void operator+=(const cMat4& rhs);
      void operator-=(const cMat4& rhs);
      void operator*=(const cMat4& rhs);
      void operator*=(float rhs);
      void operator/=(float rhs);

      // Unary operators
      cMat4 operator-(void) const;
      cMat4 operator+(void) const { return (*this); }

      // Multiply a vector by this matrix
      cVec3 operator*(const cVec3& rhs) const { return GetRotatedVec3(rhs); }
      cVec4 operator*(const cVec4& rhs) const { return GetRotatedVec4(rhs); }

      // Rotate a vector by rotation part
      void RotateVec3(cVec3& rhs) const { rhs = GetRotatedVec3(rhs); }
      void RotateVec4(cVec4& rhs) const { rhs = GetRotatedVec4(rhs); }

      void InverseRotateVec3(cVec3& rhs) const { rhs = GetInverseRotatedVec3(rhs); }
      void InverseRotateVec4(cVec4& rhs) const { rhs = GetInverseRotatedVec4(rhs); }

      cVec3 GetRotatedVec3(const cVec3& rhs) const;
      cVec3 GetInverseRotatedVec3(const cVec3& rhs) const;

      cVec4 GetRotatedVec4(const cVec4& rhs) const;
      cVec4 GetInverseRotatedVec4(const cVec4& rhs) const;

      // Translate a 3d vector by translation part
      void TranslateVec3(cVec3& rhs) const
      {rhs=GetTranslatedVec3(rhs);}

      void InverseTranslateVec3(cVec3& rhs) const
      {rhs=GetInverseTranslatedVec3(rhs);}

      cVec3 GetTranslatedVec3(const cVec3& rhs) const;
      cVec3 GetInverseTranslatedVec3(const cVec3& rhs) const;

      // Other methods
      void Invert();
      cMat4 GetInverse() const;
      void Transpose();
      cMat4 GetTranspose() const;
      void InvertTranspose();
      cMat4 GetInverseTranspose() const;

      void SetPerspective(float left, float right, float bottom, float top, float n, float f);
      void SetPerspective(float fovy, float aspect, float n, float f);
      void SetOrtho(float left, float right, float bottom, float top, float n, float f);

      void SetFromQuaternion(const cQuaternion& rhs);
      void SetFromMatrix(const cMat4& rhs);

      // Get a pointer to a (float*) for glMultMatrixf etc
      const float* GetOpenGLMatrixPointer() const { return &entries[0]; }

      // Set from a pointer to a (float*) from
      // glGetFloatv(GL_PROJECTION_MATRIX, proj);
      // glGetFloatv(GL_MODELVIEW_MATRIX, modl);
      void SetFromOpenGLMatrix(const float* pEntries);

      void LookAt(const cVec3& eye, const cVec3& dir, const cVec3& up);

      // OpenGL matrix operations
      void MultiplyMatrix(const cMat4& rhs);
      void TranslateMatrix(const cVec3& translation);
      void RotateMatrix(const cVec3& rotation);
      void ScaleMatrix(const cVec3& scale);

    private:
      float entries[16];
    };
  }
}

#endif // CMAT4_H
