#include <cmath>
#include <cassert>

#include <vector>

// Spitfire Includes
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cMat3.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cQuaternion.h>

namespace spitfire
{
  namespace math
  {
    cMat3::cMat3()
    {
      LoadIdentity();
    }

    cMat3::cMat3(const cMat3& rhs)
    {
      SetFromMatrix(rhs);
    }

    cMat3::cMat3(const cMat4& rhs)
    {
      SetFromMatrix(rhs);
    }

    cMat3& cMat3::operator=(const cMat3& rhs)
    {
      SetFromMatrix(rhs);
      return *this;
    }

    cMat3& cMat3::operator=(const cMat4& rhs)
    {
      SetFromMatrix(rhs);
      return *this;
    }

    const float& cMat3::operator[](size_t i) const
    {
      assert(i < 9);
      return entries[i];
    }

    float& cMat3::operator[](size_t i)
    {
      assert(i < 9);
      return entries[i];
    }

    const float& cMat3::GetValue(size_t x, size_t y) const
    {
      assert(x < 3);
      assert(y < 3);
      return entries[(y * 3) + x];
    }

    float& cMat3::GetValue(size_t x, size_t y)
    {
      assert(x < 3);
      assert(y < 3);
      return entries[(y * 3) + x];
    }

    void cMat3::SetValue(size_t x, size_t y, float value)
    {
      assert(x < 3);
      assert(y < 3);
      entries[(y * 3) + x] = value;
    }

    float cMat3::GetEntry(size_t position) const
    {
      assert(position < 9);
      return entries[position];
    }

    void cMat3::SetEntry(size_t position, float value)
    {
      assert(position < 9);
      entries[position] = value;
    }

    cVec3 cMat3::GetRow(size_t position) const
    {
      assert(position < 3);
      switch (position) {
        case 0: return cVec3(entries[0], entries[3], entries[6]);
        case 1: return cVec3(entries[1], entries[4], entries[7]);
        case 2: return cVec3(entries[2], entries[5], entries[8]);
      };

      return cVec3(0.0f, 0.0f, 0.0f);
    }

    cVec3 cMat3::GetColumn(size_t position) const
    {
      assert(position < 3);
      if (position == 0) return cVec3(entries[0], entries[1], entries[2]);
      if (position == 1) return cVec3(entries[3], entries[4], entries[5]);
      if (position == 2) return cVec3(entries[6], entries[7], entries[8]);

      return cVec3(0.0f, 0.0f, 0.0f);
    }

    void cMat3::LoadIdentity(void)
    {
      entries[0] = 1.0f;
      entries[1] = 0.0f;
      entries[2] = 0.0f;

      entries[3] = 0.0f;
      entries[4] = 1.0f;
      entries[5] = 0.0f;

      entries[6] = 0.0f;
      entries[7] = 0.0f;
      entries[8] = 1.0f;
    }

    void cMat3::LoadZero(void)
    {
      for (size_t entry = 0; entry < 9; entry++) entries[entry] = 0;
    }


    cMat3 cMat3::operator/(const float rhs) const
    {
      cMat3 result;

      // Cannot divide by zero
      if (rhs == 0.0f) {
        result.LoadZero();
        return result;
      }

      // Divide entries by rhs
      const float temp = 1.0f / rhs;
      for (size_t entry = 0; entry < 9; entry++) result.entries[entry] = entries[entry] * temp;

      return result;
    }

    void cMat3::operator/=(const float rhs)
    {
      (*this) = (*this) / rhs;
    }

    void cMat3::Invert()
    {
      *this = GetInverse();
    }

