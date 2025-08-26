#pragma once

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/cVec2.h>

namespace breathe {

class cCamera2D {
public:
  cCamera2D() : scale(1.0f, 1.0f) {}

  void SetPosition(const spitfire::math::cVec2& _position) { position = _position; }

  void MoveX(float fDistance) { position.x += fDistance; }
  void MoveY(float fDistance) { position.y += fDistance; }

  spitfire::math::cVec2 position;
  spitfire::math::cVec2 scale;
};

}
