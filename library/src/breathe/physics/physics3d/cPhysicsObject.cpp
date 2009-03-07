#include <cmath>

#include <list>
#include <vector>

#include <sstream>
#include <iostream>
#include <fstream>

// Boost includes
#include <boost/shared_ptr.hpp>


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
      position.Set(0.0f, 0.0f, 0.0f);
      v.Set(0.0f, 0.0f, 0.0f);
      fWeight = 1.0f;
      fRadius = 2.0f;
      m.LoadIdentity();
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
      if (body) {
        dBodyDestroy(body);
        body = NULL;
      }
    }

    void cPhysicsObject::DestroyGeom()
    {
      if (geom) {
        dGeomDestroy(geom);
        geom = NULL;
      }
    }

    void cPhysicsObject::DestroyHeightfield()
    {
      if (heightfield) {
        dGeomHeightfieldDataDestroy(heightfield);
        heightfield = NULL;
      }

      SAFE_DELETE_ARRAY(pHeightfieldData);
    }

#ifdef BUILD_PHYSICS_3D
    void cPhysicsObject::InitCommon(const physvec_t& posOriginal, const physvec_t& rot)
    {
      math::cVec3 pos(posOriginal.x, posOriginal.y, posOriginal.z + fHeight);

      m.LoadIdentity();
      m.SetRotationX(rot.x * math::cPI_DIV_180);
      m.SetRotationY(rot.y * math::cPI_DIV_180);
      m.SetRotationZ(rot.z * math::cPI_DIV_180);

      dMatrix3 r;
      r[0] = m[0];    r[1] = m[4];    r[2] = m[8];    r[3] = 0;
      r[4] = m[1];    r[5] = m[5];    r[6] = m[9];    r[7] = 0;
      r[8] = m[2];    r[9] = m[6];    r[10] = m[10];  r[11] = 0;

      m.SetTranslation(pos);

      position = pos;

      dGeomSetPosition(geom, position.x, position.y, position.z);
      dGeomSetRotation(geom, r);

      if (bBody) {
        body = dBodyCreate(physics::GetWorld());
        dBodySetPosition(body, position.x, position.y, position.z);
        dBodySetRotation(body, r);
        dBodySetAutoDisableFlag(body, 1);

        dGeomSetBody(geom, body);
      }
    }

    void cPhysicsObject::CreateBox(const math::cVec3& pos, const math::cVec3& rot)
    {
      geom = dCreateBox(bDynamic ? physics::GetSpaceDynamic() : physics::GetSpaceStatic(),
        2.0f*fWidth, 2.0f*fLength, 2.0f*fHeight);

      InitCommon(pos, rot);

      if (bBody) {
        dMass mass;
        dMassSetBoxTotal(&mass, fWeight, 2.0f*fWidth, 2.0f*fLength, 2.0f*fHeight);
        dBodySetMass(body, &mass);
      }
    }

    void cPhysicsObject::CreateSphere(const math::cVec3& pos, const math::cVec3& rot)
    {
      geom = dCreateSphere(bDynamic ? physics::GetSpaceDynamic() : physics::GetSpaceStatic(), fRadius);

      InitCommon(pos, rot);

      if (bBody) {
        dMass mass;
        dMassSetSphereTotal(&mass, fWeight, 2.0f*fRadius);
        dBodySetMass(body, &mass);
      }
    }

    void cPhysicsObject::CreateCapsule(const math::cVec3& pos, const math::cVec3& rot)
    {
      geom = dCreateCapsule(bDynamic ? physics::GetSpaceDynamic() : physics::GetSpaceStatic(), fRadius, fLength);

      InitCommon(pos, rot);

      if (bBody) {
        dMass mass;
        dMassSetCapsuleTotal(&mass, fWeight, uiDirectionX, 2.0f*fRadius, fLength);
        dBodySetMass(body, &mass);
      }
    }

    void cPhysicsObject::CreateCylinder(const math::cVec3& pos, const math::cVec3& rot)
    {
      geom = dCreateCylinder(bDynamic ? physics::GetSpaceDynamic() : physics::GetSpaceStatic(), fRadius, fLength);

      InitCommon(pos, rot);

      if (bBody) {
        dMass mass;
        dMassSetCylinderTotal(&mass, fWeight, uiDirectionX, 2.0f * fRadius, fLength);
        dBodySetMass(body, &mass);
      }
    }

    void cPhysicsObject::CreateHeightmap(const game::cTerrainHeightMapLoader& loader, size_t width, size_t length, const physvec_t& scale, const physvec_t& pos, const physvec_t& rot)
    {
#if 1
      // NOTE: This doesn't seem to be working at the moment.
      ASSERT(false);

      // CreateTrimesh();

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
      geom = dCreateHeightfield(physics::GetSpaceStatic(), heightfield, movable);
      ASSERT(geom != 0);

      InitCommon(pos, rot);
#endif
    }

    void cPhysicsObject::CreateTrimesh(const std::vector<spitfire::math::cVec3>& coords, const std::vector<unsigned int>& indicies, const physvec_t& pos, const physvec_t& rot)
    {
      vVertices.clear();
      vIndices.clear();

      bBody = false;
      bDynamic = false;

      // Trimeshes are static for the moment
      v[0] = 0.0f;
      v[1] = 0.0f;
      v[2] = 0.0f;

      const float fPositionZ = 200.0f;

      const spitfire::math::cVec3 a(0.0f, 0.0f, fPositionZ);
      const spitfire::math::cVec3 b(200.0f, 200.0f, fPositionZ);

      const int VertexCount = 4;
      const int IndexCount = 6;

      vVertices.push_back(a.x);
      vVertices.push_back(a.y);
      vVertices.push_back(a.z);
      vVertices.push_back(0.0f); // Dummy because apparently dVector3 is 4 floats

      vVertices.push_back(b.x);
      vVertices.push_back(a.y);
      vVertices.push_back(a.z);
      vVertices.push_back(0.0f); // Dummy because apparently dVector3 is 4 floats

      vVertices.push_back(b.x);
      vVertices.push_back(b.y);
      vVertices.push_back(b.z);
      vVertices.push_back(0.0f); // Dummy because apparently dVector3 is 4 floats

      vVertices.push_back(a.x);
      vVertices.push_back(b.y);
      vVertices.push_back(a.z);
      vVertices.push_back(0.0f); // Dummy because apparently dVector3 is 4 floats


      vIndices.push_back(0);
      vIndices.push_back(1);
      vIndices.push_back(2);

      vIndices.push_back(2);
      vIndices.push_back(3);
      vIndices.push_back(0);


      dTriMeshDataID trimeshData = dGeomTriMeshDataCreate();

      dGeomTriMeshDataBuildSingle(
        trimeshData,
        (const void*)vVertices.data(), 4 * sizeof(dReal), (int)VertexCount, // Faces
        (const void*)vIndices.data(), (int)IndexCount, 3 * sizeof(uint32_t) // Indices
      );

      geom = dCreateTriMesh(physics::GetSpaceStatic(), trimeshData, NULL , NULL , NULL);

      InitCommon(pos, rot);
    }