    cMat3 cMat3::GetInverse() const
    {
      assert(false); // Check this
      const float a00 = entries[0], a01 = entries[1], a02 = entries[2];
      const float a10 = entries[3], a11 = entries[4], a12 = entries[5];
      const float a20 = entries[6], a21 = entries[7], a22 = entries[8];

      const float b01 = (a22 * a11) - (a12 * a21);
      const float b11 = (-a22 * a10) + (a12 * a20);
      const float b21 = (a21 * a10) - (a11 * a20);

      cMat3 result;

      const float fDeterminant = (a00 * b01) + (a01 * b11) + (a02 * b21);
      if (fDeterminant == 0.0f) {
         result.LoadZero();
         return result;
      }

      const float fOneOverDeterminant = 1 / fDeterminant;

      result[0] = b01 * fOneOverDeterminant;
      result[1] = ((-a22 * a01) + (a02 * a21)) * fOneOverDeterminant;
      result[2] = ((a12 * a01) - (a02 * a11)) * fOneOverDeterminant;
      result[3] = b11 * fOneOverDeterminant;
      result[4] = ((a22 * a00) - (a02 * a20)) * fOneOverDeterminant;
      result[5] = ((-a12 * a00) + (a02 * a10)) * fOneOverDeterminant;
      result[6] = b21 * fOneOverDeterminant;
      result[7] = ((-a21 * a00) + (a01 * a20)) * fOneOverDeterminant;
      result[8] = ((a11 * a00) - (a01 * a10)) * fOneOverDeterminant;

      return result;
    }

    void cMat3::Transpose()
    {
      *this = GetTranspose();
    }

    cMat3 cMat3::GetTranspose() const
    {
      assert(false); // Check this
      // Flip the matrix along the upper left to lower right diagonal
      cMat3 result;
      result[0] = entries[ 0];
      result[1] = entries[ 3];
      result[2] = entries[ 6];
      result[3] = entries[ 1];
      result[4] = entries[ 4];
      result[5] = entries[ 7];
      result[6] = entries[ 2];
      result[7] = entries[ 5];
      result[8] = entries[ 8];

      return result;
    }

    void cMat3::InvertTranspose()
    {
      *this = GetInverseTranspose();
    }

    cMat3 cMat3::GetInverseTranspose() const
    {
      float fDeterminant =
        + (m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)))
        - (m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)))
        + (m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0)));

      cMat3 result;
      result.m(0, 0) = + ((m(1, 1) * m(2, 2)) - (m(2, 1) * m(1, 2)));
      result.m(0, 1) = - ((m(1, 0) * m(2, 2)) - (m(2, 0) * m(1, 2)));
      result.m(0, 2) = + ((m(1, 0) * m(2, 1)) - (m(2, 0) * m(1, 1)));
      result.m(1, 0) = - ((m(0, 1) * m(2, 2)) - (m(2, 1) * m(0, 2)));
      result.m(1, 1) = + ((m(0, 0) * m(2, 2)) - (m(2, 0) * m(0, 2)));
      result.m(1, 2) = - ((m(0, 0) * m(2, 1)) - (m(2, 0) * m(0, 1)));
      result.m(2, 0) = + ((m(0, 1) * m(1, 2)) - (m(1, 1) * m(0, 2)));
      result.m(2, 1) = - ((m(0, 0) * m(1, 2)) - (m(1, 0) * m(0, 2)));
      result.m(2, 2) = + ((m(0, 0) * m(1, 1)) - (m(1, 0) * m(0, 1)));

      result /= fDeterminant;

      return result;
    }


    void cMat3::SetFromMatrix(const cMat3& rhs)
    {
      for (size_t i = 0; i < 9; i++) entries[i] = rhs.entries[i];
    }

    void cMat3::SetFromMatrix(const cMat4& rhs)
    {
      // Load the upper left sub matrix of the 4x4 matrix
      entries[0] = rhs[0];
      entries[1] = rhs[1];
      entries[2] = rhs[2];

      entries[3] = rhs[4];
      entries[4] = rhs[5];
      entries[5] = rhs[6];

      entries[6] = rhs[8];
      entries[7] = rhs[9];
      entries[8] = rhs[10];
    }
  }
}
