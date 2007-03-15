#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <list>

#include <ODE/ode.h>

#include "BREATHE/cMem.h"

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

#include <BREATHE/cBase.h>
#include <BREATHE/MODEL/cMesh.h>
#include <BREATHE/MODEL/cModel.h>
#include <BREATHE/MODEL/cStatic.h>

#include <BREATHE/RENDER/cCamera.h>
#include <BREATHE/cLevel.h>

#include <BREATHE/PHYSICS/cPhysicsObject.h>
#include <BREATHE/PHYSICS/cPhysics.h>

#include <BREATHE/cPlayer.h>
#include <BREATHE/cPetrolBowser.h>

#include <BREATHE/VEHICLE/cPart.h>
#include <BREATHE/VEHICLE/cWheel.h>
#include <BREATHE/VEHICLE/cSeat.h>
#include <BREATHE/VEHICLE/cVehicle.h>

#include <BREATHE/cBase.h>
#include <BREATHE/MODEL/cMesh.h>
#include <BREATHE/MODEL/cModel.h>
#include <BREATHE/MODEL/cStatic.h>

#if (_MSC_VER >= 1300) && (WINVER < 0x0500)
#pragma warning(disable:4305)
#pragma warning(disable:4244)
#endif

/*



		suspensionLength[i] = 1.0f - depth;

		if (depth == 0.0f)
		{
			i++;
			continue;
		}
		//if (depth > 0.7f)
		//	depth = 0.7f;
		
		neV3 groundNormal = sn->GetDetectNormal();

		neV3 lineNormal = body2World.rot * sn->GetLineUnitVector();

		neV3 linePos = body2World * sn->GetLinePos();

		f32 dot = lineNormal.Dot(groundNormal) * -1.0f;

		//if (dot <= 0.7f)
		//	continue;

		neV3 f = depth * lineNormal * -k ;//* dot;

		force += f;

		neV3 r = linePos - carRigidBody->GetPos();

		torque += r.Cross(f);

		// add damping force
		
		f32 speed = carRigidBody->GetVelocityAtPoint(r).Dot(lineNormal);

		f = -speed * lineNormal * u;

		force += f;

		torque += r.Cross(f);

		// add friction force

		neV3 vel = carRigidBody->GetVelocityAtPoint(sn->GetDetectContactPoint() - carRigidBody->GetPos());

		vel.RemoveComponent(groundNormal);

		if (i == 0 || i == 1)
		{
			//steering

			vel.RemoveComponent(steerDir);
		}
		else
		{
			vel.RemoveComponent(body2World.rot[0]); //rear wheel always parallel to car body
		}


		f = vel;

		f.Normalize();

		f *= -2.0f;

		if (i == 2 || i ==3)
		{
			f *= (1.0f - slide);
		}
		f[1] = 0.0f;
		
		force += f;

		r = sn->GetDetectContactPoint() - carRigidBody->GetPos();

		torque += r.Cross(f);

		// driving force

		if (i == 2 || i == 3) // rear wheel
		{
			f = body2World.rot[0];

			f *= accel;

			force += f;

			torque += r.Cross(f);
		}




        dBodyGetRelPointPos( car->odeBody,
                             car->pCarInfo->suspension.pWheels[i].springChassisAttachment[0],
                             car->pCarInfo->suspension.pWheels[i].springChassisAttachment[1],
                             car->pCarInfo->suspension.pWheels[i].springChassisAttachment[2],
                             pos1 );
        dBodyGetRelPointVel( car->odeBody,
                             car->pCarInfo->suspension.pWheels[i].springChassisAttachment[0],
                             car->pCarInfo->suspension.pWheels[i].springChassisAttachment[1],
                             car->pCarInfo->suspension.pWheels[i].springChassisAttachment[2],
                             vel1 );
        dBodyGetRelPointPos( car->odeUprights[i],
                             car->pCarInfo->suspension.pWheels[i].springUprightAttachment[0],
                             car->pCarInfo->suspension.pWheels[i].springUprightAttachment[1],
                             car->pCarInfo->suspension.pWheels[i].springUprightAttachment[2],
                             pos2 );
        dBodyGetRelPointVel( car->odeUprights[i],
                             car->pCarInfo->suspension.pWheels[i].springUprightAttachment[0],
                             car->pCarInfo->suspension.pWheels[i].springUprightAttachment[1],
                             car->pCarInfo->suspension.pWheels[i].springUprightAttachment[2],
                             vel2 );

        vecSubtract( pos2, pos1, posDiff );
        springLen = vecNormalize( posDiff );

        vecSubtract( vel2, vel1, velDiff );
        springVel = dotProduct( velDiff, posDiff );

        if( i < 2 )
        {
            if( springVel < 0.0f )
                vecScale( posDiff, -springVel * car->pFShock->bumpRate, force );
            else
                vecScale( posDiff, -springVel * car->pFShock->reboundRate, force );

            vecMA( force, (car->pFSpring->restLength - springLen) * car->pFSpring->rate, posDiff, force );
        }
        else
        {
            if( springVel < 0.0f )
                vecScale( posDiff, -springVel * car->pRShock->bumpRate, force );
            else
                vecScale( posDiff, -springVel * car->pRShock->reboundRate, force );

            vecMA( force, (car->pRSpring->restLength - springLen) * car->pRSpring->rate, posDiff, force );
        }

        dBodyAddForceAtPos( car->odeUprights[i], force[0], force[1], force[2], pos2[0], pos2[1], pos2[2] );
        dBodyAddForceAtPos( car->odeBody, -force[0], -force[1], -force[2], pos1[0], pos1[1], pos1[2] );
*/

