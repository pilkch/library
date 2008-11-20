#ifndef CQUATERNION_H
#define CQUATERNION_H

// TODO: Look at moving to at least wrapping Boost.Quaternion
// http://www.boost.org/doc/libs/1_37_0/libs/math/doc/quaternion/html/index.html

// A quaternion stores a rotation of t (ccw) about (a, b, c) (normalised) as:
// x = sin(t/2) * a
// y = sin(t/2) * b
// z = sin(t/2) * c
// w = cos(t/2)

namespace breathe
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
        w(rhs.w),
        x(rhs.x),
        y(rhs.y),
        z(rhs.z)
      {
      }

      cQuaternion(float x0, float y0, float z0, float w0) :
        w(w0),
        x(x0),
        y(y0),
        z(z0)
      {
      }

      // Retrieves values from angle of rotation about x, y, z axes
      cQuaternion(const cVec3& angleOfRotationAboutXYZ)
      {
        SetFromAngles(angleOfRotationAboutXYZ);
      }

      // Retrieves values from linear interpolation betwwen 2 quaternions
      cQuaternion(const cQuaternion& q1, const cQuaternion& q2, float interpolation)
      {
        Slerp(q1, q2, interpolation);
      }

      // Load the identity quaternion
      void LoadIdentity();

      void Normalise();

      // Retrieve axis/angle
      float GetAngle() const;
      cVec3 GetAxis() const;
      cVec3 GetEuler() const;

      cMat4 GetMatrix() const;

      cQuaternion Inverse() const;
      cQuaternion Conjugate() const;

      // Set from 2 quaternions
      // If the second is backwards, that is ok
      void Slerp(const cQuaternion& q1, const cQuaternion& q2, const float interpolation);

      // Invert the quaternion
      void Invert();

      // Set from angles of rotation about x, y, z axes
      void SetFromAngles(const cVec3& v);

      // Set from axis-angle combination
      void SetFromAxisAngle(const cVec3& axis, float angle);

      // Set from vectors (source and destination)
      void SetFromVectors(const cVec3& source, const cVec3& destination);

      // Set from ODE style quaternion (w, x, y, z)
      void SetFromODEQuaternion(const float* q);

      cQuaternion& operator=(const cQuaternion& rhs);
      cQuaternion operator*(const cQuaternion& rhs) const;


      // TODO: Think about switching these around to (x, y, z, w), which is more common?
      float w;
      float x;
      float y;
      float z;
    };

    // ** Inlines

    inline cQuaternion& cQuaternion::operator=(const cQuaternion& rhs)
    {
      w = rhs.w;
      x = rhs.x;
      y = rhs.y;
      z = rhs.z;

      return *this;
    }

    inline cQuaternion cQuaternion::operator*(const cQuaternion& rhs) const
    {
      cQuaternion q(
        w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
        w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
        w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x,
        w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z
      );

      return q;
    }
  }
}

#endif // CQUATERNION_H
