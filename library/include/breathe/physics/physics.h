#ifndef CPHYSICS_H
#define CPHYSICS_H

#if !defined(BUILD_PHYSICS_2D) && !defined(BUILD_PHYSICS_3D)
#error "Please define either BUILD_PHYSICS_2D or BUILD_PHYSICS_3D in your IDE"
#endif

namespace breathe
{
  namespace physics
  {
#ifdef BUILD_PHYSICS_2D
    typedef math::cVec2 physvec_t;
    const math::cVec2 physveczero(0.0f, 0.0f);
    typedef float_t physrotation_t;
#elif defined(BUILD_PHYSICS_3D)
    typedef math::cVec3 physvec_t;
    const math::cVec3 physveczero(0.0f, 0.0f, 0.0f);
    typedef math::cQuaternion physrotation_t;
#endif

    unsigned int GetFrequencyHz();
    float GetTimeStep();
    unsigned int GetIterations();

    extern const int iMaxContacts;
    extern const float fFriction;
    extern const float fBounce;
    extern const float fBounceVel;
    extern const float fERP;
    extern const float fCFM;
    extern const float fGravity;

    class cPhysicsObject;
    typedef cSmartPtr<cPhysicsObject> cPhysicsObjectRef;


    void Destroy();
    void Update(sampletime_t currentTime);

    void AddPhysicsObject(cPhysicsObjectRef pPhysicsObject);
    void RemovePhysicsObject(cPhysicsObjectRef pPhysicsObject);

    size_t size();
    bool empty();

    typedef std::list<cPhysicsObjectRef>::iterator iterator;
    iterator begin();
    iterator end();
  }
}

#ifdef BUILD_PHYSICS_2D
#include <breathe/physics/physics2d/physics.h>
#include <breathe/physics/physics2d/cContact.h>
#elif defined(BUILD_PHYSICS_3D)
#include <breathe/physics/physics3d/physics.h>
#include <breathe/physics/physics3d/cContact.h>
#include <breathe/physics/physics3d/cRayCast.h>
#endif

#include <breathe/physics/cPhysicsObject.h>

#endif // CPHYSICS_H
