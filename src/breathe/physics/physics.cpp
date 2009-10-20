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
    cWorld* pWorld = nullptr;

    bool Init(DRIVER driver, float fWorldWidth, float fWorldDepth, float fWorldHeight)
    {
      LOG<<"physics::Init"<<std::endl;
      ASSERT(pWorld == nullptr);

      switch (driver) {
#ifdef BUILD_PHYSICS_BULLET
        case DRIVER::DRIVER_BULLET: {
          pWorld = new bullet::cWorld;
          break;
        }
#endif
#ifdef BUILD_PHYSICS_ODE
        case DRIVER::DRIVER_ODE: {
          pWorld = new ode::cWorld;
          break;
        }
#endif
        default: {
          LOG<<"physics::Init UNKNOWN driver"<<std::endl;
        }
      }

      LOG<<"physics::Init returning"<<std::endl;
      ASSERT(pWorld != nullptr);

      pWorld->Init(fWorldWidth, fWorldDepth, fWorldHeight);

      return (pWorld != nullptr);
    }

    void Destroy()
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
      std::cout<<"bodies="<<lPhysicsBody.size()<<std::endl;
      while (iter != iterEnd) {
        (*iter)->Update(currentTime);

        iter++;
      }

      _Update(currentTime);
    }




    cBoxProperties::cBoxProperties() :
      fWeightKg(1.0f),
      fWidthMetres(1.0f),
    #ifdef BUILD_PHYSICS_3D
      fDepthMetres(1.0f),
    #endif
      fHeightMetres(1.0f)
    {
    };

    cSphereProperties::cSphereProperties() :
      fWeightKg(1.0f),
      fRadiusMetres(0.5f)
    {
    }




    cHeightmapProperties::cHeightmapProperties(const game::cTerrainHeightMap& _loader) :
      loader(_loader),
      width(1),
      height(1),
      scale(1.0f, 1.0f, 1.0f)
    {
    }

    cHeightmap::cHeightmap(const cHeightmapProperties& properties) :
      loader(properties.loader),
      width(properties.width),
      height(properties.height),
      position(properties.position),
      scale(properties.scale)
    {
    }


    cCarProperties::cCarProperties() :
      fSuspensionStiffness(5.88f),
      fSuspensionCompression(0.83f),
      fSuspensionDamping(0.88f),
      fSuspensionMaxTravelCentimetres(20.0f),

      fTireFrictionSlip(10.5f)
    {
    }
  }
}

