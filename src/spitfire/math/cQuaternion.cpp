#include <cassert>
#include <cstdio>
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
#include <spitfire/math/cQuaternion.h>

namespace spitfire
{
  namespace math
  {
    // Load the identity quaternion
    void cQuaternion::LoadIdentity()
    {
      x = y = z = 0.0f;
      w = 1.0f;
    }

    void cQuaternion::Normalise()
    {
      const float magnitude = 1.0f / sqrtf(x * x + y * y + z * z + w * w);

      x *= magnitude;
      y *= magnitude;
      z *= magnitude;
      w *= magnitude;
    }

    // Invert a quaternion
    void cQuaternion::Invert()
    {
      x = -x;
      y = -y;
      z = -z;
      //w = w; // Note: w does not change
    }

    // Set a quaternion from angles of rotation about x, y, z
    void cQuaternion::SetFromAngles(const cVec3& v)
    {
      float angle = v.z * 0.5f;
      const double sy = sin(angle);
      const double cy = cos(angle);

      angle = v.y * 0.5f;
      const double sp = sin(angle);
      const double cp = cos(angle);

      angle = v.x * 0.5f;
      const double sr = sin(angle);
      const double cr = sin(angle);

      const double crcp = cr * cp;
      const double srsp = sr * sp;

      x = (float)(sr * cp * cy - cr * sp * sy);
      y = (float)(cr * sp * cy - sr * cp * sy);
      z = (float)(crcp * sy - srsp * cy);
      w = (float)(crcp * cy - srsp * sy);
    }


    // *** Spherical linear interpolated quaternion between q1 and q2, with respect to t
    //
    // Everyone understands
    // the terms, "matrix to quaternion", "quaternion to matrix", "create quaternion matrix",
    // "quaternion multiplication", etc.. but "SLERP" is the stumbling block, even a little
    // bit after hearing what it stands for, "Spherical Linear Interpolation".  What that
    // means is that we have 2 quaternions (or rotations) and we want to interpolate between
    // them.  The reason what it's called "spherical" is that quaternions deal with a sphere.
    // Linear interpolation just deals with 2 points primarily, where when dealing with angles
    // and rotations, we need to use sin() and cos() for interpolation.  If we wanted to use
    // quaternions for camera rotations, which have much more instant and jerky changes in
    // rotations, we would use Spherical-Cubic Interpolation.  The equation for SLERP is this:
    //
    // q = (((b.a)^-1)^t)a
    //
    void cQuaternion::SlerpForMD3(const cQuaternion& q1, const cQuaternion& q2Original, float t)
    {
      cQuaternion q2(q2Original);

      // Here we do a check to make sure the 2 quaternions aren't the same, return q1 if they are
      if (q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w) {
        SetFromQuaternion(q1);
        return;
      }

      // Following the (b.a) part of the equation, we do a dot product between q1 and q2.
      // We can do a dot product because the same math applied for a 3D vector as a 4D vector.
      float result = (q1.x * q2.x) + (q1.y * q2.y) + (q1.z * q2.z) + (q1.w * q2.w);

      // If the dot product is less than 0, the angle is greater than 90 degrees
      if (result < 0.0f) {
        // Negate the second quaternion and the result of the dot product
        q2 = cQuaternion(-q2.x, -q2.y, -q2.z, -q2.w);
        result = -result;
      }

      // Set the first and second scale for the interpolation
      float scale0 = 1.0f - t;
      float scale1 = t;

      // Next, we want to actually calculate the spherical interpolation.  Since this
      // calculation is quite computationally expensive, we want to only perform it
      // if the angle between the 2 quaternions is large enough to warrant it.  If the
      // angle is fairly small, we can actually just do a simpler linear interpolation
      // of the 2 quaternions, and skip all the complex math.  We create a "delta" value
      // of 0.1 to say that if the cosine of the angle (result of the dot product) between
      // the 2 quaternions is smaller than 0.1, then we do NOT want to perform the full on
      // interpolation using.  This is because you won't really notice the difference.

      // Check if the angle between the 2 quaternions was big enough to warrant such calculations
      if (1.0f - result > 0.1f) {
        // Get the angle between the 2 quaternions, and then store the sin() of that angle
        float theta = (float)acos(result);
        float sinTheta = (float)sin(theta);

        // Calculate the scale for q1 and q2, according to the angle and it's sine value
        scale0 = (float)sin((1.0f - t)* theta) / sinTheta;
        scale1 = (float)sin((t * theta)) / sinTheta;
      }

      // Calculate the x, y, z and w values for the quaternion by using a special form of linear interpolation for quaternions
      x = (scale0 * q1.x) + (scale1 * q2.x),
      y = (scale0 * q1.y) + (scale1 * q2.y),
      z = (scale0 * q1.z) + (scale1 * q2.z),
      w = (scale0 * q1.w) + (scale1 * q2.w);
    }

