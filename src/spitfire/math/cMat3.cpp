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

    void cMat3::Transpose()
    {
      *this = GetTranspose();
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
