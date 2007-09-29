#ifndef CPHYSICS_H
#define CPHYSICS_H

namespace breathe
{
	namespace physics
	{
		extern const float fInterval;
		extern  const int iMaxContacts;
		extern const float fFriction;
		extern const float fBounce;
		extern const float fBounceVel;
		extern const float fERP;
		extern const float fCFM;
		extern const float fGravity;

		extern dWorldID world;
		extern dSpaceID spaceStatic;
		extern dSpaceID spaceDynamic;
		extern dJointGroupID contactgroup;
		extern dGeomID ground;

		class cPhysicsObject;

		
		void Init();
		void Destroy();
		void Update(float fCurrentTime);

		void CreateGround(float posX, float posY, float posZ, float nX, float nY, float nZ);

		void AddPhysicsObject(cPhysicsObject *pPhysicsObject);
		void RemovePhysicsObject(cPhysicsObject *pPhysicsObject);
		
		size_t GetSize();

		typedef std::list<cPhysicsObject * >::iterator iterator;
		iterator GetObjectListBegin();
		iterator GetObjectListEnd();
	}
}

#endif //CPHYSICS_H
