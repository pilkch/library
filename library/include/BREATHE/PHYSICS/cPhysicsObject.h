#ifndef CPHYSICSOBJECT_H
#define CPHYSICSOBJECT_H

namespace BREATHE
{
	namespace PHYSICS
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

			RENDER::MODEL::cModel *pModel;


			cPhysicsObject();
			~cPhysicsObject();
			
			void SetTrimeshSource(std::vector<float>& coords, std::vector<unsigned int>& indicies);
			void SetHeightmapSource(std::vector<float>& heightvalues, unsigned int uiWidth, unsigned int uiHeight);

			void InitCommon(MATH::cVec3& pos, MATH::cVec3& rot);

			void CreateBox(MATH::cVec3 pos, MATH::cVec3 rot=MATH::cVec3(0.0f, 0.0f, 0.0f));
			void CreateSphere(MATH::cVec3 pos, MATH::cVec3 rot=MATH::cVec3(0.0f, 0.0f, 0.0f));
			void CreateCapsule(MATH::cVec3 pos, MATH::cVec3 rot=MATH::cVec3(0.0f, 0.0f, 0.0f));
			void CreateCylinder(MATH::cVec3 pos, MATH::cVec3 rot=MATH::cVec3(0.0f, 0.0f, 0.0f));
			void CreateTrimesh(MATH::cVec3 pos, MATH::cVec3 rot=MATH::cVec3(0.0f, 0.0f, 0.0f));
			void CreateHeightmap(MATH::cVec3 pos, MATH::cVec3 rot=MATH::cVec3(0.0f, 0.0f, 0.0f));

			virtual void Update(float fCurrentTime);
			virtual void UpdateComponents();

			void RemoveFromWorld();

			//void PhysicsInit(MATH::cVec3 pos, MATH::cVec3 rot=MATH::cVec3());
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
