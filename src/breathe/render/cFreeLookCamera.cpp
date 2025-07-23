// Spitfire headers
#include <spitfire/math/math.h>

// Breathe headers
#include <breathe/render/cFreeLookCamera.h>

namespace breathe {

// ** cFreeLookCamera

namespace {

const float DEFAULT_YAW = 90.0f;
const float DEFAULT_PITCH = 0.0f;
const float DEFAULT_ZOOM = 45.0f;

}

cFreeLookCamera::cFreeLookCamera() :
  fRotationRight(DEFAULT_YAW),
  fRotationUp(DEFAULT_PITCH),
  fZoom(DEFAULT_ZOOM)
{
}

void cFreeLookCamera::SetPosition(const spitfire::math::cVec3& _position)
{
  position = _position;
}

void cFreeLookCamera::SetRotation(float _fRotationRight, float _fRotationUp)
{
  fRotationRight = _fRotationRight;
  fRotationUp = _fRotationUp;
}

void cFreeLookCamera::SetRotation(const spitfire::math::cQuaternion& rotation)
{
  const spitfire::math::cVec3 euler = rotation.GetEuler();
  fRotationRight = euler.x;
  fRotationUp = euler.z;
}

void cFreeLookCamera::LookAt(const spitfire::math::cVec3& eye, const spitfire::math::cVec3& target, const spitfire::math::cVec3 up)
{
  const spitfire::math::cMat4 matView = spitfire::math::cMat4::LookAt(eye, target, up);

  SetPosition(eye);
  SetRotation(matView.GetRotation());
}

void cFreeLookCamera::MoveX(float xmmod)
{
  const spitfire::math::cQuaternion rotation = GetRotation();
  position += (-rotation) * spitfire::math::cVec3(xmmod, 0.0f, 0.0f);
}

void cFreeLookCamera::MoveZ(float ymmod)
{
  const spitfire::math::cQuaternion rotation = GetRotation();
  position += (-rotation) * spitfire::math::cVec3(0.0f, 0.0f, -ymmod);
}

void cFreeLookCamera::RotateX(float xrmod)
{
  fRotationRight += xrmod;
}

void cFreeLookCamera::RotateY(float yrmod)
{
  fRotationUp += yrmod;
}

spitfire::math::cQuaternion cFreeLookCamera::GetRotation() const
{
  spitfire::math::cQuaternion up;
  up.SetFromAxisAngle(spitfire::math::cVec3(1.0f, 0.0f, 0.0f), spitfire::math::DegreesToRadians(fRotationUp));
  spitfire::math::cQuaternion right;
  right.SetFromAxisAngle(spitfire::math::cVec3(0.0f, 1.0f, 0.0f), spitfire::math::DegreesToRadians(fRotationRight));

  return (up * right);
}

spitfire::math::cMat4 cFreeLookCamera::CalculateProjectionMatrix(size_t width, size_t height) const
{
  // TODO: Calculate the FOV from a linear camera style zoom
  const float fFOVDegrees = fZoom;
  return spitfire::math::cMat4::Perspective(spitfire::math::DegreesToRadians(fFOVDegrees), float(width) / float(height), 0.1f, 1000.0f);
}

spitfire::math::cMat4 cFreeLookCamera::CalculateViewMatrix() const
{
  spitfire::math::cMat4 matTranslation;
  matTranslation.TranslateMatrix(-position);

  const spitfire::math::cQuaternion rotation = -GetRotation();
  const spitfire::math::cMat4 matRotation = rotation.GetMatrix();

  spitfire::math::cMat4 matTargetTranslation;
  matTargetTranslation.TranslateMatrix(spitfire::math::cVec3(0.0f, 0.0f, 1.0f));

  return ((matTargetTranslation * matRotation) * matTranslation);
}

}
