#include <cstdlib>
#include <cmath>

#include <vector>
#include <limits>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cColour.h>

namespace spitfire
{
  namespace math
  {
    cVec3 cVec3::GetEdgeVector(const cVec3& point2) const
    {
      cVec3 temp_vector;
      temp_vector.x = x - point2.x;
      temp_vector.y = y - point2.y;
      temp_vector.z = z - point2.z;
      return temp_vector;
    }

    float cVec3::GetSquaredLength() const
    {
      return (x * x) + (y * y) + (z * z);
    }

    float cVec3::GetLength() const
    {
      return sqrtf(GetSquaredLength());
    }

    float cVec3::GetMagnitude() const
    {
      return GetLength();
    }

    bool cVec3::IsZeroVector() const
    {
      return (
        (x < cEPSILON) && (x > -cEPSILON) &&
        (y < cEPSILON) && (y > -cEPSILON) &&
        (z < cEPSILON) && (z > -cEPSILON)
      );
    }

    bool cVec3::IsWithinBounds(const cVec3& minimum, const cVec3& maximum) const
    {
      return (
        (x >= minimum.x) && (x <= maximum.x) &&
        (y >= minimum.y) && (y <= maximum.y) &&
        (z >= minimum.z) && (z <= maximum.z)
      );
    }

    // TODO: Inline this
    void cVec3::SetOne(void)
    {
      x=1.0f;
      y=1.0f;
      z=1.0f;
    }

    void cVec3::Negate(void)
    {
      x=-x;
      y=-y;
      z=-z;
    }

    void cVec3::Cross(const cVec3& a, const cVec3& b)
    {
      x = a.y*b.z - a.z*b.y;
      y = a.z*b.x - a.x*b.z;
      z = a.x*b.y - a.y*b.x;
    }

    cVec3 cVec3::CrossProduct(const cVec3& rhs) const
    {
      cVec3 result;

      result.x = y*rhs.z - z*rhs.y;
      result.y = z*rhs.x - x*rhs.z;
      result.z = x*rhs.y - y*rhs.x;

      return result;
    }

    void cVec3::Invert()
    {
      x = -x;
      y = -y;
      z = -z;
    }

    void cVec3::Normalise()
    {
      const float fLength = GetLength();

      // Return if length is 1 or 0
      if ((fLength == 1.0f) || (fLength == 0.0f)) return;

      const float scalefactor = 1.0f / fLength;

      if ((cEPSILON > x) && (-cEPSILON < x)) x = 0.0f;
      else x *= scalefactor;

      if ((cEPSILON > y) && (-cEPSILON < y)) y = 0.0f;
      else y *= scalefactor;

      if ((cEPSILON > z) && (-cEPSILON < z)) z = 0.0f;
      else z *= scalefactor;
    }

    void cVec3::SetLength(float fLength)
    {
      // Return if length is 1 or 0
      if (fLength == 1.0f) return;

      if (fLength == 0.0f) {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
        return;
      }

      const float scalefactor = 1.0f / fLength;
      x *= scalefactor;
      y *= scalefactor;
      z *= scalefactor;
    }

    cVec3 cVec3::SetLength(float length) const
    {
      cVec3 result(*this);

      result.SetLength(length);

      return result;
    }

    cVec3 cVec3::GetInverse() const
    {
      cVec3 result(*this);

      result.Invert();

      return result;
    }

    cVec3 cVec3::GetNormalised() const
    {
      cVec3 result(*this);

      result.Normalise();

      return result;
    }

    cVec3 cVec3::GetRotatedX(double angle) const
    {
      return cVec3(
        x,
        y * cosf((float)angle) - z * sinf((float)angle),
        y * sinf((float)angle) + z * cosf((float)angle)
      );
    }

    void cVec3::RotateX(double angle)
    {
      (*this) = GetRotatedX(angle);
    }

    cVec3 cVec3::GetRotatedY(double angle) const
    {
      return cVec3(
        x * cosf((float)angle) + z * sinf((float)angle),
        y,
        -x * sinf((float)angle) + z * cosf((float)angle)
      );
    }

    void cVec3::RotateY(double angle)
    {
      (*this) = GetRotatedY(angle);
    }

