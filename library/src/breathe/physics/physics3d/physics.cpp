#include <cmath>

// writing on a text file
#include <iostream>
#include <fstream>
#include <sstream>

#include <list>
#include <vector>

#include <ode/ode.h>


#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cOctree.h>
#include <breathe/math/cColour.h>

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>

#include <breathe/physics/physics.h>

namespace breathe
{
	namespace physics
	{
    // *** Timing
    const unsigned int uiFrequencyHz = 100;
		const float fInterval = 1000.0f / uiFrequencyHz;

    unsigned int GetFrequencyHz()
    {
      return uiFrequencyHz;
    }

    float GetInterval()
    {
      return fInterval;
    }

		// *** Physics data
		const int iMaxContacts = 100;
		const float fFriction = 2000.0f;
		const float fBounce = 0.003f;
		const float fBounceVel = 0.00002f;
		const float fERP = 0.8f;
		const float fCFM = (float)(10e-5);
		const float fGravity = -9.8f;
		const float fDampTorque = 0.05f;
		const float fDampLinearVel = 0.001f;

		dWorldID world = 0;
		dSpaceID spaceStatic = 0;
		dSpaceID spaceDynamic = 0;
		dJointGroupID contactgroup = 0;
		dGeomID ground = 0;

		std::list<cPhysicsObject * >lPhysicsObject;

		
		
		// *** Functions

		size_t size() { return lPhysicsObject.size(); }
		iterator begin() { return lPhysicsObject.begin(); }
		iterator end() { return lPhysicsObject.end(); }

		// This wrapper is called by the physics library to get information
		// about object collisions
		void nearCallbackStatic(void *pData, dGeomID o1, dGeomID o2);
		void nearCallbackDynamic(void *pData, dGeomID o1, dGeomID o2);

		// This actually implements the collision callback
		void nearCallbackStatic(dGeomID o1, dGeomID o2);
		void nearCallbackDynamic(dGeomID o1, dGeomID o2);


		void CreateGround(float posX, float posY, float posZ, float nX, float nY, float nZ)
		{
			breathe::math::cVec3 p(posX, posY, posZ);
			breathe::math::cVec3 n(nX, nY, nZ);

			n.Normalize();

			ground = dCreatePlane(spaceStatic, n.x, n.y, n.z, n.DotProduct(p));
		}

		void Init()
		{
			breathe::physics::world = dWorldCreate();

			dWorldSetGravity(world, 0, 0, fGravity);

			// This function sets the depth of the surface layer around the world objects. Contacts are allowed to sink into
      // each other up to this depth. Setting it to a small value reduces the amount of jittering between contacting
      // objects, the default value is 0. 	
			dWorldSetContactSurfaceLayer(world, 0.001f);

			dWorldSetERP(world, fERP);
			dWorldSetCFM(world, fCFM);

			dWorldSetContactMaxCorrectingVel(world, 1.0f);
			dWorldSetAutoDisableFlag(world, 1);

			spaceStatic=dHashSpaceCreate(0);
			spaceDynamic=dHashSpaceCreate(0);

			contactgroup = dJointGroupCreate(10000);
		}

		void Destroy()
		{
			
		}

		void AddPhysicsObject(cPhysicsObject *pPhysicsObject)
		{
			lPhysicsObject.push_back(pPhysicsObject);
		}
		
		void RemovePhysicsObject(cPhysicsObject *pPhysicsObject)
		{
			lPhysicsObject.remove(pPhysicsObject);
		}

		void Update(sampletime_t currentTime)
		{
			//First Iteration
			//  apply one-size-fits-all rotational and linear dampening

			iterator iter = lPhysicsObject.begin();
			iterator iterEnd = lPhysicsObject.end();

			dBodyID b;
			while(iterEnd != iter)
			{
				b = (*iter++)->body;

				if (b)
				{
          dReal const * av = dBodyGetAngularVel( b );
					dBodySetAngularVel( b, av[0] - av[0]*fDampTorque, av[1] - av[1]*fDampTorque, av[2] - av[2]*fDampTorque );
					dReal const * lv = dBodyGetLinearVel( b );
					dBodySetLinearVel( b, lv[0] - lv[0]*fDampLinearVel, lv[1] - lv[1]*fDampLinearVel, lv[2] - lv[2]*fDampLinearVel );
				}
			};

			// Was using an object with this code
			// dSpaceCollide(spaceDynamic, this, nearCallbackDynamic);
			// dSpaceCollide2((dGeomID)spaceDynamic, (dGeomID)spaceStatic, this, &nearCallbackStatic);
			// dSpaceCollide2((dGeomID)spaceDynamic, (dGeomID)spaceTrigger, this, &nearCallbackTrigger);

			dSpaceCollide(spaceDynamic, NULL, nearCallbackDynamic);
			dSpaceCollide2((dGeomID)spaceDynamic, (dGeomID)spaceStatic, NULL, &nearCallbackStatic);
			//dSpaceCollide2((dGeomID)spaceDynamic, (dGeomID)spaceTrigger, NULL, &nearCallbackTrigger);

			dWorldQuickStep(world, GetInterval() / 1000.0f);
			dJointGroupEmpty(contactgroup);
		}

		void nearCallbackStatic(void *f, dGeomID o1, dGeomID o2)
		{
			//Ignore collisions between NULL geometry
			if (!(o1 && o2))
			{
				LOG.Error("nearCallbackStatic", "NULL geometry");
				return;
			}

			//Ignore collisions between bodies that are connected by the same joint
			dBodyID Body1 = NULL, Body2 = NULL;

			if (o1) 
				Body1 = dGeomGetBody (o1);
			if (o2) 
				Body2 = dGeomGetBody (o2);

			if (Body1 && Body2 && dAreConnected (Body1, Body2))
				return;

			int i, n;
			const int N = iMaxContacts;
			dContact contact[N];

			n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
			if (n > 0)
			{
				for (i=0; i<n; i++)
				{
					contact[i].surface.mode = dContactBounce;
					contact[i].surface.mu = fFriction;
					contact[i].surface.bounce = fBounce;
					contact[i].surface.bounce_vel = fBounceVel;

					dJointID c = dJointCreateContact(world, contactgroup, &contact[i]);
					dJointAttach(c, Body1, Body2);
				}
			}
		}

		void nearCallbackDynamic(void *f, dGeomID o1, dGeomID o2)
		{
			//Ignore collisions between NULL geometry
			if (!(o1 && o2))
			{
				LOG.Error("nearCallbackDynamic", "NULL geometry");
				return;
			}

			//Ignore collisions between bodies that are connected by the same joint
			dBodyID Body1 = NULL, Body2 = NULL;

			if (o1) 
				Body1 = dGeomGetBody (o1);
			if (o2) 
				Body2 = dGeomGetBody (o2);

			if (Body1 && Body2 && dAreConnected (Body1, Body2))
				return;

			int i, n;
			const int N = iMaxContacts;
			dContact contact[N];

			n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
			if (n > 0)
			{
				for (i=0; i<n; i++)
				{
					contact[i].surface.mode = dContactBounce;
					contact[i].surface.mu = fFriction;
					contact[i].surface.bounce = fBounce;
					contact[i].surface.bounce_vel = fBounceVel;

					dJointID c = dJointCreateContact(world, contactgroup, &contact[i]);
					dJointAttach(c, Body1, Body2);
				}
			}
		}
	}
}
