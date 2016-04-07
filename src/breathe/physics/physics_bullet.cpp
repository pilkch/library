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

// Other libraries
#include <btBulletDynamicsCommon.h>
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"


// Spitfire
#include <spitfire/spitfire.h>

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
      collisionConfiguration = new btDefaultCollisionConfiguration;
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
      LOG<<"cWorld::_Destroy"<<std::endl;

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
      pCar->CreateCar(this, properties);

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

    void cWorld::_Update(durationms_t currentTime)
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


    // TODO: Remove me
    physics::cBodyRef cWorld::CreateEmptyBody()
    {
      bullet::cBodyRef pBody(new bullet::cBody);

      lPhysicsBody.push_back(pBody);

      return pBody;
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


    btRigidBody* cBody::CreateLocalCreateRigidBody(cWorld* _pWorld, float mass, const btTransform& startTransform, btCollisionShape* shape)
    {
      pWorld = _pWorld;

      btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

      // Rigidbody is dynamic if and only if mass is non zero, otherwise static
      bool bIsDynamic = (mass != 0.0f);

      btVector3 localInertia(0,0,0);
      if (bIsDynamic) shape->calculateLocalInertia(mass,localInertia);

      // Using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

      btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

      btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

      bodyRigidBody = new btRigidBody(cInfo);

      pWorld->GetWorld()->addRigidBody(bodyRigidBody);

      return bodyRigidBody;
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


    void cBody::_Update(durationms_t currentTime)
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

    void cHeightmap::_Update(durationms_t currentTime)
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
      rayCaster(nullptr),
      vehicle(nullptr),
      wheelShape(nullptr)
    {
    }

    void cCar::CreateCar(cWorld* pWorld, const physics::cCarProperties& properties)
    {
      ///btRaycastVehicle is the interface for the constraint that implements the raycast vehicle
      ///notice that for higher-quality slow-moving vehicles, another approach might be better
      ///implementing explicit hinged-wheel constraints with cylinder collision, rather then raycasts
      //float gEngineForce = 0.f;
      //float gBrakingForce = 0.f;

      //float maxEngineForce = 1000.f;//this should be engine/velocity dependent
      //float maxBrakingForce = 100.f;

      float gVehicleSteering = 0.f;
      //float steeringIncrement = 0.04f;
      //float steeringClamp = 0.3f;
      float wheelRadius = 0.5f;
      float wheelWidth = 0.4f;
      float wheelFriction = 1000;//BT_LARGE_FLOAT;
      float suspensionStiffness = 20.f;
      float suspensionDamping = 2.3f;
      float suspensionCompression = 4.4f;
      float rollInfluence = 0.1f;//1.0f;










      btCollisionShape* chassisShape = new btBoxShape(btVector3(1.f,2.f, 0.5f));
      btCompoundShape* compound = new btCompoundShape();
      btTransform localTrans;
      localTrans.setIdentity();
      //localTrans effectively shifts the center of mass with respect to the chassis
      localTrans.setOrigin(btVector3(0,0,1));

      compound->addChildShape(localTrans,chassisShape);

      btTransform tr;
      tr.setIdentity();

      tr.setOrigin(btVector3(0,0.f,0));



      pChassis = pWorld->CreateEmptyBody();

      bullet::cBodyRef pChassisAsBulletBody = boost::static_pointer_cast<bullet::cBody>(pChassis);

      btRigidBody* m_carChassis = pChassisAsBulletBody->CreateLocalCreateRigidBody(pWorld, 800, tr, compound);
      //m_carChassis->setDamping(0.2,0.2);

      wheelShape = new btCylinderShapeX(btVector3(wheelWidth,wheelRadius,wheelRadius));






      //clientResetScene();

      gVehicleSteering = 0.f;
      m_carChassis->setCenterOfMassTransform(btTransform::getIdentity());
      m_carChassis->setLinearVelocity(btVector3(0,0,0));
      m_carChassis->setAngularVelocity(btVector3(0,0,0));
      pWorld->GetWorld()->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(m_carChassis->getBroadphaseHandle(), pWorld->GetWorld()->getDispatcher());
      if (vehicle) {
        vehicle->resetSuspension();
        for (int i=0; i < vehicle->getNumWheels();i++) {
          //synchronize the wheels with the (interpolated) chassis worldtransform
          vehicle->updateWheelTransform(i,true);
        }
      }










      // create vehicle
      {

        rayCaster = new btDefaultVehicleRaycaster(pWorld->GetWorld());
        vehicle = new btRaycastVehicle(tuning, m_carChassis, rayCaster);

        // never deactivate the vehicle
        m_carChassis->setActivationState(DISABLE_DEACTIVATION);

        pWorld->GetWorld()->addVehicle(vehicle);

        float connectionHeight = 1.2f;


        bool isFrontWheel=true;

        //choose coordinate system
        int rightIndex = 0;
        int upIndex = 2;
        int forwardIndex = 1;
        vehicle->setCoordinateSystem(rightIndex, upIndex, forwardIndex);

        btVector3 wheelDirectionCS0(0,0,-1);
        btVector3 wheelAxleCS(1,0,0);

        const float CUBE_HALF_EXTENTS = 1.0f;
        btScalar suspensionRestLength(0.6);

        btVector3 connectionPointCS0;

        connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS-(0.3*wheelWidth),2*CUBE_HALF_EXTENTS-wheelRadius, connectionHeight);
        vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspensionRestLength,wheelRadius, tuning, isFrontWheel);

        connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS+(0.3*wheelWidth),2*CUBE_HALF_EXTENTS-wheelRadius, connectionHeight);
        vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspensionRestLength,wheelRadius, tuning, isFrontWheel);

        isFrontWheel = false;
        connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS+(0.3*wheelWidth),-2*CUBE_HALF_EXTENTS+wheelRadius, connectionHeight);
        vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspensionRestLength,wheelRadius, tuning, isFrontWheel);

        connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS-(0.3*wheelWidth),-2*CUBE_HALF_EXTENTS+wheelRadius, connectionHeight);
        vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspensionRestLength,wheelRadius, tuning, isFrontWheel);


        for (int i=0;i<vehicle->getNumWheels();i++)
        {
          btWheelInfo& wheel = vehicle->getWheelInfo(i);
          wheel.m_suspensionStiffness = suspensionStiffness;
          wheel.m_wheelsDampingRelaxation = suspensionDamping;
          wheel.m_wheelsDampingCompression = suspensionCompression;
          wheel.m_frictionSlip = wheelFriction;
          wheel.m_rollInfluence = rollInfluence;
        }
      }

