// Basic standard library includes
#include <cmath>

// STL
#include <iostream>
#include <list>
#include <vector>

// Anything else
#include <ode/ode.h>

// Breathe
#include <BREATHE/cBreathe.h>

#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cVar.h>
#include <BREATHE/UTIL/cConsole.h>

#include <BREATHE/MATH/cMath.h>
#include <BREATHE/MATH/cVec2.h>
#include <BREATHE/MATH/cVec3.h>
#include <BREATHE/MATH/cVec4.h>
#include <BREATHE/MATH/cMat4.h>
#include <BREATHE/MATH/cPlane.h>
#include <BREATHE/MATH/cQuaternion.h>
#include <BREATHE/MATH/cFrustum.h>
#include <BREATHE/MATH/cOctree.h>
#include <BREATHE/MATH/cColour.h>

#include <BREATHE/UTIL/cBase.h>
#include <BREATHE/MODEL/cMesh.h>
#include <BREATHE/MODEL/cModel.h>

#include <BREATHE/PHYSICS/cPhysicsObject.h>
#include <BREATHE/PHYSICS/cPhysics.h>
#include <BREATHE/PHYSICS/cContact.h>


namespace BREATHE
{
	namespace PHYSICS
	{
		cContact::cContact()
		{
			Clear();
		}

		void cContact::Clear()
		{
			bContact=false;

			std::memset(&contact, 0, sizeof(contact));
			contact.surface.mode = dContactApprox1;
		}

		bool cContact::Contact()
		{
			return bContact;
		}

		dContact cContact::GetContact()
		{
			return contact;
		}

		void cContact::SetGeom(dContactGeom o1, dGeomID o2)
		{
			bContact=true;

			contact.geom = o1;
			contact.geom.g1 = o2;
			contact.surface.mode |= dContactApprox1;			
		}

		void cContact::SetFDSSlipCoefficient1(float fSlip1)
		{
			contact.surface.mode |= dContactSlip1;
			contact.surface.slip1 = fSlip1;
		}

		void cContact::SetFDSSlipCoefficient2(float fSlip2)
		{
			contact.surface.mode |= dContactSlip2;
			contact.surface.slip2 = fSlip2;
		}

		void cContact::SetCoulombFriction(float fFriction)
		{
			contact.surface.mu = fFriction;
		}

		void cContact::SetFrictionDirection1(MATH::cVec3 v3Friction)
		{							
			contact.surface.mode |= dContactFDir1;
			contact.fdir1[0] = v3Friction.x;
			contact.fdir1[1] = v3Friction.y;
			contact.fdir1[2] = v3Friction.z;
		}

		void cContact::SetElasticity(float fERP, float fCFM)
		{
			contact.surface.mode |= dContactSoftERP;
			contact.surface.mode |= dContactSoftCFM;

			contact.surface.soft_erp = fERP;
			contact.surface.soft_cfm = fCFM;
		}

		void cContact::SetBounce(float fBounce, float fBounceVelocity)
		{
			contact.surface.mode |= dContactBounce;

			contact.surface.bounce = fBounce;
			contact.surface.bounce_vel = fBounceVelocity;
		}

		void cContact::CreateContact(float fDepth)
		{
			contact.geom.depth = fDepth;

			//dJointID j = dJointCreateContact(PHYSICS::world, PHYSICS::contactgroup, &contact);
			//dJointAttach(j, dGeomGetBody(contact.geom.g1), dGeomGetBody(contact.geom.g2));
		}
	}
}
