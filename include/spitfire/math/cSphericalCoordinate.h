#ifndef CSPHERICALCOORDINATE_H
#define CSPHERICALCOORDINATE_H

#include <spitfire/math/cVec3.h>
#include <spitfire/math/cQuaternion.h>

namespace spitfire
{
  namespace math
  {
    // http://en.wikipedia.org/wiki/Spherical_coordinates
    // "The radial distance of a point from a fixed origin"
    // "The zenith angle from the positive z-axis to the point"
    // "The azimuth angle from the positive x-axis to the orthogonal projection of the point in the x-y plane"

    // fRadialDistance (How far out from the origin we are going, this is unitless and will depend on your application)
    // fZenithDegrees (Also known as decl) (rotation down from z+ (Pointing straight up in the air))
    // fAzimuthDegrees (Also known as rasc) (rotation about the z axis)

    class cSphericalCoordinate
    {
    public:
      cSphericalCoordinate();
      cSphericalCoordinate(float_t fRadialDistance, float_t fZenithDegrees, float_t fAzimuthDegrees);

      void AssignFromEulerPosition(const cVec3& positionEuler);
      cVec3 GetEulerPosition() const;

      cQuaternion GetRotation() const; // This returns an OpenGL friendly rotation with the sphericalcoordinate vertical rotation inverted (For OpenGL compatibility)

      float_t GetDistance() const { return values.x; } // fRadialDistance
      void SetDistance(float_t _fRadialDistance) { values.x = _fRadialDistance; } // fRadialDistance

      float_t GetZenithDegrees() const { return values.y; }
      void SetZenithDegrees(float_t fZenithDegrees) { values.y = fZenithDegrees; }

      float_t GetRotationZDegrees() const { return values.z; } // fAzimuthDegrees
      void SetRotationZDegrees(float_t fRotationZDegrees) { values.z = fRotationZDegrees; } // fAzimuthDegrees

      // Non-standard, these functions convert to more sensible(?) values
      float_t GetPitchDegrees() const { return values.y + 90.0f; } // fZenithDegrees
      void SetPitchDegrees(float_t fPitchDegrees) { values.y = fPitchDegrees - 90.0f; } // fZenithDegrees

    private:
      cVec3 values; // (Radial Distance, Zenith in Degrees, Azimuth in Degrees)
    };

    inline cSphericalCoordinate::cSphericalCoordinate() :
      values(1.0f, 0.0f, 0.0f)
    {
    }

    inline cSphericalCoordinate::cSphericalCoordinate(float_t fRadialDistance, float_t fZenithDegrees, float_t fAzimuthDegrees) :
      values(fRadialDistance, fZenithDegrees, fAzimuthDegrees)
    {
    }

    inline void cSphericalCoordinate::AssignFromEulerPosition(const cVec3& positionEuler)
    {
      const float_t x = positionEuler.x;
      const float_t y = positionEuler.y;
      const float_t z = positionEuler.z;

      SetDistance(sqrtf(x * x + y * y + z * z));
      SetPitchDegrees(Atan2Degrees(y, x));
      SetRotationZDegrees(Atan2Degrees(z, sqrtf(x * x + y * y)));
    }

    inline cVec3 cSphericalCoordinate::GetEulerPosition() const
    {
      const float_t fRadialDistance = GetDistance();
      const float_t fZenithDegrees = GetPitchDegrees();
      const float_t fAzimuthDegrees = GetRotationZDegrees();

      cVec3 positionEuler;

      positionEuler.x = fRadialDistance * cos(DegreesToRadians(fAzimuthDegrees)) * cos(DegreesToRadians(fZenithDegrees));
      positionEuler.y = fRadialDistance * sin(DegreesToRadians(fAzimuthDegrees)) * cos(DegreesToRadians(fZenithDegrees));
      positionEuler.z = fRadialDistance * sin(DegreesToRadians(fZenithDegrees));

      return positionEuler;
    }

    // This returns an OpenGL friendly rotation with the sphericalcoordinate vertical rotation inverted (For OpenGL compatibility)
    inline cQuaternion cSphericalCoordinate::GetRotation() const
    {
      cVec3 r(0.0f, GetPitchDegrees(), GetRotationZDegrees());

      cQuaternion q;
      q.SetFromAngles(r);

      return q;
    }
  }
}

#endif // CSPHERICALCOORDINATE_H
