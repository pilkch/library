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
      void SetTranslation(float x, float y, float z) { SetTranslation(cVec3(x, y, z)); }
      void SetTranslation(const cVec3& translation);
      void SetRotation(const cQuaternion& rhs);
      void SetRotationAxis(float fAngleRadians, const cVec3& axis);
      void SetRotationX(float fAngleRadians);
      void SetRotationY(float fAngleRadians);
      void SetRotationZ(float fAngleRadians);
      void SetRotationEuler(float fAngleRadiansX, float fAngleRadiansY, float fAngleRadiansZ) { SetRotationEuler(cVec3(fAngleRadiansX, fAngleRadiansY, fAngleRadiansZ)); }
      void SetRotationEuler(const cVec3& anglesRadians);
      void SetScale(float scale); // Set the scale in all directions to a uniform value
      void SetScale(float scaleX, float scaleY, float scaleZ) { SetScale(cVec3(scaleX, scaleY, scaleZ)); }
      void SetScale(const cVec3& scale);

      // Assignment operator
      cMat4& operator=(const cMat4& rhs);

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
      cVec3 GetRotatedVec3(const cVec3& rhs) const;
      cVec3 GetInverseRotatedVec3(const cVec3& rhs) const;

      cVec4 GetRotatedVec4(const cVec4& rhs) const;
      cVec4 GetInverseRotatedVec4(const cVec4& rhs) const;

      // Translate a 3d vector by translation part
      cVec3 GetTranslatedVec3(const cVec3& rhs) const;
      cVec3 GetInverseTranslatedVec3(const cVec3& rhs) const;

      // Other methods
      void Invert();
      cMat4 GetInverse() const;
      void Transpose();
      cMat4 GetTranspose() const;
      void InvertTranspose();
      cMat4 GetInverseTranspose() const;

      void SetPerspective(float left, float right, float bottom, float top, float fNear, float fFar);
      void SetPerspective(float fFOVy, float fAspectRatio, float fNear, float fFar);
      void SetOrtho(float left, float right, float bottom, float top, float fNear, float fFar);

      void SetFromQuaternion(const cQuaternion& rhs);
      void SetFromMatrix(const cMat4& rhs);

      // Get a pointer to a (float*) for glMultMatrixf etc
      const float* GetOpenGLMatrixPointer() const { return &entries[0]; }

      // Set from a pointer to a (float*) from
      // glGetFloatv(GL_PROJECTION_MATRIX, proj);
      // glGetFloatv(GL_MODELVIEW_MATRIX, modl);
      void SetFromOpenGLMatrix(const float* pEntries);

      void LookAt(const cVec3& eye, const cVec3& target, const cVec3& up);

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
