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
#include <btBulletDynamicsCommon.h>
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"


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
#include <breathe/physics/physics_bullet.h>

namespace breathe
{
  namespace bullet
  {
    const float fGravity = -9.80665f;


    cWorld::cWorld() :
      broadphase(nullptr),
      collisionConfiguration(nullptr),
      dispatcher(nullptr),
      solver(nullptr),
      dynamicsWorld(nullptr),
      groundShape(nullptr),
      groundRigidBody(nullptr)
    {
      // Timing
      uiFrequencyHz = 60;
      fIntervalMS = 1000.0f / float(uiFrequencyHz);
    }

    bool cWorld::_Init(float fWorldWidth, float fWorldDepth, float fWorldHeight)
    {
      // http://bulletphysics.org/mediawiki-1.5.8/index.php/Hello_World

      int maxProxies = 1024;
      btVector3 worldAabbMin(-fWorldWidth * 0.5f, -fWorldDepth * 0.5f, -fWorldHeight * 0.5f);
      btVector3 worldAabbMax(fWorldWidth * 0.5f, fWorldDepth * 0.5f, fWorldHeight * 0.5f);

      // Build the broadphase
      broadphase = new btAxisSweep3(worldAabbMin, worldAabbMax, maxProxies);

      // Set up the collision configuration and dispatcher
      collisionConfiguration = new btDefaultCollisionConfiguration();
      dispatcher = new btCollisionDispatcher(collisionConfiguration);

      // The actual physics solver
      solver = new btSequentialImpulseConstraintSolver;

      // The world
      dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);


      dynamicsWorld->setGravity(btVector3(0.0f, 0.0f, fGravity));


      // Now create our default ground
      CreateGround();

      return true;
    }

    void cWorld::_Destroy()
    {
      // Remove all bodies
      while (!lPhysicsBody.empty()) {
        body_iterator iter = lPhysicsBody.begin();
        DestroyBody(*iter);
      }

      lPhysicsBody.clear();


      // Destroy the ground
      SAFE_DELETE(groundShape);

      // Destroy our world and dynamics helpers
      SAFE_DELETE(dynamicsWorld);
      SAFE_DELETE(solver);
      SAFE_DELETE(dispatcher);
      SAFE_DELETE(collisionConfiguration);
      SAFE_DELETE(broadphase);
    }

