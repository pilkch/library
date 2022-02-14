#pragma once

#include <list>
#include <vector>

#include <spitfire/math/cVec3.h>

namespace breathe {

namespace physics {

namespace verlet {

class cWorld {
public:
  cWorld();

  spitfire::math::cVec3 GetGravity() const { return gravity; }
  spitfire::math::cVec3 GetWind() const { return *majorWindEvents.begin() + *minorWindFluctuations.begin(); }

  void Update();

private:
  size_t updatesSinceLastMajorChange;
  size_t updatesSinceLastMinorChange;

  spitfire::math::cVec3 gravity;

  std::list<spitfire::math::cVec3> majorWindEvents;
  std::list<spitfire::math::cVec3> minorWindFluctuations;
};


struct Particle {
  Particle(const spitfire::math::cVec3& _pos) : lastPos(_pos), pos(_pos) {}

  spitfire::math::cVec3 lastPos;
  spitfire::math::cVec3 pos;
};

struct Spring {
  Spring(Particle* _a, Particle* _b, float _fDistance, float _fStiffness) :
    a(_a),
    b(_b),
    fDistance(_fDistance),
    fStiffness(_fStiffness)
  {
  }

  Particle* a;
  Particle* b;
  float fDistance;
  float fStiffness;
};

struct cGroup {
  std::vector<Particle> particles;
  std::vector<Particle*> pins; // A pin is a fixed point constraint
  std::vector<Spring> springs;
};

void Update(const cWorld& world, cGroup& group);

}

}

}