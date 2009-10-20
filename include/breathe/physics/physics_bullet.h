#ifndef PHYSICS_BULLET_H
#define PHYSICS_BULLET_H

#include <breathe/physics/physics.h>

// http://www.bulletphysics.com/

namespace breathe
{
  namespace physics_bullet
  {
    // Forward declaration
    class cBody;
    typedef cSmartPtr<cBody> cBodyRef;

    class cWorld : public audio::cWorld
    {
    public:
      cWorld();

    private:
      virtual bool _Init();
      virtual void _Destroy();

      virtual cBodyRef _CreateBody();
      virtual void _DestroyBody(cBodyRef pBody);

      virtual void _Update(sampletime_t currentTime);
    };


    // The physics object
    class cBody
    {
    public:
      cBody();

    private:
      NO_COPY(cBody);

      virtual void _SetPosition(const spitfire::math::cVec3& position);
      virtual void _SetRotation(const spitfire::math::cQuaternion& rotation);

      virtual void _Update(sampletime_t currentTime);

      virtual void _Remove();
    };

    class cCar
    {
    public:


    private:
      btRaycastVehicle::btVehicleTuning m_tuning;
      btVehicleRaycaster* m_vehicleRayCaster;
      btRaycastVehicle* m_vehicle;
      btCollisionShape* m_wheelShape;
    };
  }
}

#endif // PHYSICS_BULLET_H
