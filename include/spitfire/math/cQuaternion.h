#ifndef CQUATERNION_H
#define CQUATERNION_H

#include <spitfire/math/cMat4.h>

// https://svn.ncsa.uiuc.edu/svn/mworlds/musiverse_text/server/ogre/OgreQuaternion.cpp

// A quaternion stores a rotation of t (ccw) about (a, b, c) (normalised) as:
// x = sin(t/2) * a
// y = sin(t/2) * b
// z = sin(t/2) * c
// w = cos(t/2)

// TODO: Look at moving to at least wrapping Boost.Quaternion
// http://www.boost.org/doc/libs/1_37_0/libs/math/doc/quaternion/html/index.html

namespace spitfire
{
  namespace math
  {
    class cQuaternion
    {
    public:
      cQuaternion()
      {
        LoadIdentity();
      }

      cQuaternion(const cQuaternion& rhs) :
        x(rhs.x),
        y(rhs.y),
        z(rhs.z),
        w(rhs.w)
      {
      }

      cQuaternion(float x0, float y0, float z0, float w0) :
        x(x0),
        y(y0),
        z(z0),
        w(w0)
      {
      }

      // Retrieves values from angle of rotation about x, y, z axes
      //explicit cQuaternion(const cVec3& angleOfRotationAboutXYZ)
      //{
      //  SetFromAngles(angleOfRotationAboutXYZ);
      //}

      // Retrieves values from linear interpolation betwwen 2 quaternions
      cQuaternion(const cQuaternion& q1, const cQuaternion& q2, float interpolation)
      {
        Slerp(q1, q2, interpolation);
      }

      // Load the identity quaternion
      void LoadIdentity();

      void Normalise();

      // Retrieve axis/angle
      float GetAngleDegrees() const;
      float GetAngle() const { return GetAngleDegrees(); } // Deprecated
      cVec3 GetAxis() const;
      cVec3 GetEuler() const;

      cMat4 GetMatrix() const;

      cVec3 GetRotatedVector(const cVec3& value) const;

      cQuaternion GetInverse() const;
      cQuaternion GetConjugate() const;

      // Set spherical linear interpolated quaternion between q1 and q2, with respect to t
      // If the second is backwards, that is ok
      void Slerp(const cQuaternion& q1, const cQuaternion& q2, const float interpolation);

      // Invert the quaternion
      void Invert();

      void SetFromQuaternion(const cQuaternion& rhs)
      {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        w = rhs.w;
      }

      void SetFromMatrix(const cMat4& rhs);

      // Set from angles of rotation about x, y, z axes
      void SetFromAngles(const cVec3& v);

      // Set from axis-angle combination
      void SetFromAxisAngle(const cVec3& axis, float fAngleRadians);
      void SetFromAxisAngleDegrees(const cVec3& axis, float fAngleDegrees);

      // Set from vectors (source and destination)
      void SetFromVectors(const cVec3& source, const cVec3& destination);

      // Set from ODE style quaternion (w, x, y, z)
      void SetFromODEQuaternion(const float* q);

      // Gets an ODE Quaternion from our values
      void GetODEQuaternion(float* q) const;

      cQuaternion& operator=(const cQuaternion& rhs);

      cQuaternion operator-() const;

      cQuaternion operator*(const cQuaternion& rhs) const;

      void operator*=(const cQuaternion& rhs) { (*this) = (*this) * rhs; }

      // Commented out as these are not intuitive
      //cQuaternion operator+(const cQuaternion& rhs) const;
      //cQuaternion operator-(const cQuaternion& rhs) const;
      //void operator+=(const cQuaternion& rhs) { (*this) = (*this) + rhs; }
      //void operator-=(const cQuaternion& rhs) { (*this) = (*this) - rhs; }

      cVec3 operator*(const cVec3& rhs) const { return GetRotatedVector(rhs); }


      // TODO: Remove these
      void SlerpForMD3(const cQuaternion& q1, const cQuaternion& q2, float t);

      // This takes in an array of 16 floats to fill in a 4x4 homogeneous matrix from a quaternion
      cMat4 GetMD3Matrix() const;

      // This takes a 3x3 or 4x4 matrix and converts it to a quaternion, depending on rowColumnCount
      void SetFromMD3Matrix(const float* pMatrix, int rowColumnCount);


      float x;
      float y;
      float z;
      float w;
    };

    // ** Inlines

    inline cQuaternion& cQuaternion::operator=(const cQuaternion& rhs)
    {
      SetFromQuaternion(rhs);
      return *this;
    }

    // These are all the same thing
    inline cQuaternion cQuaternion::GetInverse() const
    {
      return cQuaternion(-x, -y, -z, w);
    }

    inline cQuaternion cQuaternion::GetConjugate() const
    {
      return cQuaternion(-x, -y, -z, w);
    }

    inline cQuaternion cQuaternion::operator-() const
    {
      return cQuaternion(-x, -y, -z, w);
    }

    // Commented out as these are not intuitive
    //inline cQuaternion cQuaternion::operator+(const cQuaternion& rhs) const
    //{
    //  return cQuaternion(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
    //}

    //inline cQuaternion cQuaternion::operator-(const cQuaternion& rhs) const
    //{
    //  return cQuaternion(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
    //}

    inline cQuaternion cQuaternion::operator*(const cQuaternion& rhs) const
    {
      cQuaternion q(
        (w * rhs.x) + (x * rhs.w) + (y * rhs.z) - (z * rhs.y), // x
        (w * rhs.y) + (y * rhs.w) + (z * rhs.x) - (x * rhs.z), // y
        (w * rhs.z) + (z * rhs.w) + (x * rhs.y) - (y * rhs.x), // z
        (w * rhs.w) - (x * rhs.x) - (y * rhs.y) - (z * rhs.z)  // w
      );

      return q;
    }
  }
}

#endif // CQUATERNION_H
