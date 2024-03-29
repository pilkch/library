#include <cmath>
#include <cassert>

#include <vector>

// Spitfire Includes
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat3.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cQuaternion.h>

#ifdef BUILD_DEBUG
#include <spitfire/util/log.h>
#endif

namespace spitfire
{
  namespace math
  {
    cMat4::cMat4()
    {
      LoadIdentity();
    }

    cMat4::cMat4(const cMat4& rhs)
    {
      SetFromMatrix(rhs);
    }

    cMat4::cMat4(cMat4&& rhs)
    {
      SetFromMatrix(rhs);
    }

    cMat4::cMat4(const cMat3& rhs)
    {
      SetFromMatrix(rhs);
    }

    cMat4::cMat4(float value0, float value1, float value2, float value3, float value4, float value5, float value6, float value7, float value8, float value9, float value10, float value11, float value12, float value13, float value14, float value15)
    {
      SetEntries(value0, value1, value2, value3, value4, value5, value6, value7, value8, value9, value10, value11, value12, value13, value14, value15);
    }

    cMat4& cMat4::operator=(const cMat4& rhs)
    {
      SetFromMatrix(rhs);
      return *this;
    }

    cMat4& cMat4::operator=(const cMat4&& rhs)
    {
      SetFromMatrix(rhs);
      return *this;
    }

    const float& cMat4::operator[](const size_t i) const
    {
      assert(i < 16);
      return entries[i];
    }

    float& cMat4::operator[](const size_t i)
    {
      assert(i < 16);
      return entries[i];
    }

    const float& cMat4::GetValue(const size_t x, const size_t y) const
    {
      assert(x < 4);
      assert(y < 4);
      return entries[(x * 4) + y];
    }

    void cMat4::SetValue(size_t x, size_t y, float value)
    {
      assert(x < 4);
      assert(y < 4);
      entries[(x * 4) + y] = value;
    }

    float cMat4::GetEntry(size_t position) const
    {
      assert(position < 16);
      return entries[position];
    }

