#pragma once

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/cVec3.h>
#include <spitfire/math/cQuaternion.h>

namespace breathe {

class cFreeLookCamera
{
public:
  cFreeLookCamera();

  constexpr const spitfire::math::cVec3& GetPosition() const { return position; }
  void SetPosition(const spitfire::math::cVec3& position);
  void SetRotation(float fRotationRight, float fRotationUp);
  void SetRotation(const spitfire::math::cQuaternion& rotation);

  void LookAt(const spitfire::math::cVec3& eye, const spitfire::math::cVec3& target, const spitfire::math::cVec3 up); // Places the camera at the eye position and looks at the target

  void MoveX(float fDistance);
  void MoveZ(float fDistance);
  void RotateX(float fDegrees);
  void RotateY(float fDegrees);

  spitfire::math::cMat4 CalculateProjectionMatrix(size_t width, size_t height) const;
  spitfire::math::cMat4 CalculateViewMatrix() const;

private:
  spitfire::math::cQuaternion GetRotation() const;

  spitfire::math::cVec3 position;
  float fRotationRight;
  float fRotationUp;

  float fZoom;
};

}
