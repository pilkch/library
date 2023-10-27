#pragma once

#include <list>
#include <vector>

#include <spitfire/math/geometry.h>
#include <spitfire/math/cVec3.h>

namespace breathe {

namespace physics {

namespace verlet {

struct cWindProperties {
  cWindProperties();

  spitfire::math::cVec3 generalWindForce;

  float fMaxHorizontalForce;
  float fMaxVerticalForce;
};

class cWorld {
public:
  cWorld();

  void Init(float fGravity, const cWindProperties& windProperties);

  float GetGravity() const { return fGravity; }
  spitfire::math::cVec3 GetWind() const { return *majorWindEvents.begin() + *minorWindFluctuations.begin(); }

  void Update();

private:
  size_t updatesSinceLastMajorChange;
  size_t updatesSinceLastMinorChange;

  float fGravity;

  // The major and minor wind events coming up
  // These are just a list of speeds that are coming up in the next time steps
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
void Collide(cGroup& group, const spitfire::math::cCapsule& capsule);
void CollideGroundPlane(cGroup& group, float fGroundHeight);

}

}

}
