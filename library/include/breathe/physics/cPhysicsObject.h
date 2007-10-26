#ifndef CPHYSICSOBJECT_H
#define CPHYSICSOBJECT_H

namespace breathe
{
	namespace physics
	{
		class cPhysics;
		
		class cPhysicsObject : virtual public cObject
		{
		private:
			std::vector<float> vCoords;
			std::vector<unsigned int> vIndicies;

		public:
			bool bBody;
			bool bDynamic;

			dBodyID body;
			dGeomID geom;

			render::model::cModel *pModel;


			cPhysicsObject();
			virtual ~cPhysicsObject();
			
			void SetTrimeshSource(std::vector<float>& coords, std::vector<unsigned int>& indicies);
			void SetHeightmapSource(std::vector<float>& heightvalues, unsigned int uiWidth, unsigned int uiHeight);

			void InitCommon(math::cVec3& pos, math::cVec3& rot);

			void CreateBox(math::cVec3 pos, math::cVec3 rot=math::cVec3(0.0f, 0.0f, 0.0f));
			void CreateSphere(math::cVec3 pos, math::cVec3 rot=math::cVec3(0.0f, 0.0f, 0.0f));
			void CreateCapsule(math::cVec3 pos, math::cVec3 rot=math::cVec3(0.0f, 0.0f, 0.0f));
			void CreateCylinder(math::cVec3 pos, math::cVec3 rot=math::cVec3(0.0f, 0.0f, 0.0f));
			void CreateTrimesh(math::cVec3 pos, math::cVec3 rot=math::cVec3(0.0f, 0.0f, 0.0f));
			void CreateHeightmap(math::cVec3 pos, math::cVec3 rot=math::cVec3(0.0f, 0.0f, 0.0f));

			virtual void Update(float fCurrentTime);
			virtual void UpdateComponents();

			void RemoveFromWorld();

			//void PhysicsInit(math::cVec3 pos, math::cVec3 rot=math::cVec3());
			//void PhysicsDestroy();
		};

		// *****************************************************************************************************
		// Upright Capsule, use this for characters that cannot fall over
		// Basically the same as a normal capsule, just with constraints that: 
		// a) Stop the capsule tipping over
		// b) Help the capsule up stairs by pushing the capsule up when we collide a ray in front of our feet
		// *****************************************************************************************************
		class cUprightCapsule : virtual public cPhysicsObject, public cPhysicsRayCast
		{
		public:
			cUprightCapsule();

			void Update(float fCurrentTime);
		};
	}
}

#endif //CPHYSICSOBJECT_H