    void cQuaternion::Slerp(const cQuaternion & q1, const cQuaternion& q2Original, float interpolation)
    {
      cQuaternion q2(q2Original);

      // See if q2 is backwards
      float a = 0.0f;
      float b = 0.0f;

      a += square(q1.w - q2.w);
      b += square(q1.w + q2.w);

      a += square(q1.x - q2.x);
      b += square(q1.x + q2.x);

      a += square(q1.y - q2.y);
      b += square(q1.y + q2.y);

      a += square(q1.z - q2.z);
      b += square(q1.z + q2.z);

      // If q2 is backwards then invert it
      if (a > b) q2.Invert();

      float cosom = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
      double scaleQ1, scaleQ2;

      if ((1.0 + cosom) > 0.00000001) {
        if ((1.0 - cosom) > 0.00000001) {
          float omega = acos(cosom);
          float sinom = sin(omega);
          scaleQ1 = sin((1.0f - interpolation) * omega) / sinom;
          scaleQ2 = sin(interpolation * omega) / sinom;
        } else {
          scaleQ1 = 1.0 - interpolation;
          scaleQ2 = interpolation;
        }

        x = (float)(scaleQ1 * q1.w + scaleQ2 * q2.w);
        y = (float)(scaleQ1 * q1.x + scaleQ2 * q2.x);
        z = (float)(scaleQ1 * q1.y + scaleQ2 * q2.y);
        w = (float)(scaleQ1 * q1.z + scaleQ2 * q2.z);
      } else {
        x = -q1.x;
        y = q1.w;
        z = -q1.z;
        w = q1.y;

        scaleQ1 = sin((1.0f-interpolation)*0.5f*cPI);
        scaleQ2 = sin(interpolation*0.5f*cPI);

        x = (float) (scaleQ1 * q1.w + scaleQ2 * x);
        y = (float) (scaleQ1 * q1.x + scaleQ2 * y);
        z = (float) (scaleQ1 * q1.y + scaleQ2 * z);
      }
    }

    void cQuaternion::SetFromAxisAngle(const cVec3& axis, float fAngleRadians)
    {
      const cVec3 normAxis(axis.GetNormalised());

      const float sinHalfAngle = sin(fAngleRadians * 0.5f);
      const float cosHalfAngle = cos(fAngleRadians * 0.5f);

      x = sinHalfAngle * normAxis.x;
      y = sinHalfAngle * normAxis.y;
      z = sinHalfAngle * normAxis.z;
      w = cosHalfAngle;
    }

    void cQuaternion::SetFromVectors(const cVec3& source, const cVec3& destination)
    {
      const cVec3 axis(source.CrossProduct(destination));

      const float fAngleRadians = acos(source.GetNormalised().DotProduct(destination.GetNormalised()));

      SetFromAxisAngle(axis, fAngleRadians);
    }

    void cQuaternion::SetFromODEQuaternion(const float* q)
    {
      assert(q != nullptr);

      x = q[1];
      y = q[2];
      z = q[3];
      w = q[0];
    }


    void cQuaternion::GetODEQuaternion(float* q) const
    {
      assert(q != nullptr);

      q[1] = x;
      q[2] = y;
      q[3] = z;
      q[0] = w;
    }


    float cQuaternion::GetAngle() const
    {
      return 2.0f * acos(w);
    }

