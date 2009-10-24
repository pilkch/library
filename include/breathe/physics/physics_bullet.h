#ifndef PHYSICS_BULLET_H
#define PHYSICS_BULLET_H

#include <breathe/physics/physics.h>

// http://www.bulletphysics.com/

namespace breathe
{
  namespace bullet
  {
    // Forward declaration
    class cBody;
    typedef cSmartPtr<cBody> cBodyRef;

    class cHeightmap;
    typedef cSmartPtr<cHeightmap> cHeightmapRef;

    class cCar;
    typedef cSmartPtr<cCar> cCarRef;

    class cWorld : public physics::cWorld
    {
    public:
      cWorld();

      btDiscreteDynamicsWorld* GetWorld() { return dynamicsWorld; }

    private:
      virtual bool _Init(float fWorldWidth, float fWorldDepth, float fWorldHeight);
      virtual void _Destroy();

      void CreateGround();
      void DestroyGround();

      //static void _CollisionCallbackStatic(void* pUserData, dGeomID o1, dGeomID o2);
      //void CollisionCallbackStatic(dGeomID o1, dGeomID o2);

      //static void _CollisionCallbackDynamic(void* pUserData, dGeomID o1, dGeomID o2);
      //void CollisionCallbackDynamic(dGeomID o1, dGeomID o2);

      virtual physics::cBodyRef _CreateBody(const physics::cBoxProperties& properties);
      virtual physics::cBodyRef _CreateBody(const physics::cSphereProperties& properties);
      virtual physics::cHeightmapRef _CreateHeightmap(const physics::cHeightmapProperties& properties);
      virtual physics::cCarRef _CreateCar(const physics::cCarProperties& properties);

      virtual void _DestroyBody(physics::cBodyRef pBody);
      virtual void _DestroyCar(physics::cCarRef pCar);

      virtual void _Update(sampletime_t currentTime);

      btAxisSweep3* broadphase;
      btDefaultCollisionConfiguration* collisionConfiguration;
      btCollisionDispatcher* dispatcher;
      btSequentialImpulseConstraintSolver* solver;
      btDiscreteDynamicsWorld* dynamicsWorld;

      btCollisionShape* groundShape;
      btRigidBody* groundRigidBody;
    };

    // The physics object
    class cBody : public physics::cBody
    {
    public:
      cBody();

      void CreateBox(cWorld* pWorld, const physics::cBoxProperties& properties);
      void CreateSphere(cWorld* pWorld, const physics::cSphereProperties& properties);

    private:
      NO_COPY(cBody);

      void InitCommon();

      virtual void _SetPositionAbsolute(const spitfire::math::cVec3& position);
      virtual void _SetRotationAbsolute(const spitfire::math::cQuaternion& rotation);
      virtual void _SetMassKg(float fMassKg);


      virtual void _AddForceRelativeToWorldKg(const spitfire::math::cVec3& forceKg);
      virtual void _AddTorqueRelativeToWorldNm(const spitfire::math::cVec3& torqueNm);

      virtual void _AddForceRelativeToObjectKg(const spitfire::math::cVec3& forceKg);
      virtual void _AddTorqueRelativeToObjectNm(const spitfire::math::cVec3& torqueNm);

      virtual void _Update(sampletime_t currentTime);

      virtual void _Remove();

      cWorld* pWorld;
      btCollisionShape* bodyShape;
      btDefaultMotionState* bodyMotionState;
      btRigidBody* bodyRigidBody;
    };


    class cHeightmap : public physics::cHeightmap
    {
    public:
      explicit cHeightmap(const physics::cHeightmapProperties& properties);

      void CreateHeightmap(cWorld* pWorld);

    private:
      virtual void _Update(sampletime_t currentTime);

      virtual void _Remove();

      btHeightfieldTerrainShape* heightmapShape;
    };


    class cCar : public physics::cCar
    {
    public:
      cCar();

    private:
      btRaycastVehicle::btVehicleTuning m_tuning;
      btVehicleRaycaster* m_vehicleRayCaster;
      btRaycastVehicle* m_vehicle;
      btCollisionShape* m_wheelShape;
    };
  }
}

#endif // PHYSICS_BULLET_H
