#include <cmath>

#include <iostream>
#include <list>
#include <vector>

#include <ode/ode.h>

#include <BREATHE/cMem.h>

#include <math.h>
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

#include <BREATHE/cLog.h>

#include <BREATHE/cBase.h>
#include <BREATHE/MODEL/cMesh.h>
#include <BREATHE/MODEL/cModel.h>

#include <BREATHE/PHYSICS/cPhysicsObject.h>
#include <BREATHE/PHYSICS/cPhysics.h>

namespace BREATHE
{
	namespace PHYSICS
	{
		dWorldID world=0;
		dSpaceID spaceStatic=0;
		dSpaceID spaceDynamic=0;
		dJointGroupID contactgroup=0;
		dGeomID ground=0;

		cPhysics::cPhysics()
		{

		}

		cPhysics::~cPhysics()
		{

		}

		void cPhysics::CreateGround(float posX, float posY, float posZ, float nX, float nY, float nZ)
		{
			BREATHE::MATH::cVec3 p(posX, posY, posZ);
			BREATHE::MATH::cVec3 n(nX, nY, nZ);

			n.Normalize();

			ground = dCreatePlane(spaceStatic, n.x, n.y, n.z, n.DotProduct(p));
		}

		void cPhysics::Init()
		{
			BREATHE::PHYSICS::world = dWorldCreate();

			dWorldSetGravity(world, 0, 0, m_gravity);
			
			dWorldSetCFM(world, (float)(10e-5));
			dWorldSetERP(world, 0.8f);

			dWorldSetContactMaxCorrectingVel(world, 1.0f);
			// This function sets the depth of the surface layer around the world objects. Contacts are allowed to sink into
      // each other up to this depth. Setting it to a small value reduces the amount of jittering between contacting
      // objects, the default value is 0. 	
			dWorldSetContactSurfaceLayer(world, 0.001f);

			dWorldSetAutoDisableFlag(world, 1);

			spaceStatic=dHashSpaceCreate(0);
			spaceDynamic=dHashSpaceCreate(0);

			contactgroup = dJointGroupCreate(10000);
		}

		void cPhysics::AddPhysicsObject(cPhysicsObject *pPhysicsObject)
		{
			lPhysicsObject.push_back(pPhysicsObject);
		}
		
		void cPhysics::RemovePhysicsObject(cPhysicsObject *pPhysicsObject)
		{
			lPhysicsObject.remove(pPhysicsObject);

			dGeomDestroy(pPhysicsObject->geom);

			if(pPhysicsObject->bBody)
        dBodyDestroy(pPhysicsObject->body);
		}

		void cPhysics::Update(float fCurrentTime)
		{
			float interval=0.016f;

			//First Iteration
			//  apply one-size-fits-all rotational and linear dampening

			std::list<cPhysicsObject *>::iterator iter=lPhysicsObject.begin();

			while(lPhysicsObject.end() != iter)
			{
				if((*iter)->bBody)
				{
          dBodyID b = (*iter)->body;

					if(b)
					{
						dReal const * av = dBodyGetAngularVel( b );
						dBodyAddTorque( b, -av[0]*0.05f, -av[1]*0.05f, -av[2]*0.05f );
						dReal const * lv = dBodyGetLinearVel( b );
						dBodyAddForce( b, -lv[0]*0.02f, -lv[1]*0.02f, -lv[2]*0.02f );
					}
				}

				iter++;
			};

			dSpaceCollide(spaceDynamic, this, cPhysics::nearCallbackDynamic);
			dSpaceCollide2((dGeomID)spaceDynamic, (dGeomID)spaceStatic, this, &cPhysics::nearCallbackStatic);
			//dSpaceCollide2((dGeomID)spaceDynamic, (dGeomID)spaceTrigger, this, &cPhysics::nearCallbackTrigger);

			//PStepper::stepAll( dt );

			dWorldQuickStep(world, interval);
			dJointGroupEmpty(contactgroup);
		}

		void cPhysics::nearCallbackStatic(void *f, dGeomID o1, dGeomID o2)
		{
			//Ignore collisions between NULL geometry
			if(!(o1 && o2))
			{
				pLog->Error("nearCallbackStatic", "NULL geometry");
				return;
			}

			//Ignore collisions between bodies that are connected by the same joint
			dBodyID Body1 = NULL, Body2 = NULL;

			if(o1) 
				Body1 = dGeomGetBody (o1);
			if(o2) 
				Body2 = dGeomGetBody (o2);

			if (Body1 && Body2 && dAreConnected (Body1, Body2))
				return;

			int i, n;
			const int N = ODE_MAX_CONTACTS;
			dContact contact[N];

			n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
			if (n > 0)
			{
				for (i=0; i<n; i++)
				{
					contact[i].surface.mode = dContactBounce;
					contact[i].surface.mu = 2000; //5000;
					contact[i].surface.bounce = 0.3f;
					contact[i].surface.bounce_vel = 0.2f;

					dJointID c = dJointCreateContact(world, contactgroup, &contact[i]);
					dJointAttach(c, Body1, Body2);
				}
			}
		}

		void cPhysics::nearCallbackDynamic(void *f, dGeomID o1, dGeomID o2)
		{
			//Ignore collisions between NULL geometry
			if(!(o1 && o2))
			{
				pLog->Error("nearCallbackDynamic", "NULL geometry");
				return;
			}

			//Ignore collisions between bodies that are connected by the same joint
			dBodyID Body1 = NULL, Body2 = NULL;

			if(o1) 
				Body1 = dGeomGetBody (o1);
			if(o2) 
				Body2 = dGeomGetBody (o2);

			if (Body1 && Body2 && dAreConnected (Body1, Body2))
				return;

			int i, n;
			const int N = ODE_MAX_CONTACTS;
			dContact contact[N];

			n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
			if (n > 0)
			{
				for (i=0; i<n; i++)
				{
					contact[i].surface.mode = dContactBounce;
					contact[i].surface.mu = 2000; //5000;
					contact[i].surface.bounce = 0.3f;
					contact[i].surface.bounce_vel = 0.2f;

					dJointID c = dJointCreateContact(world, contactgroup, &contact[i]);
					dJointAttach(c, Body1, Body2);
				}
			}
		}
	}
}