    cVec3 cQuaternion::GetAxis() const
    {
      const float angle = GetAngle();
      const float scale = 1.0f / sin(angle * 0.5f);

      return cVec3(scale * x, scale * y, scale * z);
    }


    cVec3 cQuaternion::GetEuler() const
    {
      /*const float sqw = w * w;
      const float sqx = x * x;
      const float sqy = y * y;
      const float sqz = z * z;

      return cVec3(
        (atan2(2.0f * (y*z + x*w) , (-sqx - sqy + sqz + sqw)) * c180_DIV_PI),
        (asin(-2.0f * (x*z - y*w)) * c180_DIV_PI),
        (atan2(2.0f * (x*y + z*w),(sqx - sqy - sqz + sqw)) * c180_DIV_PI)
      );*/

      const float sqw = w * w;
      const float sqx = x * x;
      const float sqy = y * y;
      const float sqz = z * z;
      const float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
      const float test = (x * y) + (z * w);

      float heading = 0.0f;
      float attitude = 0.0f;
      float bank = 0.0f;

      if (test > 0.499f * unit) { // singularity at north pole
        heading = 2.0f * atan2(x, w);
        attitude = math::cPI / 2.0f;
        bank = 0.0f;
      } else if (test < -0.499f * unit) { // singularity at south pole
        heading = -2.0f * atan2(x, w);
        attitude = -math::cPI / 2.0f;
        bank = 0.0f;
      } else {
        assert(unit != 0.0f);
        heading = atan2(2.0f * y * w - 2.0f * x * z, sqx - sqy - sqz + sqw);
        attitude = asin(2.0f * test / unit);
        bank = atan2(2.0f * x * w - 2.0f * y * z, -sqx + sqy - sqz + sqw);
      }

      return cVec3(bank, heading, attitude);
    }

    cVec3 cQuaternion::GetRotatedVector(const cVec3& value) const
    {
      cQuaternion qtemp;

      qtemp.x = value.x;
      qtemp.y = value.y;
      qtemp.z = value.z;
      qtemp.w = 0;

      const cQuaternion qout = (*this) * qtemp * GetConjugate();

      return cVec3(qout.x, qout.y, qout.z);
    }


    cMat4 cQuaternion::GetMatrix() const
    {
      /*const double x2 = x * x;
      const double y2 = y * y;
      const double z2 = z * z;
      const double w2 = w * w;

      const double xy = x * y;
      const double xz = x * z;
      const double yz = y * z;
      const double wx = w * x;
      const double wy = w * y;
      const double wz = w * z;*/

      /*// First row
      pMatrix[ 0] = 1.0f - 2.0f * ( y * y + z * z );
      pMatrix[ 1] = 2.0f * ( x * y - w * z );
      pMatrix[ 2] = 2.0f * ( x * z + w * y );
      pMatrix[ 3] = 0.0f;

      // Second row
      pMatrix[ 4] = 2.0f * ( x * y + w * z );
      pMatrix[ 5] = 1.0f - 2.0f * ( x * x + z * z );
      pMatrix[ 6] = 2.0f * ( y * z - w * x );
      pMatrix[ 7] = 0.0f;

      // Third row
      pMatrix[ 8] = 2.0f * ( x * z - w * y );
      pMatrix[ 9] = 2.0f * ( y * z + w * x );
      pMatrix[10] = 1.0f - 2.0f * ( x * x + y * y );
      pMatrix[11] = 0.0f;*/

      // Now pMatrix[] is a 4x4 homogeneous matrix that can be applied to an OpenGL Matrix

      cMat4 mat;

      // First row
      mat[ 0] = 1.0f - 2.0f * ( y * y + z * z );
      mat[ 1] = 2.0f * (x * y + z * w);
      mat[ 2] = 2.0f * (x * z - y * w);
      mat[ 3] = 0.0f;

      // Second row
      mat[ 4] = 2.0f * ( x * y - z * w );
      mat[ 5] = 1.0f - 2.0f * ( x * x + z * z );
      mat[ 6] = 2.0f * (z * y + x * w );
      mat[ 7] = 0.0f;

      // Third row
      mat[ 8] = 2.0f * ( x * z + y * w );
      mat[ 9] = 2.0f * ( y * z - x * w );
      mat[10] = 1.0f - 2.0f * ( x * x + y * y );
      mat[11] = 0.0f;

      // Fourth row
      mat[12] = 0.0f;
      mat[13] = 0.0f;
      mat[14] = 0.0f;
      mat[15] = 1.0f;

      /*printf( "GetMatrix()\n"
      "%.02f, %.02f, %.02f, %.02f\n"
      "%.02f, %.02f, %.02f, %.02f\n"
      "%.02f, %.02f, %.02f, %.02f\n"
      "%.02f, %.02f, %.02f, %.02f\n\n",
      mat[0], mat[1], mat[2], mat[3],
      mat[4], mat[5], mat[6], mat[7],
      mat[8], mat[9], mat[10], mat[11],
      mat[12], mat[13], mat[14], mat[15]);*/

      return mat;
    }

