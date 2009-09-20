#include <cmath>

#include <list>
#include <vector>

#include <sstream>
#include <iostream>
#include <fstream>

// Boost includes
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>


#include <ode/ode.h>

// Spitfire headers
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
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/geometry.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>

#include <breathe/physics/physics.h>


/*// Wedge
float sPrefabVertsTriangle[ 6 ][ 3 ] = {
  { -0.5f , -0.5f , 0.5f },
  { 0.5f , 0.5f , 0.5f },
  { -0.5f , 0.5f , 0.5f },
  { -0.5f , -0.5f , -0.5f },
  { 0.5f , 0.5f , -0.5f },
  { -0.5f , 0.5f , -0.5f }
};

int sPrefabIndicesTriangle[ 6 ][ 3 ] = {
  { 0 , 3 , 1 },
  { 1 , 3 , 4 },
  { 3 , 0 , 5 },
  { 5 , 0 , 2 },
  { 2 , 1 , 5 },
  { 1 , 4 , 5 },
};*/


const unsigned int uiDirectionX = 1;
const unsigned int uiDirectionY = 2;
const unsigned int uiDirectionZ = 3;

namespace breathe
{
  namespace physics
  {
    cPhysicsObject::cPhysicsObject() :
      bBody(true),
      bDynamic(true),
      body(NULL),
      geom(NULL),
      heightfield(NULL),
      pHeightfieldData(nullptr)
    {
      fRadius = 2.0f;
    }

    cPhysicsObject::~cPhysicsObject()
    {
      RemoveFromWorld();
    }

    void cPhysicsObject::RemoveFromWorld()
    {
      DestroyBody();
      DestroyGeom();
      DestroyHeightfield();
    }

    void cPhysicsObject::DestroyBody()
    {
      if (body != NULL) {
        dBodyDestroy(body);
        body = NULL;
      }
    }

    void cPhysicsObject::DestroyGeom()
    {
      if (geom != NULL) {
        dGeomDestroy(geom);
        geom = NULL;
      }
    }

    void cPhysicsObject::DestroyHeightfield()
    {
      if (heightfield != NULL) {
        dGeomHeightfieldDataDestroy(heightfield);
        heightfield = NULL;
      }

      SAFE_DELETE_ARRAY(pHeightfieldData);
    }

#ifdef BUILD_PHYSICS_3D
    void cPhysicsObject::InitCommon(cWorld* pWorld, const physvec_t& posOriginal, const physvec_t& rot)
    {
      math::cVec3 pos(posOriginal.x, posOriginal.y, posOriginal.z + fHeight);

      rotation.LoadIdentity();
      rotation.SetFromAngles(rot * math::cPI_DIV_180);

      const math::cMat4 m = rotation.GetMatrix();

      dMatrix3 r;
      r[0] = m[0];    r[1] = m[4];    r[2] = m[8];    r[3] = 0;
      r[4] = m[1];    r[5] = m[5];    r[6] = m[9];    r[7] = 0;
      r[8] = m[2];    r[9] = m[6];    r[10] = m[10];  r[11] = 0;

      position = pos;

      dGeomSetPosition(geom, position.x, position.y, position.z);
      dGeomSetRotation(geom, r);

      if (bBody) {
        body = dBodyCreate(pWorld->GetWorld());
        dBodySetPosition(body, position.x, position.y, position.z);
        dBodySetRotation(body, r);
        dBodySetAutoDisableFlag(body, 1);

        dGeomSetBody(geom, body);

        pWorld->AddPhysicsObject(shared_from_this());
      }
    }

    void cPhysicsObject::CreateBox(cWorld* pWorld, const math::cVec3& pos, const math::cVec3& rot)
    {
      geom = dCreateBox(bDynamic ? pWorld->GetSpaceDynamic() : pWorld->GetSpaceStatic(),
        2.0f*fWidth, 2.0f*fLength, 2.0f*fHeight);

      InitCommon(pWorld, pos, rot);

      if (bBody) {
        dMass mass;
        dMassSetBoxTotal(&mass, fWeightKg, 2.0f * fWidth, 2.0f * fLength, 2.0f * fHeight);
        dBodySetMass(body, &mass);
      }
    }

    void cPhysicsObject::CreateSphere(cWorld* pWorld, const math::cVec3& pos, const math::cVec3& rot)
    {
      geom = dCreateSphere(bDynamic ? pWorld->GetSpaceDynamic() : pWorld->GetSpaceStatic(), fRadius);

      InitCommon(pWorld, pos, rot);

      if (bBody) {
        dMass mass;
        dMassSetSphereTotal(&mass, fWeightKg, 2.0f * fRadius);
        dBodySetMass(body, &mass);
      }
    }

