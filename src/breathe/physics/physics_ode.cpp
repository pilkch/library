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
#include <ode/ode.h>


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
#include <breathe/physics/physics_ode.h>


namespace breathe
{
  namespace ode
  {
    // *** Physics data
    const int iMaxContacts = 100;
    const float fFriction = 2000.0f;
    const float fBounce = 0.003f;
    const float fBounceVel = 0.00002f;
    const float fERP = 0.8f;
    const float fCFM = (float)(10e-5);
    const float fGravity = -9.80665f;
    const float fDampTorque = 0.02f;
    const float fDampLinearVel = 0.02f;


    cWorld::cWorld() :
      world(0),
      spaceStatic(0),
      spaceDynamic(0),
      contactGroup(0),
      ground(0)
    {
      // Timing
      uiFrequencyHz = 20;
      fIntervalMS = 1000.0f / float(uiFrequencyHz);
    }


    bool cWorld::_Init(float fWorldWidth, float fWorldDepth, float fWorldHeight)
    {
      dInitODE2(0);

      const char* szExtensions = dGetConfiguration();
      std::cout<<"physics::cWorld::Init Extensions \""<<szExtensions<<"\""<<std::endl;

      // ODE_single_precision
      // ODE_double_precision
      // ODE_EXT_no_debug
      // ODE_EXT_trimesh


      world = dWorldCreate();

      dWorldSetGravity(world, 0, 0, fGravity);

      // This function sets the depth of the surface layer around the world objects. Contacts are allowed to sink into
      // each other up to this depth. Setting it to a small value reduces the amount of jittering between contacting
      // objects, the default value is 0.
      dWorldSetContactSurfaceLayer(world, 0.001f);

      dWorldSetERP(world, fERP);
      dWorldSetCFM(world, fCFM);

      dWorldSetContactMaxCorrectingVel(world, 1.0f);
      dWorldSetAutoDisableFlag(world, 1);

      spaceStatic = dHashSpaceCreate(0);
      spaceDynamic = dHashSpaceCreate(0);

      contactGroup = dJointGroupCreate(0);

      // Now create our default ground
      CreateGround();

      return true;
    }

    void cWorld::_Destroy()
    {
      dJointGroupDestroy(contactGroup);

      // Remove all bodies
      while (!lPhysicsBody.empty()) {
        body_iterator iter = lPhysicsBody.begin();
        DestroyBody(*iter);
      }

      lPhysicsBody.clear();


      dSpaceDestroy(spaceDynamic);
      dSpaceDestroy(spaceStatic);

      dWorldDestroy(world);

      dCloseODE();
    }

    void cWorld::CreateGround()
    {
      const math::cVec3 position(0.0f, 0.0f, 0.0f);
      const math::cVec3 normal(math::v3Up);

      ground = dCreatePlane(spaceStatic, normal.x, normal.y, normal.z, normal.DotProduct(position));
    }

    void cWorld::DestroyGround()
    {
      dGeomDestroy(ground);
      ground = NULL;
    }

    physics::cBodyRef cWorld::_CreateBody(const physics::cBoxProperties& properties)
    {
      ode::cBodyRef pBody(new ode::cBody);
      pBody->CreateBox(this, properties);

      lPhysicsBody.push_back(pBody);

      return pBody;
    }

    physics::cBodyRef cWorld::_CreateBody(const physics::cSphereProperties& properties)
    {
      ode::cBodyRef pBody(new ode::cBody);
      pBody->CreateSphere(this, properties);

      lPhysicsBody.push_back(pBody);

      return pBody;
    }

    physics::cHeightmapRef cWorld::_CreateHeightmap(const physics::cHeightmapProperties& properties)
    {
      ode::cHeightmapRef pHeightmap(new ode::cHeightmap(properties));
      //lPhysicsHeightmap.push_back(pHeightmap);

      return pHeightmap;
    }

