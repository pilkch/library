#ifndef CCAMERA_H
#define CCAMERA_H

#include <spitfire/math/cFrustum.h>

namespace breathe
{
  namespace render
  {
    class cCamera
    {
    public:
      cCamera();

      float_t GetFieldOfViewDegrees() const { return fFOVDegrees; }
      const spitfire::math::cVec3& GetEyePosition() const { return positionEye; }
      const spitfire::math::cVec3& GetLookAtPoint() const { return positionLookAtPoint; }
      const spitfire::math::cVec3& GetUpDirection() const { return directionUp; }

      void SetFieldOfViewDegrees(float_t _fFOVDegrees) { fFOVDegrees = _fFOVDegrees; }

      void SetLookAtCamera(const spitfire::math::cVec3& positionEye, const spitfire::math::cVec3& positionLookAtPoint);
      void SetLookAtCameraZoomToFillScreen(const spitfire::math::cVec3& positionEye, const spitfire::math::cVec3& positionOfObject, float_t fDiameterOfObjectMeters);
      void SetFirstPersonCamera(const spitfire::math::cVec3& positionEye, const spitfire::math::cQuaternion& rotation);
      void SetThirdPersonCamera(const spitfire::math::cVec3& positionOfObject, const spitfire::math::cQuaternion& rotationOfObject);
      void SetThirdPersonCamera(const spitfire::math::cVec3& positionOfObject, const spitfire::math::cQuaternion& rotationOfObject, const spitfire::math::cQuaternion& rotationOfViewRelativeToRotationOfObject);

      spitfire::math::cFrustum CreateFrustumFromCamera() const;

    private:
      float_t fFOVDegrees;
      spitfire::math::cVec3 positionEye;
      spitfire::math::cVec3 positionLookAtPoint;
      spitfire::math::cVec3 directionUp;
    };
  }
}

#endif // CCAMERA_H