#if 0
      // Rigid body
      physics::cBoxProperties boxProperties;
      boxProperties.SetMassKg(properties.fMassKg);
      boxProperties.SetPositionAbsolute(properties.position);
      boxProperties.SetRotationAbsolute(properties.rotation);
      boxProperties.fWidthMetres = properties.fWidthMetres;
      boxProperties.fDepthMetres = properties.fDepthMetres;
      boxProperties.fHeightMetres = properties.fHeightMetres;

      pChassis = pWorld->CreateBody(boxProperties);




      // Tuning
      tuning.m_suspensionStiffness = properties.fSuspensionStiffness;
      tuning.m_suspensionCompression = properties.fSuspensionCompression;
      tuning.m_suspensionDamping = properties.fSuspensionDamping;
      tuning.m_maxSuspensionTravelCm = 500.0f;// properties.fSuspensionMaxTravelCentimetres;
      tuning.m_frictionSlip = properties.fTireFrictionSlip;



      bullet::cBodyRef pChassisAsBulletBody = boost::static_pointer_cast<bullet::cBody>(pChassis);
      btRigidBody* pChassisAsBulletRigidBody = pChassisAsBulletBody->GetBody();


      // Ray caster
      rayCaster = new btDefaultVehicleRaycaster(pWorld->GetWorld());
      ASSERT(rayCaster != nullptr);
      if (rayCaster == nullptr) return;


      vehicle = new btRaycastVehicle(tuning, pChassisAsBulletRigidBody, rayCaster);
      ASSERT(vehicle != nullptr);
      if (vehicle == nullptr) return;

      pChassisAsBulletRigidBody->setActivationState(DISABLE_DEACTIVATION);

      int rightIndex = 0;
      int upIndex = 2;
      int forwardIndex = 1;
      vehicle->setCoordinateSystem(rightIndex, upIndex, forwardIndex);

      const float fRestLengthRatio = 0.6f;//properties.fSuspensionRestLengthCentimetres / properties.fSuspensionMaxTravelCentimetres;



      const float fRollInfluence = 0.9f;
      /*const float fMaxEngineForce = 1600.0f;
      const float fMaxBrakingForce = 200.0f;
      const float fSteeringClamp = 0.5f;
      const float fFrontWheelShiftX = 0.0f;
      const float fRearWheelShiftX = 0.0f;*/

      //0.2f,     // roll influence
      //1600.0f,  // max engine force
      //200.0f,   // max breaking force
      //0.5f,     // steering clamp

      //0.0f,     // fwheel X shift
      //0.0f      // rwheel X shift

      btVector3 wheelDirectionCS0(0.0f, 0.0f, -1.0f); // Pointing down
      btVector3 wheelAxleCS(1.0f, 0.0f, 0.0f); // To the right

      float connectionHeight = 10.2f;


      btVector3 connectionPointCS0;

      const float CUBE_HALF_EXTENTS = properties.fWidthMetres * 0.5f;
      const float fWheelWidthMetres = 0.01f * properties.fWheelWidthCentimetres;
      const float fWheelRadiusMetres = 0.01f * properties.fWheelRadiusCentimetres;

      {
        const bool bIsFrontWheel = true;

        connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS-(0.3 * fWheelWidthMetres), 2 * CUBE_HALF_EXTENTS - fWheelRadiusMetres, connectionHeight);
        vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, fRestLengthRatio, fWheelRadiusMetres, tuning, bIsFrontWheel);

        connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3 * fWheelWidthMetres),2 * CUBE_HALF_EXTENTS - fWheelRadiusMetres, connectionHeight);
        vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, fRestLengthRatio, fWheelRadiusMetres, tuning, bIsFrontWheel);
      }

      {
        const bool bIsFrontWheel = false;

        connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3 * fWheelWidthMetres), -2 * CUBE_HALF_EXTENTS + fWheelRadiusMetres, connectionHeight);
        vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, fRestLengthRatio, fWheelRadiusMetres, tuning, bIsFrontWheel);

        connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS - (0.3 * fWheelWidthMetres), -2 * CUBE_HALF_EXTENTS + fWheelRadiusMetres, connectionHeight);
        vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, fRestLengthRatio, fWheelRadiusMetres, tuning, bIsFrontWheel);
      }

      for (int i = 0; i < vehicle->getNumWheels(); i++) {
        btWheelInfo& wheel = vehicle->getWheelInfo(i);
        wheel.m_suspensionStiffness = properties.fSuspensionStiffness;
        wheel.m_wheelsDampingRelaxation = properties.fSuspensionDamping;
        wheel.m_wheelsDampingCompression = properties.fSuspensionCompression;
        wheel.m_frictionSlip = properties.fTireFrictionSlip;
        wheel.m_rollInfluence = fRollInfluence;
      }