    void cPhysicsObject::CreateCapsule(cWorld* pWorld, const math::cVec3& pos, const math::cVec3& rot)
    {
      geom = dCreateCapsule(bDynamic ? pWorld->GetSpaceDynamic() : pWorld->GetSpaceStatic(), fRadius, fLength);

      InitCommon(pWorld, pos, rot);

      if (bBody) {
        dMass mass;
        dMassSetCapsuleTotal(&mass, fWeightKg, uiDirectionX, 2.0f*fRadius, fLength);
        dBodySetMass(body, &mass);
      }
    }

    void cPhysicsObject::CreateCylinder(cWorld* pWorld, const math::cVec3& pos, const math::cVec3& rot)
    {
      geom = dCreateCylinder(bDynamic ? pWorld->GetSpaceDynamic() : pWorld->GetSpaceStatic(), fRadius, fLength);

      InitCommon(pWorld, pos, rot);

      if (bBody) {
        dMass mass;
        dMassSetCylinderTotal(&mass, fWeightKg, uiDirectionX, 2.0f * fRadius, fLength);
        dBodySetMass(body, &mass);
      }
    }

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
#endif


    void cPhysicsObject::AddForceRelativeToWorldKg(const physvec_t& forceKg)
    {
      dBodyAddForce(body, forceKg.x, forceKg.y, forceKg.z);
    }

    void cPhysicsObject::AddTorqueRelativeToWorldNm(const physvec_t& torqueNm)
    {
      dBodyAddTorque(body, torqueNm.x, torqueNm.y, torqueNm.z);
    }

    void cPhysicsObject::AddForceRelativeToObjectKg(const physvec_t& forceKg)
    {
      dBodyAddRelForce(body, forceKg.x, forceKg.y, forceKg.z);
    }

    void cPhysicsObject::AddTorqueRelativeToObjectNm(const physvec_t& torqueNm)
    {
      /*// Get our current rotation
      breathe::math::cQuaternion qOriginal;
      qOriginal.SetFromODEQuaternion(dBodyGetQuaternion(body));

      // Find our desired rotation (Current but with x=0, y=0)
      breathe::math::cQuaternion qDesired = qOriginal;
      qDesired.x = 0.0f;
      qDesired.y = 0.0f;
      float quat_len = sqrt(qDesired.z * qDesired.z + qDesired.w * qDesired.w);
      qDesired.z /= quat_len;
      qDesired.w /= quat_len;

      // Set our rotation to a rotation in between these two quaternions
      breathe::math::cQuaternion qInterpolated;
      qInterpolated.Slerp(qOriginal, qDesired, 0.1f);
      dReal values[4];
      qInterpolated.GetODEQuaternion(values);
      dBodySetQuaternion(body, values);

      // Get rid of our x,y angular velocity as well
      const dReal* rot = dBodyGetAngularVel(body);
      dBodySetAngularVel(body, 0.0f, 0.0f, rot[2]);*/


      dBodyAddRelTorque(body, torqueNm.x, torqueNm.y, torqueNm.z);
    }


    void cPhysicsObject::Update(sampletime_t currentTime)
    {
      if (bDynamic) {
        const dReal* p0 = nullptr;
        const dReal* r0 = nullptr;

        dQuaternion q;

        if (bBody) {
          p0 = dBodyGetPosition(body);
          r0 = dBodyGetQuaternion(body);
          const dReal* v0 = dBodyGetLinearVel(body);
          //const dReal *a0=dBodyGetAngularVel(body);

          v[0] = v0[0];
          v[1] = v0[1];
          v[2] = v0[2];
        } else {
          p0 = dGeomGetPosition(geom);
          dGeomGetQuaternion(geom, q);
          r0 = q;

          // These are static for the moment
          v[0] = 0.0f;
          v[1] = 0.0f;
          v[2] = 0.0f;
        }

        ASSERT(p0 != nullptr);
        ASSERT(r0 != nullptr);
        position.Set(p0[0], p0[1], p0[2]);
        rotation.SetFromODEQuaternion(r0);
      }
    }

#if 1
    // *****************************************************************************************************
    // Upright Capsule
    // *****************************************************************************************************
    cUprightCapsule::cUprightCapsule(cWorld* pWorld) :
      cPhysicsRayCast(pWorld)
    {
    }

    void cUprightCapsule::Update(sampletime_t currentTime)
    {
      ASSERT(HasBody());

      // Stop the capsule from rotating forwards, backwards, left, right
      //dBodySetAngularVel(GetBody(), 0.0f, 0.0f, 0.0f);
      //dBodySetTorque(GetBody(), 0.0f, 0.0f, 0.0f);

      cPhysicsObject::Update(currentTime);
    }
#endif
  }
}
