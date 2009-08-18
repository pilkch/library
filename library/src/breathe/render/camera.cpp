// Standard headers
#include <cmath>
#include <cassert>

#include <vector>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cQuaternion.h>

// Breathe headers
#include <breathe/render/camera.h>

namespace breathe
{
  namespace render
  {
    cCamera::cCamera() :
      fFOVDegrees(90.0f),
      directionUp(spitfire::math::v3Up)
    {
    }

    //void cCamera::SetLookAtCamera(const math::cVec3& positionEye, const math::cVec3& positionLookAtPoint);

    void cCamera::SetFirstPersonCamera(const spitfire::math::cVec3& _positionEye, const spitfire::math::cQuaternion& rotation)
    {
      positionEye = _positionEye;

      spitfire::math::cMat4 matRotation = rotation.GetMatrix();
      positionLookAtPoint = _positionEye - (matRotation * spitfire::math::cVec3(0.0f, 1.0f, 200.0f));

      directionUp = spitfire::math::v3Up;
    }

    void cCamera::SetThirdPersonCamera(const spitfire::math::cVec3& positionOfObject, const spitfire::math::cQuaternion& rotationOfObject)
    {
      positionLookAtPoint = positionOfObject;


      const spitfire::math::cMat4 matRotationOfObject = rotationOfObject.GetMatrix();

      spitfire::math::cVec3 positionRelativeToObject(0.0f, 100.0f, 10.0f);

      spitfire::math::cMat4 matObject;
      matObject.SetRotationPart(rotationOfObject);
      matObject.SetTranslationPart(positionOfObject);

      spitfire::math::cMat4 matRotationOfEye;
      matRotationOfEye.SetTranslationPart(positionRelativeToObject);

      positionEye = (matObject * matRotationOfEye).GetPosition();


      //directionUp = spitfire::math::v3Up;
      directionUp = matRotationOfObject.GetUp();
    }

    void cCamera::SetThirdPersonCamera(const spitfire::math::cVec3& positionOfObject, const spitfire::math::cQuaternion& rotationOfObject, const spitfire::math::cQuaternion& rotationOfViewRelativeToRotationOfObject)
    {
      positionLookAtPoint = positionOfObject;

      spitfire::math::cMat4 matRotation = (rotationOfObject * rotationOfViewRelativeToRotationOfObject).GetMatrix();
      positionEye = positionOfObject - (matRotation * spitfire::math::cVec3(0.0f, 1.0f, 25.0f));

      directionUp = spitfire::math::v3Up;
    }

    spitfire::math::cFrustum cCamera::CreateFrustumFromCamera() const
    {
      spitfire::math::cFrustum frustum;

      frustum.targetIdeal = positionLookAtPoint;
      frustum.eyeIdeal = positionEye;

//#ifdef BUILD_FIRST_PERSON
      frustum.Update();
//#endif

      return frustum;
    }
  }
}