#endif

    void cPhysicsObject::Update(sampletime_t currentTime)
    {
      if (bDynamic) {
        dReal* p0 = nullptr;
        dReal* r0 = nullptr;

        if (bBody) {
          p0 = const_cast<dReal*>(dBodyGetPosition(body));
          r0 = const_cast<dReal*>(dBodyGetRotation(body));
          const dReal* v0 = dBodyGetLinearVel(body);
          //const dReal *a0=dBodyGetAngularVel(body);

          v[0] = v0[0];
          v[1] = v0[1];
          v[2] = v0[2];
        } else {
          p0 = const_cast<dReal*>(dGeomGetPosition(geom));
          r0 = const_cast<dReal*>(dGeomGetRotation(geom));

          // These are static for the moment
          v[0] = 0.0f;
          v[1] = 0.0f;
          v[2] = 0.0f;
        }

        m[0]  = r0[0];
        m[1]  = r0[4];
        m[2]  = r0[8];
        m[3]  = 0;
        m[4]  = r0[1];
        m[5]  = r0[5];
        m[6]  = r0[9];
        m[7]  = 0;
        m[8]  = r0[2];
        m[9]  = r0[6];
        m[10] = r0[10];
        m[11] = 0;
        m[12] = p0[0];
        m[13] = p0[1];
        m[14] = p0[2];
        m[15] = 1;
      }

      UpdateComponents();
    }

    void cPhysicsObject::UpdateComponents()
    {
      position = m.GetPosition();
    }

#if 1
    // *****************************************************************************************************
    // Upright Capsule
    // *****************************************************************************************************
    cUprightCapsule::cUprightCapsule()
      : cPhysicsObject()
    {

    }

    void cUprightCapsule::Update(sampletime_t currentTime)
    {
      // Stop the capsule from rotating forwards, backwards, left, right
      dBodySetAngularVel(GetBody(), 0.0f, 0.0f, 0.0f);
      dBodySetTorque(GetBody(), 0.0f, 0.0f, 0.0f);

      cPhysicsObject::Update(currentTime);
    }
#endif
  }
}