namespace BREATHE
{
	namespace VEHICLE
	{
		float fFrontSlip=0.0001f;
		float fRearSlip=0.001f;

		cWheel::cWheel(cVehicle *parent)
			: PHYSICS::cPhysicsObject()
		{
			pParent=parent;

			bFront=false;
			bContact=false;


			fSuspensionK=6.0f;
			fSuspensionU=0.8f;

			fSuspensionNormal=0.0f;
			fSuspensionMin=0.0f;
			fSuspensionMax=0.0f;

			fContact=0.0f;

			
			std::memset(r, 0, sizeof(float)*16);
		}
		
		cWheel::~cWheel()
		{
			dGeomDestroy(ray_);
		}

		void cWheel::Init(bool bFront, float fWRadius, float fInWeight, float fSK, float fSU, float fSNormal, float fSMin, float fSMax, MATH::cVec3 &pos)
		{
			this->bFront=bFront;

			fRadius=fWRadius;
			fWeight=fInWeight;

			fSuspensionK=fSK;
			fSuspensionU=fSU;

			fSuspensionNormal=fSNormal;
			fSuspensionMin=fSMin;
			fSuspensionMax=fSMax;

			v3SuspensionTopRel=pos;
			v3SuspensionTop=pos;

			fTraction=0.0f;

			ray_ = dCreateRay(0, 1 );
		}

		void cWheel::RayCast()
		{
			MATH::cVec3 dir=-pParent->m.GetUp().GetNormalized();

			bContact = false;
			fContact = fSuspensionMax;

			std::memset( &contact_, 0, sizeof( contact_ ) );
			dGeomRaySet(ray_, v3SuspensionTop.x, v3SuspensionTop.y, v3SuspensionTop.z, dir.x, dir.y, dir.z);
			dGeomRaySetLength(ray_, 1.0f);
			dSpaceCollide2(ray_, (dGeomID)PHYSICS::spaceStatic, this, callback);
			dSpaceCollide2(ray_, (dGeomID)PHYSICS::spaceDynamic, this, callback);
		}

		void cWheel::callback( void * data, dGeomID g1, dGeomID g2 )
		{
			dContact c;

			if( dGeomGetBody( g1 ) == ((cWheel *)data)->pParent->body )
				return;

			if( dCollide( g2, g1, 1, &c.geom, sizeof(c) ) == 1)
			{
				if(c.geom.depth < ((cWheel *)data)->fContact) 
				{
					((cWheel *)data)->bContact = true;
					((cWheel *)data)->fContact = c.geom.depth;
					((cWheel *)data)->contact_ = c.geom;
				}
			}
		}