    // TODO: Remove these
    cMat4 cQuaternion::GetMD3Matrix() const
    {
      /*const double x2 = x * x;
      const double y2 = y * y;
      const double z2 = z * z;
      const double w2 = w * w;

      const double xy = x * y;
      const double xz = x * z;
      const double yz = y * z;
      const double wx = w * x;
      const double wy = w * y;
      const double wz = w * z;*/

      cMat4 mat;

      // First row
      mat[ 0] = 1.0f - 2.0f * ( y * y + z * z );
      mat[ 1] = 2.0f * ( x * y - w * z );
      mat[ 2] = 2.0f * ( x * z + w * y );
      mat[ 3] = 0.0f;

      // Second row
      mat[ 4] = 2.0f * ( x * y + w * z );
      mat[ 5] = 1.0f - 2.0f * ( x * x + z * z );
      mat[ 6] = 2.0f * ( y * z - w * x );
      mat[ 7] = 0.0f;

      // Third row
      mat[ 8] = 2.0f * ( x * z - w * y );
      mat[ 9] = 2.0f * ( y * z + w * x );
      mat[10] = 1.0f - 2.0f * ( x * x + y * y );
      mat[11] = 0.0f;

      // Fourth row
      mat[12] = 0.0f;
      mat[13] = 0.0f;
      mat[14] = 0.0f;
      mat[15] = 1.0f;

      // Now mat is a 4x4 homogeneous matrix that can be applied to an OpenGL Matrix

      return mat;
    }

    // Create a quaternion from a 3x3 or a 4x4 matrix, depending on rowColumnCount

