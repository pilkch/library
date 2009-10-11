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

    class cPhysicsObject;
    typedef cSmartPtr<cPhysicsObject> cPhysicsObjectRef;

    typedef std::list<cPhysicsObjectRef>::iterator iterator;

    class cWorld
    {
    public:
      size_t GetFrequencyHz() const;
      float GetIntervalMS() const;
      size_t GetIterations() const;

      void Destroy();
      void Update(sampletime_t currentTime);

      void AddPhysicsObject(cPhysicsObjectRef pPhysicsObject);
      void RemovePhysicsObject(cPhysicsObjectRef pPhysicsObject);

      size_t size() const;
      bool empty() const;

      iterator begin();
      iterator end();

#ifdef BUILD_PHYSICS_3D
      dSpaceID GetSpaceStatic();
      dSpaceID GetSpaceDynamic();

      dWorldID GetWorld();

      dJointGroupID GetContactGroup();

      void CreateGround(float posX, float posY, float posZ, float nX, float nY, float nZ);
      void Init(float width, float height, float depth);
#endif

    private:
      std::list<cPhysicsObjectRef> lPhysicsObject;
    };
  }
}

#ifdef BUILD_PHYSICS_2D
#include <breathe/physics/physics2d/physics.h>
#include <breathe/physics/physics2d/cContact.h>
#elif defined(BUILD_PHYSICS_3D)
#include <breathe/physics/physics3d/cContact.h>
#include <breathe/physics/physics3d/cRayCast.h>
#endif

#include <breathe/physics/cPhysicsObject.h>

#endif // CPHYSICS_H