		void cWheel::Update(float fTime)
		{
			RayCast();


			MATH::cVec3 right=pParent->m.GetRight().GetNormalized();
			MATH::cVec3 front=pParent->m.GetFront().GetNormalized();
			MATH::cVec3 up=pParent->m.GetUp().GetNormalized();
			MATH::cVec3 down=-up;
			MATH::cVec3 dir=front;


			MATH::cMat4 mPositionRel;
			mPositionRel.SetTranslation(v3SuspensionTopRel);

			MATH::cMat4 mRotation;
			if(bFront)
        mRotation.SetRotationZ(-MATH::atan2(pParent->fSteer, 1));//BREATHE::MATH::cPI);



			MATH::cMat4 mContact;
			mContact.SetTranslation((fContact-fRadius)*MATH::v3Down);

			m=pParent->m*mPositionRel*mRotation*mContact;

			p=m.GetPosition();

			r[0] = m[0];		r[1] = m[4];		r[2] = m[8];		r[3] = 0;
			r[4] = m[1];		r[5] = m[5];		r[6] = m[9];		r[7] = 0;
			r[8] = m[2];		r[9] = m[6];		r[10] = m[10];	r[11] = 0;

			v=pParent->v;
			
			v3SuspensionTop = (pParent->m*mPositionRel).GetPosition();


			if(bContact)
			{
				float wheelAccel=pParent->fSpeed;
				float wheelBrake=pParent->fBrake;
				float wheelSlip=fabsf(pParent->fVel)*fabsf(pParent->fVel);

				if(bFront)
				{
					if(pParent->bFourWheelDrive)
						wheelAccel*=0.4f;
					else
						wheelAccel=0.0f;

					dir = (front + (right * pParent->fSteer)).GetNormalized();

					wheelSlip*=fFrontSlip;
					wheelBrake*=0.75f;
				}
				else
				{
					if(pParent->bFourWheelDrive)
						wheelAccel*=0.6f;

					wheelSlip*=fRearSlip;
					wheelBrake*=0.25;
				}


				dContact c;

				c.surface.mode = 0;        
        c.surface.mode |= dContactFDir1;
        c.surface.mode |= dContactMu2;
        c.surface.mode |= dContactMotion1;
        c.surface.mode |= dContactApprox1;
        c.surface.mode |= dContactSoftCFM;
        c.surface.mode |= dContactSoftERP;
        c.surface.mode |= dContactSlip1;
        c.surface.mode |= dContactSlip2;
        c.surface.mode |= dContactBounce;
				
				//Sideways friction
				//set the FDS slip coefficient in friction direction 2 to fSlip*v, 
				//where v is the tire rolling velocity and k is a tire parameter that you can chose based on experimentation.

				//Forward rolling friction
				//if( (pParent->fControl_Accelerate > 0.0f && pParent->fVel >= -0.1f) || 
				//	(pParent->fControl_Accelerate < 0.0f && pParent->fVel <= 0.1f) ) 
				//{
					c.surface.mu = (float)0.0001;
					c.surface.mu2 = wheelSlip * 0.9f;
				//}
				//else
				//{
				//	c.surface.mu = 0.4f;
				//	c.surface.mu2 = 10.0f;
				//}
				
				//  up/down contacts are full friction; sideways contacts are frictionless 
				//  to allow for better sliding behavior.
				BREATHE::MATH::cVec3 geom_normal = (BREATHE::MATH::cVec3 const &)c.geom.normal[0];
				c.surface.mu = wheelSlip * fabsf( geom_normal.z ) * 0.0000001f;

				c.surface.slip1 = wheelSlip;
				c.surface.slip2 = wheelSlip;


				//Collision repulsion
				c.surface.soft_erp = 0.2; //0.9999;
				c.surface.soft_cfm = 0.0001; //0.00001;
				
				//Bounce
				c.surface.bounce = 0.001f;
				c.surface.bounce_vel = 0.001f;


				c.geom = contact_;
				c.geom.depth = fContact;
				c.geom.g1 = pParent->geom;


				//float fDepth=0.5f/fSuspensionMax;
				//c.geom.depth = fDepth*fContact;

				(BREATHE::MATH::cVec3 &)c.fdir1 = dir;

				c.surface.motion1 = wheelAccel;
				
				MATH::cVec3 v(wheelAccel * dir * 100.0f);
				dBodyAddForce(pParent->body, v.x, v.y, v.z);

				if(bFront)
				{
					MATH::cVec3 v(down * pParent->fSteer * pParent->fSpeed * 100.0f);
					dBodyAddRelTorque(pParent->body, v.x, v.y, v.z);
				}

				dJointID j = dJointCreateContact(PHYSICS::world, PHYSICS::contactgroup, &c);
				dJointAttach(j, dGeomGetBody(c.geom.g1), dGeomGetBody(c.geom.g2));
				

				
				float dampTorque=10000.0f + (10000.0f * wheelBrake);
				float dampForce=100.0f + (4000.0f * wheelBrake);
				
				dReal const * av = dBodyGetAngularVel(pParent->body);
				dBodyAddTorque(pParent->body, -av[0]*dampTorque, -av[1]*dampTorque, -av[2]*dampTorque);

				dReal const * lv = dBodyGetLinearVel(pParent->body);
				dBodyAddForce(pParent->body, -lv[0]*dampForce, -lv[1]*dampForce, -lv[2]*dampForce);





				/*float restlength=1.0;
				float delta = fSuspensionMax-(fContact-fRadius);
				float deltalength = MATH::sqrt(delta*delta);
				float diff = (deltalength-restlength)/deltalength;

				float fForce=1.90f*pParent->fWeight*(fSuspensionMax-(fContact-fRadius))*fSuspensionK;
				float fTorque=10.0f;
				MATH::cVec3 force(up.x*fForce, up.y*fForce, up.z*fForce);
				MATH::cVec3 torque(up.z*fTorque, up.z*fTorque, 0.0f);
				MATH::cVec3 pos(v3SuspensionTopRel);
				//MATH::cVec3 pos(v3SuspensionTop);
				//MATH::cVec3 pos((pParent->m*mPositionRel).GetPosition());

				//dBodyAddRelForce(pParent->body, force.x, force.y, force.z);
				//dBodyAddRelForceAtRelPos(pParent->body, force.x, force.y, force.z, pos.x, pos.y, pos.z);*/


				fTraction=1.0f;
			}
			else
				fTraction=0.0f;
		}
	}
}