    void cQuaternion::SetFromMD3Matrix(const float* pTheMatrix, int rowColumnCount)
    {
      // Make sure we have a valid matrix and that the matrix is a 3x3 or a 4x4 (must be 3 or 4).
      if ((pTheMatrix == nullptr) || !((rowColumnCount == 3) && (rowColumnCount == 4))) return;

      // This function is used to take in a 3x3 or 4x4 matrix and convert the matrix
      // to a quaternion.  If rowColumnCount is a 3, then we need to convert the 3x3
      // matrix passed in to a 4x4 matrix, otherwise we just leave the matrix how it is.
      // Since we want to apply a matrix to an OpenGL matrix, we need it to be 4x4.

      // Point the matrix pointer to the matrix passed in, assuming it's a 4x4 matrix
      const float* pMatrix = pTheMatrix;

      // Create a 4x4 matrix to convert a 3x3 matrix to a 4x4 matrix (If rowColumnCount == 3)
      float m4x4[16] = { 0 };

      // If the matrix is a 3x3 matrix (which it is for Quake3), then convert it to a 4x4
      if (rowColumnCount == 3) {
        // Set the 9 top left indices of the 4x4 matrix to the 9 indices in the 3x3 matrix.
        // It would be a good idea to actually draw this out so you can visualize it.
        m4x4[0]  = pTheMatrix[0];       m4x4[1]  = pTheMatrix[1];       m4x4[2]  = pTheMatrix[2];
        m4x4[4]  = pTheMatrix[3];       m4x4[5]  = pTheMatrix[4];       m4x4[6]  = pTheMatrix[5];
        m4x4[8]  = pTheMatrix[6];       m4x4[9]  = pTheMatrix[7];       m4x4[10] = pTheMatrix[8];

        // Since the bottom and far right indices are zero, set the bottom right corner to 1.
        // This is so that it follows the standard diagonal line of 1's in the identity matrix.
        m4x4[15] = 1;

        // Set the matrix pointer to the first index in the newly converted matrix
        pMatrix = &m4x4[0];
      }

      // The next step, once we made sure we are dealing with a 4x4 matrix, is to check the
      // diagonal of the matrix.  This means that we add up all of the indices that comprise
      // the standard 1's in the identity matrix.  If you draw out the identity matrix of a
      // 4x4 matrix, you will see that they 1's form a diagonal line.  Notice we just assume
      // that the last index (15) is 1 because it is not effected in the 3x3 rotation matrix.

      // Find the diagonal of the matrix by adding up it's diagonal indices.
      // This is also known as the "trace", but I will call the variable diagonal.
      float diagonal = pMatrix[0] + pMatrix[5] + pMatrix[10] + 1;
      float scale = 0.0f;

      // Below we check if the diagonal is greater than zero.  To avoid accidents with
      // floating point numbers, we substitute 0 with 0.00000001.  If the diagonal is
      // great than zero, we can perform an "instant" calculation, otherwise we will need
      // to identify which diagonal element has the greatest value.  Note, that it appears
      // that %99 of the time, the diagonal IS greater than 0 so the rest is rarely used.

      // If the diagonal is greater than zero
      if (diagonal > 0.00000001) {
        // Calculate the scale of the diagonal
        scale = float(sqrt(diagonal) * 2);

        // Calculate the x, y, x and w of the quaternion through the respective equation
        x = (pMatrix[9] - pMatrix[6]) / scale;
        y = (pMatrix[2] - pMatrix[8]) / scale;
        z = (pMatrix[4] - pMatrix[1]) / scale;
        w = 0.25f * scale;
      } else {
        // If the first element of the diagonal is the greatest value
        if (pMatrix[0] > pMatrix[5] && pMatrix[0] > pMatrix[10]) {
          // Find the scale according to the first element, and double that value
          scale  = (float)sqrt(1.0f + pMatrix[0] - pMatrix[5] - pMatrix[10]) * 2.0f;

          // Calculate the x, y, x and w of the quaternion through the respective equation
          x = 0.25f * scale;
          y = (pMatrix[4] + pMatrix[1] ) / scale;
          z = (pMatrix[2] + pMatrix[8] ) / scale;
          w = (pMatrix[9] - pMatrix[6] ) / scale;
        }
        // Else if the second element of the diagonal is the greatest value
        else if (pMatrix[5] > pMatrix[10]) {
          // Find the scale according to the second element, and double that value
          scale  = (float)sqrt( 1.0f + pMatrix[5] - pMatrix[0] - pMatrix[10] ) * 2.0f;

          // Calculate the x, y, x and w of the quaternion through the respective equation
          x = (pMatrix[4] + pMatrix[1] ) / scale;
          y = 0.25f * scale;
          z = (pMatrix[9] + pMatrix[6] ) / scale;
          w = (pMatrix[2] - pMatrix[8] ) / scale;
        }
        // Else the third element of the diagonal is the greatest value
        else {
          // Find the scale according to the third element, and double that value
          scale  = (float)sqrt( 1.0f + pMatrix[10] - pMatrix[0] - pMatrix[5] ) * 2.0f;

          // Calculate the x, y, x and w of the quaternion through the respective equation
          x = (pMatrix[2] + pMatrix[8] ) / scale;
          y = (pMatrix[9] + pMatrix[6] ) / scale;
          z = 0.25f * scale;
          w = (pMatrix[4] - pMatrix[1] ) / scale;
        }
      }
    }

    void cQuaternion::SetFromMatrix(const cMat4& rhs)
    {
      SetFromQuaternion(rhs.GetRotation());
    }
  }
}
