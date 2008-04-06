#ifndef CPHYSICSOBJECT_H
#define CPHYSICSOBJECT_H

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

		class cPhysicsObject : virtual public cObject
		{
		public:
			cPhysicsObject();
			virtual ~cPhysicsObject();

			virtual void Update(sampletime_t currentTime);

			void CreateBox(const physvec_t& pos, const physvec_t& rot = physveczero);
			void CreateSphere(const physvec_t& pos, const physvec_t& rot = physveczero);
			void CreateCapsule(const physvec_t& pos, const physvec_t& rot = physveczero);
			void CreateCylinder(const physvec_t& pos, const physvec_t& rot = physveczero);
#ifdef BUILD_PHYSICS_2D
			void CreateHeightmap(const std::vector<float>& heightvalues, const physvec_t& scale, const physvec_t& pos);
			void CreateCombinedShapes(std::list<b2ShapeDef*> lShapes, const physvec_t& pos, const physvec_t& rot = physveczero);
#else
			void CreateHeightmap(const std::vector<float>& heightvalues, size_t width, size_t height, const physvec_t& scale, const physvec_t& pos, const physvec_t& rot = physveczero);
      void CreateTrimesh(const std::vector<float>& coords, const std::vector<unsigned int>& indicies, const physvec_t& pos, const physvec_t& rot = physveczero);
#endif

			void RemoveFromWorld();
      
      void SetDynamic(bool bIsDynamic) { bDynamic = bIsDynamic; }
      void SetUseBody(bool bUseBody) { bBody = bUseBody; }

      bool HasBody() const { return bBody; }
      bool IsDynamic() const { return bDynamic; }

#ifdef BUILD_PHYSICS_2D
      void SetFriction(float friction) { assert(body == nullptr); fFriction = friction; }
      b2Body* GetBody() const { return body; }
#else
      dBodyID GetBody() const { return body; }
      dGeomID GetGeom() const { return geom; }

      //object_type GetType() const { return type; }

			render::model::cModel *pModel;
#endif

		private:
      void InitCommon(std::list<b2ShapeDef*> lShapes, const physvec_t& pos, const physvec_t& rot);

#ifdef BUILD_PHYSICS_3D
			virtual void UpdateComponents();
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

			std::vector<float> vCoords;
			std::vector<unsigned int> vIndicies;
#endif
    };
	}
}

#ifdef BUILD_PHYSICS_3D
#include <breathe/physics/physics3d/cPhysicsObject.h>
#endif

#endif //CPHYSICSOBJECT_H
