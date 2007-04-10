#ifndef CPHYSICS_H
#define CPHYSICS_H

namespace BREATHE
{
	namespace PHYSICS
	{
		#define ODE_MAX_CONTACTS 100
		#define uIterations 5
		#define m_rFriction 2000
		#define m_rBounce 0.3
		#define m_rBounceVel 0.2
		#define m_gravity -9.8f
		
		extern dWorldID world;
		extern dSpaceID spaceStatic;
		extern dSpaceID spaceDynamic;
		extern dJointGroupID contactgroup;
		extern dGeomID ground;


		//class cPhysicsObject;

		class cPhysics : public cUpdateable
		{
		public:
			std::list<cPhysicsObject * >lPhysicsObject;

			float fInterval;

			cPhysics();
			~cPhysics();

			void Init();
			void Update(float fCurrentTime);

			void CreateGround(float posX, float posY, float posZ, float nX, float nY, float nZ);

			void AddPhysicsObject(cPhysicsObject *pPhysicsObject);
			void RemovePhysicsObject(cPhysicsObject *pPhysicsObject);

		private:

			// This wrapper is called by the physics library to get information
			// about object collisions
			static void nearCallbackStatic(void *pData, dGeomID o1, dGeomID o2);
			static void nearCallbackDynamic(void *pData, dGeomID o1, dGeomID o2);

			// This actually implements the collision callback
			void nearCallbackStatic(dGeomID o1, dGeomID o2);
			void nearCallbackDynamic(dGeomID o1, dGeomID o2);
		};
	}
}

#endif //CPHYSICS_H