/*
      {
        const bool bIsFrontWheel = true;

        vehicle->addWheel(
          vehicle_config->wheel_fr_v,
          vehicle_config->wheel_direction_CS0,
          vehicle_config->wheel_axle_CS,
          btScalar(fRestLengthRatio),
          0.01 * fWheelRadiusCentimetres,
          tuning,
          bIsFrontWheel
        );

        vehicle->addWheel(
          vehicle_config->wheel_fl_v,
          vehicle_config->wheel_direction_CS0,
          vehicle_config->wheel_axle_CS,
          btScalar(fRestLengthRatio),
          0.01 * fWheelRadiusCentimetres,
          tuning,
          bIsFrontWheel
        );
      }

      {
        const bool bIsFrontWheel = false;

        vehicle->addWheel(
          vehicle_config->wheel_br_v,
          vehicle_config->wheel_direction_CS0,
          vehicle_config->wheel_axle_CS,
          btScalar(fRestLengthRatio),
          0.01 * fWheelRadiusCentimetres,
          tuning,
          bIsFrontWheel
        );

        vehicle->addWheel(
          vehicle_config->wheel_bl_v,
          vehicle_config->wheel_direction_CS0,
          vehicle_config->wheel_axle_CS,
          btScalar(fRestLengthRatio),
          0.01 * fWheelRadiusCentimetres,
          tuning,
          bIsFrontWheel
        );
      }

      for ( int i = 0; i < vehicle->getNumWheels(); i++) {
        btWheelInfo& wheel = vehicle->getWheelInfo(i);
        wheel.m_rollInfluence = fRollInfluence;
      }*/

      pWorld->GetWorld()->addVehicle(vehicle);
