// Standard libraries
#include <cmath>
#include <cassert>

#include <vector>
#include <map>
#include <list>
#include <set>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

// Boost includes
#include <boost/smart_ptr.hpp>

// Other libraries
#ifdef BUILD_PHYSICS_BULLET
#include <bullet/bullet.h>
#endif

#ifdef BUILD_PHYSICS_ODE
#include <ode/ode.h>
#endif

// Spitfire
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/geometry.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>

#include <breathe/physics/physics.h>
#ifdef BUILD_PHYSICS_BULLET
#include <breathe/physics/physics_bullet.h>
#endif
#ifdef BUILD_PHYSICS_ODE
#include <breathe/physics/physics_ode.h>
#endif


namespace breathe
{
  namespace physics
  {
    // Global variable unfortunately
    cManager* pManager = nullptr;

    bool Init(DRIVER driver)
    {
      LOG<<"physics::Init"<<std::endl;
      ASSERT(pManager == nullptr);

      switch (driver) {
#ifdef BUILD_PHYSICS_BULLET
        case DRIVER::DRIVER_BULLET: {
          pManager = new bullet::cManager;
          break;
        }
#endif
#ifdef BUILD_PHYSICS_ODE
        case DRIVER::DRIVER_ODE: {
          pManager = new ode::cManager;
          break;
        }
#endif
        default: {
          LOG<<"physics::Init UNKNOWN driver"<<std::endl;
        }
      }

      LOG<<"physics::Init returning"<<std::endl;
      ASSERT(pManager != nullptr);

      pManager->Init();

      return (pManager != nullptr);
    }

    void Destroy()
    {
      LOG<<"physics::Destroy"<<std::endl;
      ASSERT(pManager != nullptr);

      pManager->Destroy();

      SAFE_DELETE(pManager);
    }

    cManager* GetManager()
    {
      ASSERT(pManager != nullptr);
      return pManager;
    }



    void cManager::Update(sampletime_t currentTime, const math::cVec3& listenerPosition, const math::cVec3& listenerTarget, const math::cVec3& listenerUp)
    {
      const cListener listener(listenerPosition, listenerTarget, listenerUp);


      std::list<cSourceRef>::iterator iter = lPhysicsBody.begin();
      const std::list<cSourceRef>::iterator iterEnd = lPhysicsBody.end();
      std::cout<<"uSources="<<lPhysicsBody.size()<<std::endl;
      while (iter != iterEnd) {
        (*iter)->Update(currentTime, listener);

        iter++;
      }


      // Sort in order of distance from the listener
      lPhysicsBody.sort(cSource::DistanceFromListenerCompare);


      _Update(currentTime, listener);
    }













    class btVehicleTuning;
    struct btVehicleRaycaster;
    class btCollisionShape;

    #include "BulletDynamics/Vehicle/btRaycastVehicle.h"
    #include "BulletDynamics/ConstraintSolver/btHingeConstraint.h"
    #include "BulletDynamics/ConstraintSolver/btSliderConstraint.h"

    ///VehicleDemo shows how to setup and use the built-in raycast vehicle
    class ForkLiftDemo : public GlutDemoApplication
    {
    public:

      btRigidBody* m_carChassis;

      //----------------------------
      btRigidBody* m_liftBody;
      btVector3       m_liftStartPos;
      btHingeConstraint* m_liftHinge;

      btRigidBody* m_forkBody;
      btVector3       m_forkStartPos;
      btSliderConstraint* m_forkSlider;

      btRigidBody* m_loadBody;
      btVector3       m_loadStartPos;

      void lockLiftHinge(void);
      void lockForkSlider(void);

      bool m_useDefaultCamera;
      //----------------------------


      btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

      class btBroadphaseInterface*    m_overlappingPairCache;

      class btCollisionDispatcher*    m_dispatcher;

      class btConstraintSolver*       m_constraintSolver;

      class btDefaultCollisionConfiguration* m_collisionConfiguration;

      class btTriangleIndexVertexArray*       m_indexVertexArrays;

      btVector3*      m_vertices;


      btRaycastVehicle::btVehicleTuning       m_tuning;
      btVehicleRaycaster*     m_vehicleRayCaster;
      btRaycastVehicle*       m_vehicle;
      btCollisionShape*       m_wheelShape;

      float           m_cameraHeight;

      float   m_minCameraDistance;
      float   m_maxCameraDistance;


      ForkLiftDemo();

      virtual ~ForkLiftDemo();

      virtual void clientMoveAndDisplay();

      virtual void    clientResetScene();

      virtual void displayCallback();

      ///a very basic camera following the vehicle
      virtual void updateCamera();

      virtual void specialKeyboard(int key, int x, int y);

      virtual void specialKeyboardUp(int key, int x, int y);

      void renderme();

      void initPhysics();
      void termPhysics();
    };
  }
}

