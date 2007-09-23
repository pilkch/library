// Basic standard library includes
#include <cmath>

// STL
#include <list>
#include <vector>

// writing on a text file
#include <iostream>
#include <fstream>
#include <sstream>

// Anything else
#include <ode/ode.h>

// Breathe
#include <BREATHE/cBreathe.h>

#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cVar.h>

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
#include <BREATHE/RENDER/MODEL/cMesh.h>
#include <BREATHE/RENDER/MODEL/cModel.h>

#include <BREATHE/PHYSICS/cPhysics.h>
#include <BREATHE/PHYSICS/cContact.h>
#include <BREATHE/PHYSICS/cRayCast.h>
#include <BREATHE/PHYSICS/cPhysicsObject.h>

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
			bContact = false;
			fDepth = MATH::cINFINITY;

			std::memset(&contact, 0, sizeof(contact));
			contact.surface.mode = dContactApprox1;
		}

		dContact cContact::GetContact()
		{
			return contact;
		}

		void cContact::SetContact(dContactGeom o1, dGeomID o2, float fContact)
		{
			bContact = true;
			fDepth = fContact;

			contact.geom = o1;
			contact.geom.g1 = o2;
			contact.surface.mode = dContactApprox1;			
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
			// JITTER
			// What is your CFM?  If it's too stiff, you can get jitter.  Try using a
			// value 10x or 100x greater than what you're using now.  Stuff I've done in
			// my app usually uses a CFM around 0.001... 0.01 gets spongy, 0.00001 is
			// usually jittery.  I have always left ERP at 1.0, but you could also try reducing 
			// that to combat jitter.

			contact.surface.mode |= dContactSoftERP;
			contact.surface.mode |= dContactSoftCFM;

			contact.surface.soft_erp = fERP;
			contact.surface.soft_cfm = fCFM;
		}

		void cContact::SetSuspensionKU(float fSuspensionK, float fSuspensionU)
		{
			// By adjusting the values of ERP and CFM, you can achieve various effects. 
			// For example you can simulate springy constraints, where the two bodies 
			// oscillate as though connected by springs. Or you can simulate more spongy constraints, 
			// without the oscillation. In fact, ERP and CFM can be selected to have the same effect 
			// as any desired spring and damper constants. If you have a spring constant kp and 
			// damping constant kd, then the corresponding ODE constants are:
			// ERP = h k_p / (h k_p + k_d)
			// CFM = 1 / (h k_p + k_d)
			// where h is the stepsize. These values will give the same effect as a spring-and-damper 
			// system simulated with implicit first order integration.

			float fSuspensionStep = PHYSICS::fInterval * 1000.0f;
			SetElasticity((fSuspensionStep * fSuspensionK) / ((fSuspensionStep * fSuspensionK) + fSuspensionU), 
										1.0f / ((fSuspensionStep * fSuspensionK) + fSuspensionU));
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

			dJointID j = dJointCreateContact(PHYSICS::world, PHYSICS::contactgroup, &contact);
			dJointAttach(j, dGeomGetBody(contact.geom.g1), dGeomGetBody(contact.geom.g2));
		}
	}
}
