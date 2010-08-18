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
#include <btBulletDynamicsCommon.h>
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#endif

#ifdef BUILD_PHYSICS_ODE
#include <ode/ode.h>
#endif

#ifdef BUILD_PHYSICS_BOX2D
#include <Box2D/Box2D.h>
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
#ifdef BUILD_PHYSICS_BOX2D
#include <breathe/physics/physics_box2d.h>
#endif

namespace breathe
{
  namespace physics
  {
    // Global variable unfortunately
    cWorld* pWorld = nullptr;

    cWorld* Create(DRIVER driver, const physvec_t& worldDimensions)
    {
      LOG<<"physics::Create"<<std::endl;
      ASSERT(pWorld == nullptr);

      switch (driver) {
#ifdef BUILD_PHYSICS_BULLET
        case DRIVER::BULLET: {
          pWorld = new bullet::cWorld;
          break;
        }
#endif
#ifdef BUILD_PHYSICS_ODE
        case DRIVER::ODE: {
          pWorld = new ode::cWorld;
          break;
        }
#endif
#ifdef BUILD_PHYSICS_BOX2D
        case DRIVER::BOX2D: {
          pWorld = new box2d::cWorld;
          break;
        }
#endif
        default: {
          LOG<<"physics::Create UNKNOWN driver"<<std::endl;
        }
      }

      LOG<<"physics::Create returning"<<std::endl;
      ASSERT(pWorld != nullptr);

      pWorld->Init(worldDimensions);

      return pWorld;
    }

    void Destroy(cWorld*)
    {
      LOG<<"physics::Destroy"<<std::endl;
      ASSERT(pWorld != nullptr);

      pWorld->Destroy();

      SAFE_DELETE(pWorld);
    }

    cWorld* GetWorld()
    {
      ASSERT(pWorld != nullptr);
      return pWorld;
    }



    void cWorld::Update(sampletime_t currentTime)
    {
      std::list<cBodyRef>::iterator iter = lPhysicsBody.begin();
      const std::list<cBodyRef>::iterator iterEnd = lPhysicsBody.end();
      while (iter != iterEnd) {
        (*iter)->Update(currentTime);

        iter++;
      }

      _Update(currentTime);
    }




    cBoxProperties::cBoxProperties() :
      bIsDynamic(true),
      fWidthMetres(1.0f),
    #ifdef BUILD_PHYSICS_3D
      fDepthMetres(1.0f),
    #endif
      fHeightMetres(1.0f),
      fMassKg(1.0f),
      fFriction(0.3f),
      fRestitution(0.1f)
    {
    };

    cSphereProperties::cSphereProperties() :
      bIsDynamic(true),
      fRadiusMetres(0.5f),
      fMassKg(1.0f),
      fFriction(0.3f),
      fRestitution(0.1f)
    {
    }



#ifdef BUILD_PHYSICS_3D
    cHeightmapProperties::cHeightmapProperties(const game::cTerrainHeightMap& _loader) :
      loader(_loader),
      width(1),
      height(1),
      scale(1.0f, 1.0f, 1.0f),
      fFriction(0.3f),
      fRestitution(0.1f)
    {
    }

    cHeightmap::cHeightmap(const cHeightmapProperties& properties) :
      loader(properties.loader),
      width(properties.width),
      height(properties.height),
      position(properties.position),
      scale(properties.scale),
      fFriction(properties.fFriction),
      fRestitution(properties.fRestitution)
    {
    }
#else
    cHeightmapProperties::cHeightmapProperties(const std::vector<float>& _values) :
      values(_values),
      width(1),
      scale(1.0f, 1.0f),
      fFriction(0.3f),
      fRestitution(0.1f)
    {
    }

    cHeightmap::cHeightmap(const cHeightmapProperties& properties) :
      values(properties.values),
      width(properties.width),
      position(properties.position),
      scale(properties.scale),
      fFriction(properties.fFriction),
      fRestitution(properties.fRestitution)
    {
    }
#endif


    cSpringProperties::cSpringProperties() :
      fK(1.0f),
      fFriction(1.0f)
    {
    }


    cCarProperties::cCarProperties() :
      fMassKg(1600.0f),
      fWidthMetres(2.0f),
      fHeightMetres(1.8f),

      fSuspensionStiffness(5.88f),
      fSuspensionCompression(0.83f),
      fSuspensionDamping(0.88f),
      fSuspensionRestLengthCentimetres(30.0f),
      fSuspensionMaxTravelCentimetres(50.0f),

      // Wheels
      nWheels(4),
      fWheelMassKg(20.0f),
      fWheelWidthCentimetres(20.0f),
      fWheelRadiusCentimetres(45.0f),
      fTireFrictionSlip(10.5f)
    {
    }

    cCar::cCar()
    {
      for (size_t i = 0; i < 4; i++) {
        fWheelAccelerationForceNewtons[i] = 0.0f;
        fWheelBrakingForceNewtons[i] = 0.0f;
        fWheelSteeringAngleMinusOneToPlusOne[i] = 0.0f;
      }
    }
  }
}

