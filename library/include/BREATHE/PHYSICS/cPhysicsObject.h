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
			//cPhysics *pPhysics;


			bool bBody;
			bool bDynamic;

			dBodyID body;
			dGeomID geom;

			BREATHE::MODEL::cModel *pModel;


			cPhysicsObject();
			~cPhysicsObject();
			
			void SetTrimeshSource(std::vector<float> &coords, std::vector<unsigned int> &indicies);

			void CreateBox(MATH::cVec3 pos, MATH::cVec3 rot=MATH::cVec3());
			void CreateSphere(MATH::cVec3 pos, MATH::cVec3 rot=MATH::cVec3());
			void CreateTrimesh(MATH::cVec3 pos, MATH::cVec3 rot=MATH::cVec3());

			void Update(float fTime);
			void UpdateComponents();


			
			//pPhysics->AddPhysicsObject(this);
			//pPhysics->RemovePhysicsObject(this);

			//void PhysicsInit(MATH::cVec3 pos, MATH::cVec3 rot=MATH::cVec3());
			//void PhysicsDestroy();
		};
	}
}

#endif //CPHYSICSOBJECT_H
