#ifndef CPHYSICS_H
#define CPHYSICS_H

namespace breathe
{
	namespace physics
	{
    unsigned int GetFrequencyHz();
    float GetInterval();

		extern const int iMaxContacts;
		extern const float fFriction;
		extern const float fBounce;
		extern const float fBounceVel;
		extern const float fERP;
		extern const float fCFM;
		extern const float fGravity;

		class cPhysicsObject;

		
		void Init();
		void Destroy();
		void Update(sampletime_t currentTime);

		void AddPhysicsObject(cPhysicsObject *pPhysicsObject);
		void RemovePhysicsObject(cPhysicsObject *pPhysicsObject);
		
		size_t size();
		bool empty();

		typedef std::list<cPhysicsObject * >::iterator iterator;
		iterator begin();
		iterator end();
	}
}

#ifdef BUILD_PHYSICS_2D
#include <breathe/physics/physics2d/physics.h>
#include <breathe/physics/physics2d/cContact.h>
#include <breathe/physics/physics2d/cRayCast.h>
#include <breathe/physics/physics2d/cPhysicsObject.h>
#elif defined(BUILD_PHYSICS_3D)
#include <breathe/physics/physics3d/physics.h>
#include <breathe/physics/physics3d/cContact.h>
#include <breathe/physics/physics3d/cRayCast.h>
#include <breathe/physics/physics3d/cPhysicsObject.h>
#endif

#endif //CPHYSICS_H
