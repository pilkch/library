#ifndef CPHYSICSOBJECT_H
#define CPHYSICSOBJECT_H

#ifdef BUILD_PHYSICS_3D
#include <breathe/game/cHeightmapLoader.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>
#endif

namespace breathe
{
  namespace physics
  {
    /*enum object_type
    {
      object_type_box = 0,
      object_type_sphere,
      object_type_capsule,
      object_type_cylinder,
      object_type_heightmap,
#ifdef BUILD_PHYSICS_3D
      object_type_trimesh,
#endif
    };*/

    /*class cPhysicsObject : virtual public cObject, public boost::enable_shared_from_this<cPhysicsObject>
    {
    public:
      cPhysicsObject();
      virtual ~cPhysicsObject();

      virtual void Update(sampletime_t currentTime);

      void CreateBox(cWorld* pWorld, const physvec_t& pos, const physvec_t& rot = physveczero);
      void CreateSphere(cWorld* pWorld, const physvec_t& pos, const physvec_t& rot = physveczero);
      void CreateCapsule(cWorld* pWorld, const physvec_t& pos, const physvec_t& rot = physveczero);
      void CreateCylinder(cWorld* pWorld, const physvec_t& pos, const physvec_t& rot = physveczero);
#ifdef BUILD_PHYSICS_2D
      void CreateHeightmap(cWorld* pWorld, const std::vector<float>& heightvalues, const physvec_t& scale, const physvec_t& pos);
      void CreateCombinedShapes(cWorld* pWorld, std::list<b2ShapeDef*> lShapes, const physvec_t& pos, const physvec_t& rot = physveczero);
#else
      void CreateHeightmap(cWorld* pWorld, const game::cTerrainHeightMap& loader, size_t width, size_t height, const physvec_t& scale, const physvec_t& pos, const physvec_t& rot = physveczero);
      void CreateTrimesh(cWorld* pWorld, const std::vector<spitfire::math::cVec3>& coords, const std::vector<unsigned int>& indices, const physvec_t& pos, const physvec_t& rot = physveczero);
#endif

      void RemoveFromWorld();

      void SetDynamic(bool bIsDynamic) { bDynamic = bIsDynamic; }
      void SetUseBody(bool bUseBody) { bBody = bUseBody; }

      bool HasBody() const { return bBody; }
      bool IsDynamic() const { return bDynamic; }

      physvec_t GetPositionAbsolute() const { return position; }
      physrotation_t GetRotationAbsolute() const { return rotation; }

#ifdef BUILD_PHYSICS_2D
      void SetFriction(float friction) { assert(body == nullptr); fFriction = friction; }
      b2Body* GetBody() const { return body; }
#else
      dBodyID GetBody() const { return body; }
      dGeomID GetGeom() const { return geom; }
      void DestroyBody();
      void DestroyGeom();

      void DestroyHeightfield();

      //object_type GetType() const { return type; }
#endif

      float_t GetWeightKg() const { return fWeightKg; }


      void AddForceRelativeToWorldKg(const physvec_t& forceKg);
      void AddTorqueRelativeToWorldNm(const physvec_t& torqueNm);

      void AddForceRelativeToObjectKg(const physvec_t& forceKg);
      void AddTorqueRelativeToObjectNm(const physvec_t& torqueNm);


      physvec_t GetClosestPointOnObject(const physvec_t& point) const;

    private:
#ifdef BUILD_PHYSICS_2D
      void InitCommon(cWorld* pWorld, const std::list<b2ShapeDef*>& lShapes, const physvec_t& pos, const physvec_t& rot);
#endif

#ifdef BUILD_PHYSICS_3D
      void InitCommon(cWorld* pWorld, const physvec_t& pos, const physvec_t& rot);
#endif

      //object_type type;

      bool bBody;
      bool bDynamic;

#ifdef BUILD_PHYSICS_2D
      float fFriction;
      b2Body* body;
#else
      dBodyID body;
      dGeomID geom;

      dHeightfieldDataID heightfield;
      float* pHeightfieldData;

      std::vector<spitfire::math::cVec3> vVertices;
      std::vector<uint32_t> vIndices;
#endif
    };

    typedef cSmartPtr<cPhysicsObject> cPhysicsObjectRef;*/
  }
}

#ifdef BUILD_PHYSICS_3D
#include <breathe/physics/physics3d/cPhysicsObject.h>
#endif

#endif // CPHYSICSOBJECT_H