    void cWorld::CreateGround()
    {
      const math::cVec3 position(0.0f, 0.0f, 0.0f);
      const math::cVec3 normal(math::v3Up);

      groundShape = new btStaticPlaneShape(btVector3(0.0f, 0.0f, 1.0f), 1.0f);

      btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(position.x, position.y, position.z)));
      btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0.0f, groundMotionState, groundShape, btVector3(0.0f, 0.0f, 0.0f));
      btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
      dynamicsWorld->addRigidBody(groundRigidBody);
    }

    void cWorld::DestroyGround()
    {
      dynamicsWorld->removeRigidBody(groundRigidBody);
      delete groundRigidBody->getMotionState();
      SAFE_DELETE(groundRigidBody);
      SAFE_DELETE(groundShape);
    }

    physics::cBodyRef cWorld::_CreateBody(const physics::cBoxProperties& properties)
    {
      bullet::cBodyRef pBody(new bullet::cBody);
      pBody->CreateBox(this, properties);

      lPhysicsBody.push_back(pBody);

      return pBody;
    }

    physics::cBodyRef cWorld::_CreateBody(const physics::cSphereProperties& properties)
    {
      bullet::cBodyRef pBody(new bullet::cBody);
      pBody->CreateSphere(this, properties);

      lPhysicsBody.push_back(pBody);

      return pBody;
    }

    physics::cHeightmapRef cWorld::_CreateHeightmap(const physics::cHeightmapProperties& properties)
    {
      bullet::cHeightmapRef pHeightmap(new bullet::cHeightmap(properties));
      //lPhysicsHeightmap.push_back(pHeightmap);

      return pHeightmap;
    }

    physics::cCarRef cWorld::_CreateCar(const physics::cCarProperties& properties)
    {
      bullet::cCarRef pCar(new bullet::cCar);

      return pCar;
    }

    void cWorld::_DestroyBody(physics::cBodyRef pBody)
    {
      lPhysicsBody.remove(pBody);
    }

    void cWorld::_DestroyCar(physics::cCarRef pCar)
    {
      //lPhysicsBody.remove(pCar->GetBody());
    }

    void cWorld::_Update(sampletime_t currentTime)
    {
      /*{
        // First Iteration
        // Generic rotational and linear dampening

        body_iterator iter = lPhysicsBody.begin();
        const body_iterator iterEnd = lPhysicsBody.end();
        while(iterEnd != iter) {
          bullet::cBodyRef pBody = boost::static_pointer_cast<bullet::cBody>(*iter);
          dBodyID b = pBody->GetBody();
          if (b != NULL) {
            dReal const* av = dBodyGetAngularVel(b);
            dBodySetAngularVel(b, av[0] - av[0] * fDampTorque, av[1] - av[1] * fDampTorque, av[2] - av[2] * fDampTorque);
            dReal const* lv = dBodyGetLinearVel(b);
            dBodySetLinearVel(b, lv[0] - lv[0] * fDampLinearVel, lv[1] - lv[1] * fDampLinearVel, lv[2] - lv[2] * fDampLinearVel);
          }

          iter++;
        };
      }*/

      std::cout<<"cWorld::Update Calling dWorldQuickStep interval="<<fIntervalMS / 1000.0f<<" objects="<<lPhysicsBody.size()<<std::endl;
      dynamicsWorld->stepSimulation(1.0f / float(uiFrequencyHz), 10);


      //std::cout<<"cWorld::Update Updating each object"<<std::endl;
      {
        body_iterator iter = lPhysicsBody.begin();
        const body_iterator iterEnd = lPhysicsBody.end();
        while (iter != iterEnd) {
          (*iter)->Update(currentTime);

          iter++;
        }
      }

      //std::cout<<"cWorld::Update returning"<<std::endl;
    }





    cBody::cBody() :
      pWorld(nullptr),
      bodyShape(nullptr),
      bodyMotionState(nullptr),
      bodyRigidBody(nullptr)
    {
    }

    void cBody::_SetPositionAbsolute(const spitfire::math::cVec3& position)
    {
    }

    void cBody::_SetRotationAbsolute(const spitfire::math::cQuaternion& rotation)
    {
    }

    void cBody::_SetMassKg(float fMassKg)
    {
    }

    void cBody::InitCommon()
    {
      bodyMotionState = new btDefaultMotionState(btTransform(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w), btVector3(position.x, position.y, position.z)));
      btScalar mass = fMassKg;
      btVector3 bodyInertia(0, 0, 0);
      bodyShape->calculateLocalInertia(mass, bodyInertia);
      btRigidBody::btRigidBodyConstructionInfo bodyRigidBodyCI(mass, bodyMotionState, bodyShape, bodyInertia);
      bodyRigidBody = new btRigidBody(bodyRigidBodyCI);
      pWorld->GetWorld()->addRigidBody(bodyRigidBody);
    }

    void cBody::CreateBox(cWorld* _pWorld, const physics::cBoxProperties& properties)
    {
      pWorld = _pWorld;

      position = properties.position;
      rotation = properties.rotation;
      fMassKg = properties.fMassKg;

      btVector3 dimensions(properties.fWidthMetres, properties.fDepthMetres, properties.fHeightMetres);
      bodyShape = new btBoxShape(dimensions);

      InitCommon();
    }

    void cBody::CreateSphere(cWorld* _pWorld, const physics::cSphereProperties& properties)
    {
      pWorld = _pWorld;

      position = properties.position;
      rotation = properties.rotation;
      fMassKg = properties.fMassKg;

      bodyShape = new btSphereShape(properties.fRadiusMetres);

      InitCommon();
    }


    void cBody::_AddForceRelativeToWorldKg(const physics::physvec_t& forceKg)
    {
      ASSERT(bodyRigidBody != nullptr);
      const btVector3 force(forceKg.x, forceKg.y, forceKg.z);

      bodyRigidBody->activate();
      bodyRigidBody->applyCentralForce(force);
    }

    void cBody::_AddTorqueRelativeToWorldNm(const physics::physvec_t& torqueNm)
    {
      ASSERT(bodyRigidBody != nullptr);
      const btVector3 torque(torqueNm.x, -torqueNm.y, torqueNm.z);

      bodyRigidBody->activate();
      bodyRigidBody->applyTorque(torque);
    }

    void cBody::_AddForceRelativeToObjectKg(const physics::physvec_t& forceKg)
    {
      ASSERT(bodyRigidBody != nullptr);
      btVector3 force(forceKg.x, forceKg.y, forceKg.z);

      //bodyRigidBody->activate();
      {
        btTransform transform = bodyRigidBody->getCenterOfMassTransform();
        force = transform.getBasis() * force;
      }

      bodyRigidBody->activate();
      bodyRigidBody->applyCentralForce(force);
    }

    void cBody::_AddTorqueRelativeToObjectNm(const physics::physvec_t& torqueNm)
    {
      ASSERT(bodyRigidBody != nullptr);
      btVector3 torque(torqueNm.x, -torqueNm.y, torqueNm.z);

      //m_body->activate();
      {
        btTransform transform = bodyRigidBody->getCenterOfMassTransform();
        torque = transform.getBasis() * torque;
      }

      bodyRigidBody->activate();
      bodyRigidBody->applyTorque(torque);
    }


    void cBody::_Update(sampletime_t currentTime)
    {
      if (bodyRigidBody != nullptr) {
        btTransform trans;
        bodyRigidBody->getMotionState()->getWorldTransform(trans);

        btVector3 p = trans.getOrigin();
        position.Set(p.getX(), p.getY(), p.getZ());

        btQuaternion r = trans.getRotation();
        rotation.x = r.getX();
        rotation.y = r.getY();
        rotation.z = r.getZ();
        rotation.w = r.getW();
      }
    }

    void cBody::_Remove()
    {
      if (bodyRigidBody != nullptr) {
        pWorld->GetWorld()->removeRigidBody(bodyRigidBody);
        delete bodyRigidBody->getMotionState();
        SAFE_DELETE(bodyRigidBody);
      }
    }




    cHeightmap::cHeightmap(const physics::cHeightmapProperties& properties) :
      physics::cHeightmap(properties),
      heightmapShape(nullptr)
    {
    }

    void cHeightmap::CreateHeightmap(cWorld* pWorld)
    {
/*
    void cPhysicsObject::CreateHeightmap(cWorld* pWorld, const game::cTerrainHeightMap& loader, size_t width, size_t length, const physvec_t& scale, const physvec_t& pos, const physvec_t& rot)
    {
      // NOTE: Actual heightmap doesn't seem to be working at the moment so we use a trimesh instead

#if 1
      // TODO: Shared indices
      const float fScale = scale.x;

      std::vector<spitfire::math::cVec3> tempVertices;
      std::vector<uint32_t> tempIndices;
      tempVertices.reserve(length * width * 4);
      tempIndices.reserve(length * width * 6);

      size_t index = 0;
      for (size_t y = 0; y < length; y++) {
        for (size_t x = 0; x < width; x++) {
          const float fX = float(x) * fScale;
          const float fY = float(y) * fScale;

          tempVertices.push_back(spitfire::math::cVec3(fX, fY, loader.GetHeight(fX, fY)));

          tempVertices.push_back(spitfire::math::cVec3(fX + fScale, fY, loader.GetHeight(fX + fScale, fY)));

          tempVertices.push_back(spitfire::math::cVec3(fX + fScale, fY + fScale, loader.GetHeight(fX + fScale, fY + fScale)));

          tempVertices.push_back(spitfire::math::cVec3(fX, fY + fScale, loader.GetHeight(fX, fY + fScale)));


          tempIndices.push_back(index + 0);
          tempIndices.push_back(index + 1);
          tempIndices.push_back(index + 2);

          tempIndices.push_back(index + 2);
          tempIndices.push_back(index + 3);
          tempIndices.push_back(index + 0);

          index += 4;
        }
      }

      CreateTrimesh(pWorld, tempVertices, tempIndices, pos, rot);

#else
      DestroyHeightfield();

      bBody = false;
      bDynamic = false;

      // Heightfields are always static
      v[0] = 0.0f;
      v[1] = 0.0f;
      v[2] = 0.0f;


      fWidth = float(width) * scale.x;
      fLength = float(length) * scale.y;
      fHeight = 1.0f * scale.z;

      const size_t height = length;

      pHeightfieldData = new float[width * height];
      for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
          pHeightfieldData[(width * y) + x] = 200.0f * (sinf(float(x)) + cosf(float(y)));// 10.0f * loader.GetHeight(x, y);
        }
      }

      heightfield = dGeomHeightfieldDataCreate();

      // TODO: Last parameter, tile indefinitely
      int copyHeightData = 1; // 0 = we are keeping our own copy (Dynamic), 1 = copy the data to ODE (static)
      int wrap = 1; // 0 = don't wrap, 1 = wrap indefinitely
      float fThickness = 10.0f; // The thickness of the "shell" of the heightmap
      dGeomHeightfieldDataBuildSingle(
        heightfield, pHeightfieldData, copyHeightData,
        fWidth, fLength,
        width, height,
        scale.z, pos.z, fThickness, wrap
      );

      // Set finite AABB
      dGeomHeightfieldDataSetBounds(heightfield, 0.0f, 1000.0f);

      int movable = 1; // 1 = true, 0 = false
      geom = dCreateHeightfield(pWorld->GetSpaceStatic(), heightfield, movable);
      ASSERT(geom != 0);

      InitCommon(pWorld, pos, rot);
#endif
    }

    void cPhysicsObject::CreateTrimesh(cWorld* pWorld, const std::vector<spitfire::math::cVec3>& coords, const std::vector<unsigned int>& indices, const physvec_t& pos, const physvec_t& rot)
    {
      vVertices = coords;
      vIndices = indices;

      bBody = false;
      bDynamic = false;

      // Trimeshes are static for the moment
      v[0] = 0.0f;
      v[1] = 0.0f;
      v[2] = 0.0f;

      dTriMeshDataID trimeshData = dGeomTriMeshDataCreate();

      const int VertexCount = vVertices.size();
      const int IndexCount = vIndices.size();

      dGeomTriMeshDataBuildSingle(
        trimeshData,
        (const void*)vVertices.data(), sizeof(spitfire::math::cVec3), (int)VertexCount, // Faces
        (const void*)vIndices.data(), (int)IndexCount, 3 * sizeof(uint32_t) // Indices
      );

      geom = dCreateTriMesh(pWorld->GetSpaceStatic(), trimeshData, NULL , NULL , NULL);

      InitCommon(pWorld, pos, rot);
    }
#endif*/
    }

    void cHeightmap::_Update(sampletime_t currentTime)
    {
    }

    void cHeightmap::_Remove()
    {
      /*if (heightfield != NULL) {
        dGeomHeightfieldDataDestroy(heightfield);
        heightfield = NULL;
      }

      SAFE_DELETE_ARRAY(pHeightfieldData);*/
    }



    cCar::cCar() :
      m_vehicleRayCaster(nullptr),
      m_vehicle(nullptr),
      m_wheelShape(nullptr)
    {
    }



/*
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
    };*/
  }
}