#endif
    }


    void cCar::_Update(durationms_t currentTime)
    {
      for (int i = 0; i < 4; i++) {
        vehicle->applyEngineForce(fWheelAccelerationForceNewtons[i], i);
        vehicle->setBrake(fWheelBrakingForceNewtons[i], i);
        vehicle->setSteeringValue(fWheelSteeringAngleMinusOneToPlusOne[i], i);

        // Synchronize the wheels with the (interpolated) chassis worldtransform
        vehicle->updateWheelTransform(i, true);


        // Update our wheel positions and rotations
        const btVector3 position = vehicle->getWheelInfo(i).m_worldTransform.getOrigin();
        const btQuaternion rotation = vehicle->getWheelInfo(i).m_worldTransform.getRotation();

        wheelPositionRelative[i].Set(position.x(), position.y(), position.z());

        wheelRotationRelative[i].x = rotation.x();
        wheelRotationRelative[i].y = rotation.y();
        wheelRotationRelative[i].z = rotation.z();
        wheelRotationRelative[i].w = rotation.w();
      }


      /*int wheelIndex = 2;
      m_vehicle->applyEngineForce(gEngineForce,wheelIndex);
      m_vehicle->setBrake(gBreakingForce,wheelIndex);
      wheelIndex = 3;
      m_vehicle->applyEngineForce(gEngineForce,wheelIndex);
      m_vehicle->setBrake(gBreakingForce,wheelIndex);


      wheelIndex = 0;
      m_vehicle->setSteeringValue(gVehicleSteering,wheelIndex);
      wheelIndex = 1;
      m_vehicle->setSteeringValue(gVehicleSteering,wheelIndex);*/



      /*// apply engine Force on relevant wheels
      for (int i = mWheelsEngine[0]; i < mWheelsEngineCount; i++) {
        mVehicle->applyEngineForce(mEngineForce, mWheelsEngine[i]);
      }

      if (mSteeringLeft)
      {
          mSteering += gSteeringIncrement;
          if (mSteering > gSteeringClamp)
              mSteering = gSteeringClamp;
      }
      else if (mSteeringRight)
      {
          mSteering -= gSteeringIncrement;
          if (mSteering < -gSteeringClamp)
              mSteering = -gSteeringClamp;
      }

      // apply Steering on relevant wheels
      for (int i = mWheelsSteerable[0]; i < mWheelsSteerableCount; i++)
      {
          if (i < 2)
              mVehicle->setSteeringValue (mSteering, mWheelsSteerable[i]);
          else
              mVehicle->setSteeringValue (-mSteering, mWheelsSteerable[i]);
      }*/
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


      btRaycastVehicle::btVehicleTuning tuning;
      btVehicleRaycaster*     vehicleRayCaster;
      btRaycastVehicle*       vehicle;
      btCollisionShape*       wheelShape;

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