    physics::cCarRef cWorld::_CreateCar(const physics::cCarProperties& properties)
    {
      ode::cBodyRef pBody(new ode::cBody);

      lPhysicsBody.push_back(pBody);

      ode::cCarRef pCar(new ode::cCar(pBody));

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
      {
        // First Iteration
        // Generic rotational and linear dampening

        body_iterator iter = lPhysicsBody.begin();
        const body_iterator iterEnd = lPhysicsBody.end();
        while(iterEnd != iter) {
          ode::cBodyRef pBody = boost::static_pointer_cast<ode::cBody>(*iter);
          dBodyID b = pBody->GetBody();
          if (b != NULL) {
            dReal const* av = dBodyGetAngularVel(b);
            dBodySetAngularVel(b, av[0] - av[0] * fDampTorque, av[1] - av[1] * fDampTorque, av[2] - av[2] * fDampTorque);
            dReal const* lv = dBodyGetLinearVel(b);
            dBodySetLinearVel(b, lv[0] - lv[0] * fDampLinearVel, lv[1] - lv[1] * fDampLinearVel, lv[2] - lv[2] * fDampLinearVel);
          }

          iter++;
        };
      }

      // For triggers
      // Was using an object with this code
      // dSpaceCollide(spaceDynamic, this, nearCallbackDynamic);
      // dSpaceCollide2((dGeomID)spaceDynamic, (dGeomID)spaceStatic, this, &nearCallbackStatic);
      // dSpaceCollide2((dGeomID)spaceDynamic, (dGeomID)spaceTrigger, this, &nearCallbackTrigger);

      //std::cout<<"cWorld::Update Calling dSpaceCollide"<<std::endl;
      dSpaceCollide(spaceDynamic, this, _CollisionCallbackDynamic);
      //std::cout<<"cWorld::Update Calling dSpaceCollide2"<<std::endl;
      dSpaceCollide2((dGeomID)spaceDynamic, (dGeomID)spaceStatic, this, &_CollisionCallbackStatic);

      // For triggers
      //dSpaceCollide2((dGeomID)spaceDynamic, (dGeomID)spaceTrigger, this, &nearCallbackTrigger);

      std::cout<<"cWorld::Update Calling dWorldQuickStep interval="<<fIntervalMS / 1000.0f<<" objects="<<lPhysicsBody.size()<<std::endl;
      dWorldQuickStep(world, fIntervalMS / 1000.0f);

      //std::cout<<"cWorld::Update Calling dJointGroupEmpty"<<std::endl;
      dJointGroupEmpty(contactGroup);




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


    void cWorld::CollisionCallbackStatic(dGeomID o1, dGeomID o2)
    {
      // Ignore collisions between NULL geometry
      if ((o1 == 0) && (o2 == 0)) {
        LOG.Error("cWorld::CollisionCallbackStatic", "NULL geometry");
        return;
      }

      // Ignore collisions between bodies that are connected by the same joint
      dBodyID Body1 = NULL;
      dBodyID Body2 = NULL;

      if (o1 != 0) Body1 = dGeomGetBody (o1);
      if (o2 != 0) Body2 = dGeomGetBody (o2);

      if ((Body1 != 0) && (Body2 != 0) && dAreConnected (Body1, Body2)) return;

      const int N = iMaxContacts;
      dContact contact[N];

      const size_t n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
      for (size_t i = 0; i < n; i++) {
        contact[i].surface.mode = dContactBounce;
        contact[i].surface.mu = fFriction;
        contact[i].surface.bounce = fBounce;
        contact[i].surface.bounce_vel = fBounceVel;

        dJointID c = dJointCreateContact(world, contactGroup, &contact[i]);
        dJointAttach(c, Body1, Body2);
      }
    }

    void cWorld::_CollisionCallbackStatic(void* pUserData, dGeomID o1, dGeomID o2)
    {
      ASSERT(pUserData != nullptr);
      cWorld* pWorld = (cWorld*)pUserData;
      pWorld->CollisionCallbackStatic(o1, o2);
    }


    void cWorld::CollisionCallbackDynamic(dGeomID o1, dGeomID o2)
    {
      //Ignore collisions between NULL geometry
      if ((o1 == 0) && (o2 == 0)) {
        LOG.Error("cWorld::CollisionCallbackDynamic", "NULL geometry");
        return;
      }

      //Ignore collisions between bodies that are connected by the same joint
      dBodyID Body1 = NULL;
      dBodyID Body2 = NULL;

      if (o1 != 0) Body1 = dGeomGetBody(o1);
      if (o2 != 0) Body2 = dGeomGetBody(o2);

      if ((Body1 != 0) && (Body2 != 0) && dAreConnected(Body1, Body2)) return;

      const int N = iMaxContacts;
      dContact contact[N];

      const size_t n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
      for (size_t i = 0; i < n; i++) {
        contact[i].surface.mode = dContactBounce;
        contact[i].surface.mu = fFriction;
        contact[i].surface.bounce = fBounce;
        contact[i].surface.bounce_vel = fBounceVel;

        dJointID c = dJointCreateContact(world, contactGroup, &contact[i]);
        dJointAttach(c, Body1, Body2);
      }
    }

    void cWorld::_CollisionCallbackDynamic(void* pUserData, dGeomID o1, dGeomID o2)
    {
      ASSERT(pUserData != nullptr);
      cWorld* pWorld = (cWorld*)pUserData;
      pWorld->CollisionCallbackDynamic(o1, o2);
    }




    cBody::cBody() :
      body(NULL),
      geom(NULL)
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

    void cBody::InitCommon(cWorld* pWorld)
    {
      const math::cMat4 m = rotation.GetMatrix();

      dMatrix3 r;
      r[0] = m[0];    r[1] = m[4];    r[2] = m[8];    r[3] = 0;
      r[4] = m[1];    r[5] = m[5];    r[6] = m[9];    r[7] = 0;
      r[8] = m[2];    r[9] = m[6];    r[10] = m[10];  r[11] = 0;


      //position.z += fHeight;

      ASSERT(geom != NULL);
      dGeomSetPosition(geom, position.x, position.y, position.z);
      dGeomSetRotation(geom, r);


      //if (bHasBody) {
      body = dBodyCreate(pWorld->GetWorld());
      dBodySetPosition(body, position.x, position.y, position.z);
      dBodySetRotation(body, r);
      dBodySetAutoDisableFlag(body, 1);

      dGeomSetBody(geom, body);
      //}
    }

    void cBody::CreateBox(cWorld* pWorld, const physics::cBoxProperties& properties)
    {
      position = properties.position;
      rotation = properties.rotation;
      fMassKg = properties.fMassKg;

      geom = dCreateBox(pWorld->GetSpaceDynamic(), properties.fWidthMetres, properties.fDepthMetres, properties.fHeightMetres);

      InitCommon(pWorld);

      dMass mass;
      dMassSetBoxTotal(&mass, fMassKg, properties.fWidthMetres, properties.fDepthMetres, properties.fHeightMetres);
      dBodySetMass(body, &mass);
    }

    void cBody::CreateSphere(cWorld* pWorld, const physics::cSphereProperties& properties)
    {
      position = properties.position;
      rotation = properties.rotation;
      fMassKg = properties.fMassKg;

      geom = dCreateSphere(pWorld->GetSpaceDynamic(), properties.fRadiusMetres);

      InitCommon(pWorld);

      dMass mass;
      dMassSetSphereTotal(&mass, fMassKg, 2.0f * properties.fRadiusMetres);
      dBodySetMass(body, &mass);
    }


    void cBody::_AddForceRelativeToWorldKg(const physics::physvec_t& forceKg)
    {
      ASSERT(body != NULL);
      dBodyAddForce(body, forceKg.x, forceKg.y, forceKg.z);
    }

    void cBody::_AddTorqueRelativeToWorldNm(const physics::physvec_t& torqueNm)
    {
      ASSERT(body != NULL);
      dBodyAddTorque(body, torqueNm.x, torqueNm.y, torqueNm.z);
    }

    void cBody::_AddForceRelativeToObjectKg(const physics::physvec_t& forceKg)
    {
      ASSERT(body != NULL);
      dBodyAddRelForce(body, forceKg.x, forceKg.y, forceKg.z);
    }

    void cBody::_AddTorqueRelativeToObjectNm(const physics::physvec_t& torqueNm)
    {
      ASSERT(body != NULL);
      dBodyAddRelTorque(body, torqueNm.x, torqueNm.y, torqueNm.z);
    }


    void cBody::_Update(sampletime_t currentTime)
    {
      if (body != NULL) {
        const dReal* p0 = dBodyGetPosition(body);
        const dReal* r0 = dBodyGetQuaternion(body);
        //const dReal* v0 = dBodyGetLinearVel(body);
        //const dReal *a0 = dBodyGetAngularVel(body);

        ASSERT(p0 != nullptr);
        ASSERT(r0 != nullptr);
        position.Set(p0[0], p0[1], p0[2]);
        rotation.SetFromODEQuaternion(r0);
      }
    }

    void cBody::_Remove()
    {
      if (body != NULL) {
        dBodyDestroy(body);
        body = NULL;
      }

      if (geom != NULL) {
        dGeomDestroy(geom);
        geom = NULL;
      }
    }




    cHeightmap::cHeightmap(const physics::cHeightmapProperties& properties) :
      physics::cHeightmap(properties),
      heightfield(NULL),
      pHeightfieldData(nullptr)
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
      if (heightfield != NULL) {
        dGeomHeightfieldDataDestroy(heightfield);
        heightfield = NULL;
      }

      SAFE_DELETE_ARRAY(pHeightfieldData);
    }