    cVec3 cVec3::GetRotatedZ(double angle) const
    {
      return cVec3(
        x * cosf((float)angle) - y * sinf((float)angle),
        x * sinf((float)angle) + y * cosf((float)angle),
        z
      );
    }

    void cVec3::RotateZ(double angle)
    {
      (*this) = GetRotatedZ(angle);
    }

    cVec3 cVec3::GetRotatedAxis(double angle, const cVec3& axis) const
    {
      const cVec3 u = axis.GetNormalised();

      const float sinAngle = sinf((float)angle);
      const float cosAngle = cosf((float)angle);
      const float oneMinusCosAngle = 1.0f - cosAngle;

      cVec3 rotMatrixRow0;
      rotMatrixRow0.x = (u.x) * (u.x) + cosAngle * (1.0f - (u.x * u.x));
      rotMatrixRow0.y = (u.x) * (u.y) * (oneMinusCosAngle) - sinAngle * u.z;
      rotMatrixRow0.z = (u.x) * (u.z) * (oneMinusCosAngle) + sinAngle * u.y;

      cVec3 rotMatrixRow1;
      rotMatrixRow1.x = (u.x) * (u.y) * (oneMinusCosAngle) + sinAngle * u.z;
      rotMatrixRow1.y = (u.y) * (u.y) + cosAngle * (1.0f - (u.y * u.y));
      rotMatrixRow1.z = (u.y) * (u.z) * (oneMinusCosAngle) - sinAngle * u.x;

      cVec3 rotMatrixRow2;
      rotMatrixRow2.x = (u.x) * (u.z) * (oneMinusCosAngle) - sinAngle * u.y;
      rotMatrixRow2.y = (u.y) * (u.z) * (oneMinusCosAngle) + sinAngle * u.x;
      rotMatrixRow2.z = (u.z) * (u.z) + cosAngle * (1.0f - (u.z * u.z));

      cVec3 result;

      result.x = DotProduct(rotMatrixRow0);
      result.y = DotProduct(rotMatrixRow1);
      result.z = DotProduct(rotMatrixRow2);

      return result;
    }

    void cVec3::RotateAxis(double angle, const cVec3& axis)
    {
      (*this) = GetRotatedAxis(angle, axis);
    }

    void cVec3::RotateByQuaternion(const cQuaternion& rhs)
    {
      RotateAxis(rhs.GetAngle(), rhs.GetAxis());
    }

    cVec3 cVec3::GetRotatedByQuaternion(const cQuaternion& rhs) const
    {
      return cVec3(GetRotatedAxis(rhs.GetAngle(), rhs.GetAxis()));
    }

    void cVec3::PackTo01()
    {
      (*this) = GetPackedTo01();
    }

    cVec3 cVec3::GetPackedTo01() const
    {
      cVec3 temp(*this);

      temp.Normalise();

      temp = temp * 0.5f + cVec3(0.5f, 0.5f, 0.5f);

      return temp;
    }

    cVec3 cVec3::lerp(const cVec3& v2, float factor) const
    {
      cVec3 result;

      result = ((*this) * factor) + (v2 * (1.0f - factor));

      return result;
    }

    cVec3 cVec3::operator*(const float rhs) const
    {
      float newX = x * rhs;
      float newY = y * rhs;
      float newZ = z * rhs;

      return cVec3(newX, newY, newZ);
    }

    bool cVec3::operator==(const cVec3& rhs) const
    {
      return ((x == rhs.x) && (y==rhs.y) && (z==rhs.z));
    }

    bool cVec3::operator!=(const cVec3& rhs) const
    {
      return !((x == rhs.x) && (y==rhs.y) && (z==rhs.z));
    }

    void cVec3::operator-=(const cVec3& rhs)
    {
      x -= rhs.x;
      y -= rhs.y;
      z -= rhs.z;
    }

    void cVec3::operator*=(const float rhs)
    {
      x *= rhs;
      y *= rhs;
      z *= rhs;
    }

    void cVec3::operator/=(const float rhs)
    {
      x /= rhs;
      y /= rhs;
      z /= rhs;
    }
  }
}