    cVec4 cMat4::GetRow(size_t position) const
    {
      switch (position) {
        case 0: return cVec4(entries[0], entries[4], entries[8], entries[12]);
        case 1: return cVec4(entries[1], entries[5], entries[9], entries[13]);
        case 2: return cVec4(entries[2], entries[6], entries[10], entries[14]);
        case 3: return cVec4(entries[3], entries[7], entries[11], entries[15]);
      };

      return cVec4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    cVec4 cMat4::GetColumn(size_t position) const
    {
      switch (position) {
        case 0: return cVec4(entries[0], entries[1], entries[2], entries[3]);
        case 1: return cVec4(entries[4], entries[5], entries[6], entries[7]);
        case 2: return cVec4(entries[8], entries[9], entries[10], entries[11]);
        case 3: return cVec4(entries[12], entries[13], entries[14], entries[15]);
      };

      // "Illegal argument to cMat4::GetColumn()"
      return cVec4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    void cMat4::SetEntry(size_t position, float value)
    {
      assert(position < 16);
      entries[position] = value;
    }

    void cMat4::SetEntries(float value0, float value1, float value2, float value3, float value4, float value5, float value6, float value7, float value8, float value9, float value10, float value11, float value12, float value13, float value14, float value15)
    {
      entries[0] = value0;
      entries[1] = value1;
      entries[2] = value2;
      entries[3] = value3;
      entries[4] = value4;
      entries[5] = value5;
      entries[6] = value6;
      entries[7] = value7;
      entries[8] = value8;
      entries[9] = value9;
      entries[10] = value10;
      entries[11] = value11;
      entries[12] = value12;
      entries[13] = value13;
      entries[14] = value14;
      entries[15] = value15;
    }


    void cMat4::LoadIdentity()
    {
      entries[0] = 1.0f;
      entries[1] = 0.0f;
      entries[2] = 0.0f;
      entries[3] = 0.0f;

      entries[4] = 0.0f;
      entries[5] = 1.0f;
      entries[6] = 0.0f;
      entries[7] = 0.0f;

      entries[8] = 0.0f;
      entries[9] = 0.0f;
      entries[10] = 1.0f;
      entries[11] = 0.0f;

      entries[12] = 0.0f;
      entries[13] = 0.0f;
      entries[14] = 0.0f;
      entries[15] = 1.0f;
    }

    void cMat4::LoadZero()
    {
      for (size_t entry = 0; entry < 16; entry++) entries[entry] = 0;
    }

    void cMat4::ClearRotation()
    {
      // Load identity matrix, but only for the rotation part
      entries[0] = 1.0f;
      entries[1] = 0.0f;
      entries[2] = 0.0f;

      entries[4] = 0.0f;
      entries[5] = 1.0f;
      entries[6] = 0.0f;

      entries[8] = 0.0f;
      entries[9] = 0.0f;
      entries[10] = 1.0f;
    }

    void cMat4::ClearTranslation()
    {
      // Load identity matrix, but only for the translation part
      entries[12] = 0.0f;
      entries[13] = 0.0f;
      entries[14] = 0.0f;
    }

    cVec3 cMat4::GetRight() const
    {
      return cVec3(entries[0], entries[1], entries[2]);
    }

    cVec3 cMat4::GetFront() const
    {
      return cVec3(entries[4], entries[5], entries[6]);
    }

    cVec3 cMat4::GetUp() const
    {
      return cVec3(entries[8], entries[9], entries[10]);
    }

    // Returned in absolute units
    cVec3 cMat4::GetTranslation() const
    {
      return cVec3(entries[12], entries[13], entries[14]);
    }

    // Returned in degrees
    cQuaternion cMat4::GetRotation() const
    {
      const math::cVec3 r(
        entries[0] + entries[4] + entries[8],
        entries[1] + entries[5] + entries[9],
        entries[0] + entries[1] + entries[4] + entries[5]
      ); //entries[2] + entries[6] + entries[10]));

      cQuaternion q;
      q.SetFromAngles(r);

      return q;
    }


    void cMat4::SetTranslation(const cVec3& translation)
    {
      entries[12] = translation.x;
      entries[13] = translation.y;
      entries[14] = translation.z;
    }

    void cMat4::SetRotationEuler(const cVec3& angleRadians)
    {
      const float sx = sinf(angleRadians.x);
      const float cx = cosf(angleRadians.x);
      const float sy = sinf(angleRadians.y);
      const float cy = cosf(angleRadians.y);
      const float sz = sinf(angleRadians.z);
      const float cz = cosf(angleRadians.z);

      entries[0] = cy * cz;
      entries[1] = cy * sz;
      entries[2] = -sy;

      entries[4] = (sx * sy * cz) - (cx * sz);
      entries[5] = (sx * sy * sz) + (cx * cz);
      entries[6] = sx * cy;

      entries[8] = (cx * sy * cz) + (sx * sz);
      entries[9] = (cx * sy * sz) - (sx * cz);
      entries[10] = cx * cy;
    }

    void cMat4::SetRotation(const cQuaternion& rhs)
    {
      const float xx = rhs.x * rhs.x;
      const float xy = rhs.x * rhs.y;
      const float xz = rhs.x * rhs.z;
      const float xw = rhs.x * rhs.w;

      const float yy = rhs.y * rhs.y;
      const float yz = rhs.y * rhs.z;
      const float yw = rhs.y * rhs.w;

      const float zz = rhs.z * rhs.z;
      const float zw = rhs.z * rhs.w;

      entries[0] = 1.0f - 2.0f * (yy + zz);
      entries[1] = 2.0f * (xy - zw);
      entries[2] = 2.0f * (xz + yw);
      entries[3] = 0.0f;

      entries[4] = 2.0f * (xy + zw);
      entries[5] = 1.0f - 2.0f * (xx + zz);
      entries[6] = 2.0f * (yz - xw);
      entries[7] = 0.0f;

      entries[8] = 2.0f * (xz - yw);
      entries[9] = 2.0f * (yz + xw);
      entries[10] = 1.0f - 2.0f * (xx + yy);
      entries[11] = 0.0f;

      entries[12] = 0.0f;
      entries[13] = 0.0f;
      entries[14] = 0.0f;
      entries[15] = 1.0f;
    }

    void cMat4::SetRotationAxisAngleRadians(const cVec3& axis, float fAngleRadians)
    {
      if (axis.IsZeroVector() || spitfire::math::IsApproximatelyZero(fAngleRadians)) {
        LoadIdentity();
        return;
      }

      const float c = cosf(fAngleRadians);
      const float l_c = 1.0f - c;

      const float s = sin(fAngleRadians);

      SetEntries(
        axis.x * axis.x + (1.0f - axis.x * axis.x) * c, axis.x * axis.y * l_c - axis.z * s, axis.x * axis.z * l_c + axis.y * s, 0.0f,
        axis.x * axis.y * l_c + axis.z * s, axis.y * axis.y + (1.0f - axis.y * axis.y) * c, axis.y * axis.z * l_c - axis.x * s, 0.0f,
        axis.x * axis.z * l_c - axis.y * s, axis.y * axis.z * l_c + axis.x * s, axis.z * axis.z + (1 - axis.z * axis.z) * c, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
      );
    }

    void cMat4::SetRotationX(float fAngleRadians)
    {
      entries[5] = cosf(fAngleRadians);
      entries[6] = -sinf(fAngleRadians);

      entries[9] = sinf(fAngleRadians);
      entries[10] = cosf(fAngleRadians);
    }

    void cMat4::SetRotationY(float fAngleRadians)
    {
      entries[0] = cosf(fAngleRadians);
      entries[2] = sinf(fAngleRadians);

      entries[8] = -sinf(fAngleRadians);
      entries[10] = cosf(fAngleRadians);
    }

    void cMat4::SetRotationZ(float fAngleRadians)
    {
      entries[0] = cosf(fAngleRadians);
      entries[1] = sinf(fAngleRadians);

      entries[4] = -sinf(fAngleRadians);
      entries[5] = cosf(fAngleRadians);
    }

    void cMat4::SetScale(const cVec3& scale)
    {
      entries[0] = scale.x;
      entries[5] = scale.y;
      entries[10] = scale.z;
    }

    void cMat4::SetScale(float scale)
    {
      entries[0] = scale;
      entries[5] = scale;
      entries[10] = scale;
    }




    cMat4 cMat4::operator+(const cMat4& rhs) const    // Overloaded operators
    {
      cMat4 result;
      for (size_t entry = 0; entry < 16; entry++) {
        result.SetEntry(entry, entries[entry] + rhs.GetEntry(entry));  // Add entries
      }
      return result;
    }

    cMat4 cMat4::operator-(const cMat4& rhs) const    // Overloaded operators
    {
      cMat4 result;
      for (size_t entry = 0; entry < 16; entry++) {
        result.SetEntry(entry, entries[entry] - rhs.GetEntry(entry));  // Subtract entries
      }
      return result;
    }

    cMat4 cMat4::operator*(const cMat4& rhs) const
    {
      cMat4 result;
      result.SetEntry(0, (entries[0] * rhs.GetEntry(0))+(entries[4] * rhs.GetEntry(1))+(entries[8] * rhs.GetEntry(2))+(entries[12] * rhs.GetEntry(3)));
      result.SetEntry(4, (entries[0] * rhs.GetEntry(4))+(entries[4] * rhs.GetEntry(5))+(entries[8] * rhs.GetEntry(6))+(entries[12] * rhs.GetEntry(7)));
      result.SetEntry(8, (entries[0] * rhs.GetEntry(8))+(entries[4] * rhs.GetEntry(9))+(entries[8] * rhs.GetEntry(10))+(entries[12] * rhs.GetEntry(11)));
      result.SetEntry(12, (entries[0] * rhs.GetEntry(12))+(entries[4] * rhs.GetEntry(13))+(entries[8] * rhs.GetEntry(14))+(entries[12] * rhs.GetEntry(15)));

      result.SetEntry(1, (entries[1] * rhs.GetEntry(0))+(entries[5] * rhs.GetEntry(1))+(entries[9] * rhs.GetEntry(2))+(entries[13] * rhs.GetEntry(3)));
      result.SetEntry(5, (entries[1] * rhs.GetEntry(4))+(entries[5] * rhs.GetEntry(5))+(entries[9] * rhs.GetEntry(6))+(entries[13] * rhs.GetEntry(7)));
      result.SetEntry(9, (entries[1] * rhs.GetEntry(8))+(entries[5] * rhs.GetEntry(9))+(entries[9] * rhs.GetEntry(10))+(entries[13] * rhs.GetEntry(11)));
      result.SetEntry(13, (entries[1] * rhs.GetEntry(12))+(entries[5] * rhs.GetEntry(13))+(entries[9] * rhs.GetEntry(14))+(entries[13] * rhs.GetEntry(15)));

      result.SetEntry(2, (entries[2] * rhs.GetEntry(0))+(entries[6] * rhs.GetEntry(1))+(entries[10] * rhs.GetEntry(2))+(entries[14] * rhs.GetEntry(3)));
      result.SetEntry(6, (entries[2] * rhs.GetEntry(4))+(entries[6] * rhs.GetEntry(5))+(entries[10] * rhs.GetEntry(6))+(entries[14] * rhs.GetEntry(7)));
      result.SetEntry(10, (entries[2] * rhs.GetEntry(8))+(entries[6] * rhs.GetEntry(9))+(entries[10] * rhs.GetEntry(10))+(entries[14] * rhs.GetEntry(11)));
      result.SetEntry(14, (entries[2] * rhs.GetEntry(12))+(entries[6] * rhs.GetEntry(13))+(entries[10] * rhs.GetEntry(14))+(entries[14] * rhs.GetEntry(15)));

      result.SetEntry(3, (entries[3] * rhs.GetEntry(0))+(entries[7] * rhs.GetEntry(1))+(entries[11] * rhs.GetEntry(2))+(entries[15] * rhs.GetEntry(3)));
      result.SetEntry(7, (entries[3] * rhs.GetEntry(4))+(entries[7] * rhs.GetEntry(5))+(entries[11] * rhs.GetEntry(6))+(entries[15] * rhs.GetEntry(7)));
      result.SetEntry(11, (entries[3] * rhs.GetEntry(8))+(entries[7] * rhs.GetEntry(9))+(entries[11] * rhs.GetEntry(10))+(entries[15] * rhs.GetEntry(11)));
      result.SetEntry(15, (entries[3] * rhs.GetEntry(12))+(entries[7] * rhs.GetEntry(13))+(entries[11] * rhs.GetEntry(14))+(entries[15] * rhs.GetEntry(15)));

      return result;
    }

    cMat4 cMat4::operator*(const float rhs) const
    {
      cMat4 result;
      for (size_t i = 0; i < 16; i++) {
        result.SetEntry(i, entries[i] * rhs);    // Multiply entries by rhs
      }
      return result;
    }

    cMat4 cMat4::operator/(const float rhs) const
    {
      cMat4 result;

      // Cannot divide by zero
      if (rhs == 0.0f) {
        result.LoadZero();
        return result;
      }

      const float temp = 1.0f / rhs;

      for (size_t entry = 0; entry < 16; entry++) {
        result.SetEntry(entry, entries[entry] * temp);    // Divide entries by rhs
      }

      return result;
    }

    bool cMat4::operator==(const cMat4& rhs) const
    {
      for (size_t i = 0; i < 16; i++) {
        if (entries[i]!=rhs.entries[i]) return false;
      }

      return true;
    }

    bool cMat4::operator!=(const cMat4& rhs) const
    {
      return !((*this) == rhs);
    }

    void cMat4::operator+=(const cMat4& rhs)
    {
      (*this) = (*this) + rhs;
    }

    void cMat4::operator-=(const cMat4& rhs)
    {
      (*this) = (*this) - rhs;
    }

    void cMat4::operator*=(const cMat4& rhs)
    {
      (*this) = (*this) * rhs;
    }

    void cMat4::operator*=(const float rhs)
    {
      (*this) = (*this) * rhs;
    }

    void cMat4::operator/=(const float rhs)
    {
      (*this) = (*this) / rhs;
    }

    cMat4 cMat4::operator-(void) const
    {
      cMat4 result(*this);

      for (size_t i = 0; i < 16; i++) result.entries[i] = -result.entries[i];

      return result;
    }

    cVec3 cMat4::GetRotatedVec3(const cVec3& rhs) const
    {
      cVec3 result;

      result.x = (entries[0] * rhs.x) + (entries[4] * rhs.y) + (entries[8] * rhs.z);
      result.y = (entries[1] * rhs.x) + (entries[5] * rhs.y) + (entries[9] * rhs.z);
      result.z = (entries[2] * rhs.x) + (entries[6] * rhs.y) + (entries[10] * rhs.z);

      return result;
    }

    cVec4 cMat4::GetRotatedVec4(const cVec4& rhs) const
    {
      cVec4 result;

      result.x = (entries[0] * rhs.GetX()) + (entries[4] * rhs.GetY()) + (entries[8] * rhs.GetZ()) + (entries[12] * rhs.GetW());
      result.y = (entries[1] * rhs.GetX()) + (entries[5] * rhs.GetY()) + (entries[9] * rhs.GetZ()) + (entries[13] * rhs.GetW());
      result.z = (entries[2] * rhs.GetX()) + (entries[6] * rhs.GetY()) + (entries[10] * rhs.GetZ()) + (entries[14] * rhs.GetW());
      result.w = (entries[3] * rhs.GetX()) + (entries[7] * rhs.GetY()) + (entries[11] * rhs.GetZ()) + (entries[15] * rhs.GetW());

      return result;
    }

    cVec3 cMat4::GetInverseRotatedVec3(const cVec3& rhs) const
    {
      cVec3 result;

      // Rotate by transpose
      result.x = (entries[0] * rhs.x) + (entries[1] * rhs.y) + (entries[2] * rhs.z);
      result.y = (entries[4] * rhs.x) + (entries[5] * rhs.y) + (entries[6] * rhs.z);
      result.z = (entries[8] * rhs.x) + (entries[9] * rhs.y) + (entries[10] * rhs.z);

      return result;
    }

    cVec4 cMat4::GetInverseRotatedVec4(const cVec4& rhs) const
    {
      cVec4 result;

      // Rotate by transpose
      result.x = (entries[0] * rhs.x) + (entries[1] * rhs.y) + (entries[2] * rhs.z) + (entries[3] * rhs.w);
      result.y = (entries[4] * rhs.x) + (entries[5] * rhs.y) + (entries[6] * rhs.z) + (entries[7] * rhs.w);
      result.z = (entries[8] * rhs.x) + (entries[9] * rhs.y) + (entries[10] * rhs.z) + (entries[11] * rhs.w);
      result.w = (entries[12] * rhs.x) + (entries[13] * rhs.y) + (entries[14] * rhs.z) + (entries[15] * rhs.w);

      return result;
    }

    cVec3 cMat4::GetTranslatedVec3(const cVec3& rhs) const
    {
      return cVec3(rhs.x + entries[12], rhs.y + entries[13], rhs.z + entries[14]);
    }

    cVec3 cMat4::GetInverseTranslatedVec3(const cVec3& rhs) const
    {
      return cVec3(rhs.x - entries[12], rhs.y - entries[13], rhs.z - entries[14]);
    }

    void cMat4::Invert()
    {
      *this = GetInverse();
    }

    cMat4 cMat4::GetInverse() const
    {
      // Calculate pairs for first 8 elements (cofactors)
      float tmp[12]; //temporary pair storage
      tmp[0] = entries[10] * entries[15];
      tmp[1] = entries[11] * entries[14];
      tmp[2] = entries[9] * entries[15];
      tmp[3] = entries[11] * entries[13];
      tmp[4] = entries[9] * entries[14];
      tmp[5] = entries[10] * entries[13];
      tmp[6] = entries[8] * entries[15];
      tmp[7] = entries[11] * entries[12];
      tmp[8] = entries[8] * entries[14];
      tmp[9] = entries[10] * entries[12];
      tmp[10] = entries[8] * entries[13];
      tmp[11] = entries[9] * entries[12];

      // Calculate first 8 elements (cofactors)
      cMat4 result;
      result.SetEntry(0,    tmp[0] * entries[5] + tmp[3] * entries[6] + tmp[4] * entries[7]
              -  tmp[1] * entries[5] - tmp[2] * entries[6] - tmp[5] * entries[7]);

      result.SetEntry(1,    tmp[1] * entries[4] + tmp[6] * entries[6] + tmp[9] * entries[7]
              -  tmp[0] * entries[4] - tmp[7] * entries[6] - tmp[8] * entries[7]);

      result.SetEntry(2,    tmp[2] * entries[4] + tmp[7] * entries[5] + tmp[10] * entries[7]
              -  tmp[3] * entries[4] - tmp[6] * entries[5] - tmp[11] * entries[7]);

      result.SetEntry(3,    tmp[5] * entries[4] + tmp[8] * entries[5] + tmp[11] * entries[6]
              -  tmp[4] * entries[4] - tmp[9] * entries[5] - tmp[10] * entries[6]);

      result.SetEntry(4,    tmp[1] * entries[1] + tmp[2] * entries[2] + tmp[5] * entries[3]
              -  tmp[0] * entries[1] - tmp[3] * entries[2] - tmp[4] * entries[3]);

      result.SetEntry(5,    tmp[0] * entries[0] + tmp[7] * entries[2] + tmp[8] * entries[3]
              -  tmp[1] * entries[0] - tmp[6] * entries[2] - tmp[9] * entries[3]);

      result.SetEntry(6,    tmp[3] * entries[0] + tmp[6] * entries[1] + tmp[11] * entries[3]
              -  tmp[2] * entries[0] - tmp[7] * entries[1] - tmp[10] * entries[3]);

      result.SetEntry(7,    tmp[4] * entries[0] + tmp[9] * entries[1] + tmp[10] * entries[2]
              -  tmp[5] * entries[0] - tmp[8] * entries[1] - tmp[11] * entries[2]);

      // Calculate pairs for second 8 elements (cofactors)
      tmp[0] = entries[2] * entries[7];
      tmp[1] = entries[3] * entries[6];
      tmp[2] = entries[1] * entries[7];
      tmp[3] = entries[3] * entries[5];
      tmp[4] = entries[1] * entries[6];
      tmp[5] = entries[2] * entries[5];
      tmp[6] = entries[0] * entries[7];
      tmp[7] = entries[3] * entries[4];
      tmp[8] = entries[0] * entries[6];
      tmp[9] = entries[2] * entries[4];
      tmp[10] = entries[0] * entries[5];
      tmp[11] = entries[1] * entries[4];

      // Calculate second 8 elements (cofactors)
      result.SetEntry(8,    tmp[0] * entries[13] + tmp[3] * entries[14] + tmp[4] * entries[15]
              -  tmp[1] * entries[13] - tmp[2] * entries[14] - tmp[5] * entries[15]);

      result.SetEntry(9,    tmp[1] * entries[12] + tmp[6] * entries[14] + tmp[9] * entries[15]
              -  tmp[0] * entries[12] - tmp[7] * entries[14] - tmp[8] * entries[15]);

      result.SetEntry(10,    tmp[2] * entries[12] + tmp[7] * entries[13] + tmp[10] * entries[15]
              -  tmp[3] * entries[12] - tmp[6] * entries[13] - tmp[11] * entries[15]);

      result.SetEntry(11,    tmp[5] * entries[12] + tmp[8] * entries[13] + tmp[11] * entries[14]
              -  tmp[4] * entries[12] - tmp[9] * entries[13] - tmp[10] * entries[14]);

      result.SetEntry(12,    tmp[2] * entries[10] + tmp[5] * entries[11] + tmp[1] * entries[9]
              -  tmp[4] * entries[11] - tmp[0] * entries[9] - tmp[3] * entries[10]);

      result.SetEntry(13,    tmp[8] * entries[11] + tmp[0] * entries[8] + tmp[7] * entries[10]
              -  tmp[6] * entries[10] - tmp[9] * entries[11] - tmp[1] * entries[8]);

      result.SetEntry(14,    tmp[6] * entries[9] + tmp[11] * entries[11] + tmp[3] * entries[8]
              -  tmp[10] * entries[11] - tmp[2] * entries[8] - tmp[7] * entries[9]);

      result.SetEntry(15,    tmp[10] * entries[10] + tmp[4] * entries[8] + tmp[9] * entries[9]
              -  tmp[8] * entries[9] - tmp[11] * entries[10] - tmp[5] * entries[8]);

      // Calculate determinant
      const float determinant =
        entries[0] * result.GetEntry(0) +
        entries[1] * result.GetEntry(1) +
        entries[2] * result.GetEntry(2) +
        entries[3] * result.GetEntry(3)
      ;
      if (determinant == 0.0f) {
        // "Non-Invertible Matrix sent to Invert()"
        cMat4 identity;
        return identity;
      }

      result = result / determinant;

      // Transpose matrix
      result.Transpose();

      return result;
    }


    void cMat4::Transpose()
    {
      *this = GetTranspose();
    }

    cMat4 cMat4::GetTranspose() const
    {
      // Set up temp matrix
      cMat4 temp;
      temp.SetEntry( 0, entries[ 0]);
      temp.SetEntry( 1, entries[ 4]);
      temp.SetEntry( 2, entries[ 8]);
      temp.SetEntry( 3, entries[12]);
      temp.SetEntry( 4, entries[ 1]);
      temp.SetEntry( 5, entries[ 5]);
      temp.SetEntry( 6, entries[ 9]);
      temp.SetEntry( 7, entries[13]);
      temp.SetEntry( 8, entries[ 2]);
      temp.SetEntry( 9, entries[ 6]);
      temp.SetEntry(10, entries[10]);
      temp.SetEntry(11, entries[14]);
      temp.SetEntry(12, entries[ 3]);
      temp.SetEntry(13, entries[ 7]);
      temp.SetEntry(14, entries[11]);
      temp.SetEntry(15, entries[15]);

      return temp;
    }

    void cMat4::InvertTranspose()
    {
      *this = GetInverseTranspose();
    }

    cMat4 cMat4::GetInverseTranspose() const
    {
      // Calculate pairs for first 8 elements (cofactors)
      float tmp[12];                        //temporary pair storage
      tmp[0] = entries[10] * entries[15];
      tmp[1] = entries[11] * entries[14];
      tmp[2] = entries[9] * entries[15];
      tmp[3] = entries[11] * entries[13];
      tmp[4] = entries[9] * entries[14];
      tmp[5] = entries[10] * entries[13];
      tmp[6] = entries[8] * entries[15];
      tmp[7] = entries[11] * entries[12];
      tmp[8] = entries[8] * entries[14];
      tmp[9] = entries[10] * entries[12];
      tmp[10] = entries[8] * entries[13];
      tmp[11] = entries[9] * entries[12];

      // Calculate first 8 elements (cofactors)
      cMat4 result;
      result.SetEntry(0,    tmp[0] * entries[5] + tmp[3] * entries[6] + tmp[4] * entries[7]
              -  tmp[1] * entries[5] - tmp[2] * entries[6] - tmp[5] * entries[7]);

      result.SetEntry(1,    tmp[1] * entries[4] + tmp[6] * entries[6] + tmp[9] * entries[7]
              -  tmp[0] * entries[4] - tmp[7] * entries[6] - tmp[8] * entries[7]);

      result.SetEntry(2,    tmp[2] * entries[4] + tmp[7] * entries[5] + tmp[10] * entries[7]
              -  tmp[3] * entries[4] - tmp[6] * entries[5] - tmp[11] * entries[7]);

      result.SetEntry(3,    tmp[5] * entries[4] + tmp[8] * entries[5] + tmp[11] * entries[6]
              -  tmp[4] * entries[4] - tmp[9] * entries[5] - tmp[10] * entries[6]);

      result.SetEntry(4,    tmp[1] * entries[1] + tmp[2] * entries[2] + tmp[5] * entries[3]
              -  tmp[0] * entries[1] - tmp[3] * entries[2] - tmp[4] * entries[3]);

      result.SetEntry(5,    tmp[0] * entries[0] + tmp[7] * entries[2] + tmp[8] * entries[3]
              -  tmp[1] * entries[0] - tmp[6] * entries[2] - tmp[9] * entries[3]);

      result.SetEntry(6,    tmp[3] * entries[0] + tmp[6] * entries[1] + tmp[11] * entries[3]
              -  tmp[2] * entries[0] - tmp[7] * entries[1] - tmp[10] * entries[3]);

      result.SetEntry(7,    tmp[4] * entries[0] + tmp[9] * entries[1] + tmp[10] * entries[2]
              -  tmp[5] * entries[0] - tmp[8] * entries[1] - tmp[11] * entries[2]);

      // Calculate pairs for second 8 elements (cofactors)
      tmp[0] = entries[2] * entries[7];
      tmp[1] = entries[3] * entries[6];
      tmp[2] = entries[1] * entries[7];
      tmp[3] = entries[3] * entries[5];
      tmp[4] = entries[1] * entries[6];
      tmp[5] = entries[2] * entries[5];
      tmp[6] = entries[0] * entries[7];
      tmp[7] = entries[3] * entries[4];
      tmp[8] = entries[0] * entries[6];
      tmp[9] = entries[2] * entries[4];
      tmp[10] = entries[0] * entries[5];
      tmp[11] = entries[1] * entries[4];

      // Calculate second 8 elements (cofactors)
      result.SetEntry(8,    tmp[0] * entries[13] + tmp[3] * entries[14] + tmp[4] * entries[15]
              -  tmp[1] * entries[13] - tmp[2] * entries[14] - tmp[5] * entries[15]);

      result.SetEntry(9,    tmp[1] * entries[12] + tmp[6] * entries[14] + tmp[9] * entries[15]
              -  tmp[0] * entries[12] - tmp[7] * entries[14] - tmp[8] * entries[15]);

      result.SetEntry(10,    tmp[2] * entries[12] + tmp[7] * entries[13] + tmp[10] * entries[15]
              -  tmp[3] * entries[12] - tmp[6] * entries[13] - tmp[11] * entries[15]);

      result.SetEntry(11,    tmp[5] * entries[12] + tmp[8] * entries[13] + tmp[11] * entries[14]
              -  tmp[4] * entries[12] - tmp[9] * entries[13] - tmp[10] * entries[14]);

      result.SetEntry(12,    tmp[2] * entries[10] + tmp[5] * entries[11] + tmp[1] * entries[9]
              -  tmp[4] * entries[11] - tmp[0] * entries[9] - tmp[3] * entries[10]);

      result.SetEntry(13,    tmp[8] * entries[11] + tmp[0] * entries[8] + tmp[7] * entries[10]
              -  tmp[6] * entries[10] - tmp[9] * entries[11] - tmp[1] * entries[8]);

      result.SetEntry(14,    tmp[6] * entries[9] + tmp[11] * entries[11] + tmp[3] * entries[8]
              -  tmp[10] * entries[11] - tmp[2] * entries[8] - tmp[7] * entries[9]);

      result.SetEntry(15,    tmp[10] * entries[10] + tmp[4] * entries[8] + tmp[9] * entries[9]
              -  tmp[8] * entries[9] - tmp[11] * entries[10] - tmp[5] * entries[8]);

      // Calculate determinant
      const float determinant =
        entries[0] * result.GetEntry(0) +
        entries[1] * result.GetEntry(1) +
        entries[2] * result.GetEntry(2) +
        entries[3] * result.GetEntry(3)
      ;
      if (determinant == 0.0f) {
        // "Non-Invertible Matrix sent to GetInverseTranspose()"
        cMat4 identity;
        return identity;
      }

      result /= determinant;

      return result;
    }


    cMat4 cMat4::Perspective(float fFovYRadians, float fAspectRatio, float fNear, float fFar)
    {
      cMat4 result;
      const float fFovYDegrees = RadiansToDegrees(fFovYRadians);
      result.SetPerspective(fFovYDegrees, fAspectRatio, fNear, fFar);
      return result;
    }

    cMat4 cMat4::LookAt(const cVec3& eye, const cVec3& target, const cVec3& up)
    {
      cMat4 result;
      result.SetLookAt(eye, target, up);
      return result;
    }

    void cMat4::SetPerspective(float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar)
    {
      LoadZero();

      // Check for division by 0
      if ((fLeft == fRight) || (fTop == fBottom) || (fNear == fFar)) return;

      entries[0] = (2.0f * fNear) / (fRight - fLeft);

      entries[5] = (2.0f * fNear) / (fTop - fBottom);

      entries[8] = (fRight + fLeft) / ( fRight - fLeft);
      entries[9] = (fTop + fBottom) / ( fTop - fBottom);

      if (fFar != -1) entries[10] = -(fFar + fNear) / (fFar - fNear);
      else {    //if f == -1, use an infinite far plane
        entries[10] = -1.0f;
      }

      entries[11] = -1.0f;

      if (fFar != -1) entries[14] = -(2.0f * fFar * fNear) / (fFar - fNear);
      else {  //if f==-1, use an infinite far plane
        entries[14] = -2.0f * fNear;
      }
    }

    void cMat4::SetPerspective(float fFovYDegrees, float fAspect, float fNear, float fFar)
    {
      ASSERT(fAspect != 0.0f);
      ASSERT(fFar != fNear);

      LoadIdentity();

      const float fFovRadians = DegreesToRadians(fFovYDegrees);
      const float fTanHalfFovY = tan(fFovRadians / 2.0f);

      SetValue(0, 0, 1.0f / (fAspect * fTanHalfFovY));
      SetValue(1, 1, 1.0f / fTanHalfFovY);
      SetValue(2, 2, - (fFar + fNear) / (fFar - fNear));
      SetValue(2, 3, -1.0f);
      SetValue(3, 2, - (2.0f * fFar * fNear) / (fFar - fNear));
    }

    void cMat4::SetOrtho(float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar)
    {
      // http://stackoverflow.com/a/12230368/1074390

      LoadIdentity();

      entries[0] = 2.0f / (fRight - fLeft);

      entries[5] = 2.0f / (fTop - fBottom);

      entries[10] = -2.0f / (fFar - fNear);

      // Translation
      entries[12] = -(fRight + fLeft) / (fRight - fLeft);
      entries[13] = -(fTop + fBottom) / (fTop - fBottom);
      entries[14] = -(fFar + fNear) / (fFar - fNear);
    }

    void cMat4::SetLookAt(const cVec3& eye, const cVec3& target, const cVec3& up)
    {
      // http://www.3dgep.com/understanding-the-view-matrix/#Look_At_Camera

      const cVec3 vz = (eye - target).GetNormalised();
      const cVec3 vx = (up.CrossProduct(vz)).GetNormalised();
      // vy doesn't need to be normalized because it's a cross
      // product of 2 normalized vectors
      const cVec3 vy = vz.CrossProduct(vx);
      const cMat4 inverseViewMatrix(
        vx.x, vx.y, vx.z, 0.0f,
        vy.x, vy.y, vy.z, 0.0f,
        vz.x, vz.y, vz.z, 0.0f,
        eye.x, eye.y, eye.z, 1.0f
      );

      *this = inverseViewMatrix.GetInverse();
    }

    void cMat4::SetFromQuaternion(const cQuaternion& rhs)
    {
      SetFromMatrix(rhs.GetMatrix());
    }

    void cMat4::SetFromMatrix(const cMat4& rhs)
    {
      for (size_t i = 0; i < 16; i++) entries[i] = rhs.entries[i];
    }

    void cMat4::SetFromMatrix(const cMat3& rhs)
    {
      LoadIdentity();

      entries[0] = rhs.entries[0];
      entries[1] = rhs.entries[1];
      entries[2] = rhs.entries[2];
      entries[4] = rhs.entries[3];
      entries[5] = rhs.entries[4];
      entries[6] = rhs.entries[5];
      entries[8] = rhs.entries[6];
      entries[9] = rhs.entries[7];
      entries[10] = rhs.entries[8];
    }

    void cMat4::SetFromOpenGLMatrix(const float* pEntries)
    {
      assert(pEntries != nullptr);
      for (size_t i = 0; i < 16; i++) entries[i] = pEntries[i];
    }


    // OpenGL matrix operations
    void cMat4::MultiplyMatrix(const cMat4& rhs)
    {
      cMat4 temp;

      for (size_t i = 0; i < 16; i++) {
        temp[i] = 0.0f;

        for(size_t k = 0; k < 4; k++) {
          //               row column               row column
          temp[i] += entries[(i % 4) + (k * 4)] * rhs[k + ((i / 4) * 4)];
        }
      }

      *this = temp;
    }

    void cMat4::TranslateMatrix(const cVec3& translation)
    {
      cMat4 matrix;

      matrix.SetTranslation(translation);

      MultiplyMatrix(matrix);
    }

    void cMat4::RotateMatrix(const cVec3& rotation)
    {
      cMat4 matrix;

      matrix.SetRotationEuler(cPI_DIV_180 * rotation);

      MultiplyMatrix(matrix);
    }

    void cMat4::ScaleMatrix(const cVec3& scale)
    {
      cMat4 matrix;

      matrix.SetScale(scale);

      MultiplyMatrix(matrix);
    }

#ifdef BUILD_DEBUG
    class cUnitTestContext
    {
    public:
      cUnitTestContext();

      void Assert(bool bCondition, const char* szText, const char* szFile, const char* szFunction, int iLine);

      bool IsPassed() const { return !bError; }

    private:
      bool bError;
    };

    cUnitTestContext::cUnitTestContext() :
      bError(false)
    {
    }

    void cUnitTestContext::Assert(bool bCondition, const char* szText, const char* szFile, const char* szFunction, int iLine)
    {
      if (!bCondition) {
        LOGERROR("Unit test condition failed \"", szText, "\" in file \"", szFile, "\", function \"", szFunction, "\", line ", iLine);
        bError = true;
      }
    }
    
#define UTASSERT(b) unitTestContext.Assert(b, #b, __FILE__, __PRETTY_FUNCTION__, __LINE__)

    void UnitTestMat4Translation(cUnitTestContext& unitTestContext)
    {
      const cVec3 translation(1.0f, 2.0f, 3.0f);
      cMat4 matTranslation;
      matTranslation.SetTranslation(translation);

      // Check the resulting translation
      UTASSERT(matTranslation.GetTranslation().IsApproximatelyEqual(translation));

      // Check each translation element
      UTASSERT(IsApproximatelyEqual(matTranslation.GetEntry(12), translation.x));
      UTASSERT(IsApproximatelyEqual(matTranslation.GetEntry(13), translation.y));
      UTASSERT(IsApproximatelyEqual(matTranslation.GetEntry(14), translation.z));
    }

    void UnitTestMat4()
    {
      cUnitTestContext unitTestContext;
      UnitTestMat4Translation(unitTestContext);
      if (unitTestContext.IsPassed()) LOG("Unit test passed");
      else LOGERROR("Unit test failed");
    }
#endif
  }
}