    cCar::cCar(cBodyRef _pBody)
    {
      pBody = _pBody;
    }


    /*const float fEquilibriumLengthCentimetres = ;
    const float fCurrentLengthCentimetres = ;


    // Spring rate
    // Spring rates typically have units of N/mm (Newton per millimetre). An example of a linear spring rate is 500 lbf/in. For every inch the spring is compressed, it exerts 500 lbf.
    // http://en.wikipedia.org/wiki/Suspension_%28vehicle%29#Mathematics_of_the_spring_rate
    // k = (pow(d, 4) * G) / (8 * N * pow(d, 3))
    // d is the wire diameter
    // G is the springs shear modulus (e.g., about 12,000,000 lbf/inÂ² or 80 GPa for steel)
    // N is the number of wraps
    // D is the diameter of the coil.

    const float d = ; // wire diameter
    const float G = ; // springs shear modulus (e.g., about 12,000,000 lbf/inÂ² or 80 GPa for steel)
    const float N = ; // number of wraps and D is the diameter of the coil.

    const float k = (pow(d, 4) * G) / (8 * N * pow(d, 3)); // spring rate of the spring

    // Force
    // http://en.wikipedia.org/wiki/Suspension_%28vehicle%29#Spring_rate
    // F = -kx
    // F is the force the spring exerts
    // k is the spring rate of the spring.
    // x is the displacement from equilibrium length i.e. the length at which the spring is neither compressed or stretched.

    const float x = fEquilibriumLengthCentimetres - fCurrentLengthCentimetres; // displacement from equilibrium length i.e. the length at which the spring is neither compressed or stretched

    const float F = -kx;*/
  }
}

