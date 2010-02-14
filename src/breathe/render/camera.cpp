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

    void cCamera::SetLookAtCamera(const spitfire::math::cVec3& _positionEye, const spitfire::math::cVec3& _positionLookAtPoint)
    {
      positionEye = _positionEye;

      positionLookAtPoint = _positionLookAtPoint;

      // This is wrong if the camera can "roll"
      directionUp = spitfire::math::v3Up;
      directionRight = spitfire::math::v3Right;
    }

    void cCamera::SetFirstPersonCamera(const spitfire::math::cVec3& _positionEye, const spitfire::math::cQuaternion& rotation)
    {
      positionEye = _positionEye;

      spitfire::math::cMat4 matRotation = rotation.GetMatrix();
      positionLookAtPoint = _positionEye - (matRotation * spitfire::math::cVec3(0.0f, 1.0f, 200.0f));

      // This is wrong if the camera can "roll"
      directionUp = spitfire::math::v3Up;
      directionRight = spitfire::math::v3Right;
    }

    void cCamera::SetThirdPersonCamera(const spitfire::math::cVec3& positionOfObject, const spitfire::math::cQuaternion& rotationOfObject)
    {
      positionLookAtPoint = positionOfObject;



      spitfire::math::cVec3 positionRelativeToObject(0.0f, -100.0f, 0.0f);


      //spitfire::math::cMat4 matObject;
      //matObject.SetRotationPart(rotationOfObject);
      //matObject.SetTranslationPart(positionOfObject);

      //spitfire::math::cMat4 matRotationOfEye;
      //matRotationOfEye.SetTranslationPart(positionRelativeToObject);

      //positionEye = (matObject * matRotationOfEye).GetPosition();


      // This is wrong if the camera can "roll"
      //const spitfire::math::cMat4 matRotationOfObject = rotationOfObject.GetMatrix();
      //directionUp = matRotationOfObject.GetUp();



      positionEye = positionLookAtPoint + rotationOfObject.GetRotatedVector(positionRelativeToObject);

      directionUp = rotationOfObject.GetRotatedVector(spitfire::math::v3Up).GetNormalised();
      directionRight = rotationOfObject.GetRotatedVector(spitfire::math::v3Right).GetNormalised();





      /*
      // Third person over the shoulder camera for a Far Cry Mod
      // http://farcry.crymod.com/thread.php?threadid=9783

      float camDist = 0.75f;
      float AngleCoeff = m_vEyeAngles.z;
      float AngleVCoeff = m_vEyeAngles.x/90;
      float camHeight = 1.0f;
      float Circling = 1.75f;

      if (AngleCoeff >= 0)
      {
        AngleCoeff += 30;

        // X
        if (m_vEyeAngles.x >= 0)
        {
          camHeight = camDist*(1-pow((1-AngleVCoeff), Circling));
          offset.z += camHeight;
          camDist = max(camDist*(1-AngleVCoeff), camDist/2);
          AngleCoeff += 90*pow(AngleVCoeff, Circling);
        }
        else
        {
          camHeight = camDist*(1-pow((1- -AngleVCoeff), Circling));
          offset.z -= camHeight;
        }

        if (AngleCoeff >= 360)
        {
          int steps = AngleCoeff/360;
          AngleCoeff -= 360*steps;
        }

        while(true)
        {
          if (AngleCoeff < 90)
          {
            AngleCoeff /= 90;
            offset.x -= camDist*(1-pow((1-AngleCoeff), Circling));
            offset.y += camDist*(1-pow(AngleCoeff, Circling));
            break;
          }
          if (AngleCoeff < 180)
          {
            AngleCoeff -= 90;
            AngleCoeff /= 90;
            offset.x -= camDist*(1-pow(AngleCoeff, Circling));
            offset.y -= camDist*(1-pow((1-AngleCoeff), Circling));
            break;
          }
          if (AngleCoeff < 270)
          {
            AngleCoeff -= 180;
            AngleCoeff /= 90;
            offset.x += camDist*(1-pow((1-AngleCoeff), Circling));
            offset.y -= camDist*(1-pow(AngleCoeff, Circling));
            break;
          }
          AngleCoeff -= 270;
          AngleCoeff /= 90;
          offset.x += camDist*(1-pow(AngleCoeff, Circling));
          offset.y += camDist*(1-pow((1-AngleCoeff), Circling));
          break;
        }
      }
      else
      {
        AngleCoeff += 30;
        AngleCoeff = -AngleCoeff+360;

        // X
        if (m_vEyeAngles.x >= 0)
        {
          camHeight = camDist*(1-pow((1-AngleVCoeff), Circling));
          offset.z += camHeight;
          camDist = max(camDist*(1-AngleVCoeff), camDist/2);
          AngleCoeff -= 90*pow(AngleVCoeff, Circling);
        }
        else
        {
          camHeight = camDist*(1-pow((1- -AngleVCoeff), Circling));
          offset.z -= camHeight;
        }

        if (AngleCoeff >= 360)
        {
          int steps = AngleCoeff/360;
          AngleCoeff -= 360*steps;
        }

        while(true)
        {
          if (AngleCoeff < 90)
          {
            AngleCoeff /= 90;
            offset.x += camDist*(1-pow((1-AngleCoeff), Circling));
            offset.y += camDist*(1-pow(AngleCoeff, Circling));
            break;
          }
          if (AngleCoeff < 180)
          {
            AngleCoeff -= 90;
            AngleCoeff /= 90;
            offset.x += camDist*(1-pow(AngleCoeff, Circling));
            offset.y -= camDist*(1-pow((1-AngleCoeff), Circling));
            break;
          }
          if (AngleCoeff < 270)
          {
            AngleCoeff -= 180;
            AngleCoeff /= 90;
            offset.x -= camDist*(1-pow((1-AngleCoeff), Circling));
            offset.y -= camDist*(1-pow(AngleCoeff, Circling));
            break;
          }
          AngleCoeff -= 270;
          AngleCoeff /= 90;
          offset.x -= camDist*(1-pow(AngleCoeff, Circling));
          offset.y += camDist*(1-pow((1-AngleCoeff), Circling));
          break;
        }
      }

      camera->SetPos(offset);
      camera->SetAngles(m_vEyeAngles);
*/
    }

    void cCamera::SetThirdPersonCamera(const spitfire::math::cVec3& positionOfObject, const spitfire::math::cQuaternion& rotationOfObject, const spitfire::math::cQuaternion& rotationOfViewRelativeToRotationOfObject)
    {
      positionLookAtPoint = positionOfObject;

      spitfire::math::cMat4 matRotation = (rotationOfObject * rotationOfViewRelativeToRotationOfObject).GetMatrix();
      positionEye = positionOfObject - (matRotation * spitfire::math::cVec3(0.0f, 1.0f, 25.0f));

      // This is wrong if the camera can "roll"
      directionUp = spitfire::math::v3Up;
      directionRight = spitfire::math::v3Right;
    }

    spitfire::math::cFrustum cCamera::CreateFrustumFromCamera() const
    {
      spitfire::math::cFrustum frustum;

      frustum.SetLookAtPoint(positionLookAtPoint);
      frustum.SetEyePosition(positionEye);
      frustum.SetEyeUp(directionUp);
      frustum.SetEyeRight(directionRight);

      frustum.Update();

      return frustum;
    }
  }
}
