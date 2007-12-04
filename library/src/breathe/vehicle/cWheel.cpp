#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <set>


#include <ODE/ode.h>

#include <breathe/breathe.h>

#include <breathe/util/cString.h>

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
#include <breathe/render/model/cStatic.h>

#include <breathe/game/cLevel.h>

#include <breathe/physics/physics.h>

#include <breathe/game/cPlayer.h>
#include <breathe/game/cPetrolBowser.h>

#include <breathe/vehicle/cPart.h>
#include <breathe/vehicle/cWheel.h>
#include <breathe/vehicle/cSeat.h>
#include <breathe/vehicle/cVehicle.h>

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

#if (_MSC_VER >= 1300) && (WINVER < 0x0500)
#pragma warning(disable:4305)
#pragma warning(disable:4244)
#endif

#ifndef BUILD_EDITOR
extern float fAAA;
extern float fBBB;
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

namespace breathe
{
	namespace vehicle
	{
		float fFrontSlip=0.0001f;
		float fRearSlip=0.001f;

		cWheel::cWheel(cVehicle *parent)
			: physics::cPhysicsObject()
		{
			pParent=parent;

			bFront=false;


			fSuspensionK=6.0f;
			fSuspensionU=0.8f;

			fSuspensionNormal=0.0f;
			fSuspensionMin=0.0f;
			fSuspensionMax=0.0f;
		}
		
		void cWheel::Init(bool bFront, float fWRadius, float fInWeight, float fSK, float fSU, float fSNormal, float fSMin, float fSMax, math::cVec3 &pos)
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

			fTraction=1.0f;
			
			cPhysicsRayCast::Create(1.0f);
		}

		void cWheel::RayCast()
		{
			math::cVec3 dir=-pParent->m.GetUp().GetNormalized();

			rayContact.Clear();

			rayContact.fDepth = fSuspensionMax - math::cEPSILON;
			
			dGeomRaySet(geomRay, v3SuspensionTop.x, v3SuspensionTop.y, v3SuspensionTop.z, dir.x, dir.y, dir.z);
			dGeomRaySetLength(geomRay, fSuspensionMax);
			dSpaceCollide2(geomRay, (dGeomID)physics::spaceStatic, this, RayCastCallback);
			dSpaceCollide2(geomRay, (dGeomID)physics::spaceDynamic, this, RayCastCallback);
		}

		void cWheel::RayCastCallback( void * data, dGeomID g1, dGeomID g2 )
		{
			dContact c;

			cWheel* p = ((cWheel*)data);

			if( dGeomGetBody( g1 ) == p->pParent->body )
				return;

			if( dCollide( g2, g1, 1, &c.geom, sizeof(c) ) == 1 && c.geom.depth < p->rayContact.fDepth)
				p->rayContact.SetContact(c.geom, p->pParent->geom, c.geom.depth);
		}


		//------------------------------------------------------------------------------------------------
		/*void Vehicle::applyAntiSwayBarForces (  )
		{
			Real amt;
			Body * wheelBody;
			for( int i = 0; i < 4; i++)
			{
				SuspensionJoint * const wheelJoint = static_cast<SuspensionJoint*>( _wheels.at( i )->getBody()->getJoint(0) );
				const Ogre::Vector3 anchor2 (wheelJoint->getAdditionalAnchor());
				const Ogre::Vector3 anchor1 (wheelJoint->getAnchor());
				const Ogre::Vector3 axis (wheelJoint->getAxis());
				const Ogre::Real displacement = ( anchor1 - anchor2 ).dotProduct( axis );
				if( displacement > 0 )
				{

					amt = displacement * _swayForce;
					if( amt > _swayForceLimit ) 
						amt = _swayForceLimit;

					// force down
					wheelBody = _wheels.at( i )->getBody();
					wheelBody->addForce( -axis * amt );

					// force up
					wheelBody = _wheels.at( i^1 )->getBody();
					wheelBody->addForce( axis * amt );
				}
			}
		}*/

		void cWheel::Update(sampletime_t currentTime)
		{
			// Find out if this wheel is even touching anything
			RayCast();
		
			// Set up the vectors to use when working out wheel rotation and position
			math::cVec3 right=pParent->m.GetRight().GetNormalized();
			math::cVec3 front=pParent->m.GetFront().GetNormalized();
			math::cVec3 up=pParent->m.GetUp().GetNormalized();
			math::cVec3 down=-up;
			
			math::cVec3 dir=front;

			{
				float fContact = rayContact.bContact ? rayContact.fDepth : fSuspensionMax;

				math::cMat4 mPositionRel;
				mPositionRel.SetTranslation(v3SuspensionTopRel);

				math::cMat4 mRotation;
				if(bFront)
					mRotation.SetRotationZ(-atan2(pParent->fSteer, 1.0f));



				math::cMat4 mContact;
				mContact.SetTranslation((fContact-fRadius)*math::v3Down);

				m=pParent->m*mPositionRel*mRotation*mContact;

				p=m.GetPosition();

				v=pParent->v;
				
				v3SuspensionTop = (pParent->m*mPositionRel).GetPosition();
			}


			if(rayContact.bContact)
			{
				// Traction will range from 
				// 0.0f (No contact)
				// and
				// 1.0f (We are at the boundary between no weight being put on the wheel/suspension and weight being applied)
				// When we compress the springs and the tyre we still have fTraction=1.0f;
				fTraction=1.0f;

				float wheelAccel=pParent->fSpeed;
				float wheelBrake=pParent->fBrake;
				float wheelSlip=0.00001f * fabsf(pParent->fVel) * fabsf(pParent->fVel);

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

				
				


				// *** Create the contact

				// Sideways friction
				// set friction direction 1 in the direction that the tire is rolling in, 
				// and set the FDS slip coefficient in friction direction 2 to fTireConstant*fTireRollingVelocity, 
				// where fTireRollingVelocity is the tire rolling velocity and 
				// fTireConstant is a tire parameter that you can chose based on experimentation.

				float fTireConstant=1.0f;
				float fTireRollingVelocity=1.0f;

				rayContact.SetFrictionDirection1(dir);
				rayContact.SetFDSSlipCoefficient2(fTireConstant*fTireRollingVelocity);

				//rayContact.SetFDSSlipCoefficient1(wheelSlip);
				//rayContact.SetFDSSlipCoefficient2(wheelSlip);


				// Forward rolling friction
				// up/down contacts are full friction; sideways contacts are frictionless 
				// to allow for better sliding behavior.
				//breathe::math::cVec3 geom_normal = (breathe::math::cVec3 const &)c.geom.normal[0];
				//contact.setCoulombFriction(wheelSlip * std::min(fabsf(geom_normal.z), 1.0f) * 0.0000001f);
				//if( (pParent->fControl_Accelerate > 0.0f && pParent->fVel >= -0.1f) || 
				//	(pParent->fControl_Accelerate < 0.0f && pParent->fVel <= 0.1f) ) 
					 rayContact.SetCoulombFriction(0.001f);
				//else
				//	rayContact.setCoulombFriction(0.4f);




				 
				// According to the ODE docs;

				// By adjusting the values of ERP and CFM, you can achieve various effects. 
				// For example you can simulate springy constraints, where the two bodies oscillate 
				// as though connected by springs. Or you can simulate more spongy constraints, without 
				// the oscillation. In fact, ERP and CFM can be selected to have the same effect as any 
				// desired spring and damper constants. 
				// If you have a spring constant fSuspensionK and damping constant fSuspensionU, 
				// then the corresponding ODE constants are:

				// ERP = fStep * fSuspensionK / ((fStep * fSuspensionK) + fSuspensionU)
				// CFM = 1.0f / ((fStep * fSuspensionK) + fSuspensionU)

				// where h is the stepsize. These values will give the same effect as a spring-and-damper 
				// system simulated with implicit first order integration.
				
				// Suspension
				{
#ifdef BUILD_EDITOR
					fSuspensionK = 6.0f;
					fSuspensionU = 1.0f;
#else
					fSuspensionK = fAAA;
					fSuspensionU = fBBB;
#endif
					dReal const * lv = dBodyGetLinearVel( pParent->body );
					if(lv[2] < 0.3f)
						rayContact.SetSuspensionKU(fSuspensionK, fSuspensionU);
					else
					{
            //fSuspensionK /= (lv[2] * lv[2] * fSuspensionMax);
						if(rayContact.fDepth > fSuspensionNormal) rayContact.fDepth = fSuspensionNormal - math::cEPSILON;
						//rayContact.SetElasticity(physics::fERP, physics::fCFM);
						if(lv[2] < 0.0f)
              rayContact.SetSuspensionKU(fSuspensionK, fSuspensionU);
						else
              rayContact.SetSuspensionKU(fSuspensionK * 0.8f, fSuspensionU);
					}
				}
				
				// Bounce
				rayContact.SetBounce(physics::fBounce, physics::fBounceVel);

				// Contact Depth
				rayContact.CreateContact(rayContact.fDepth);

				/*{
          dReal const * lv = dBodyGetLinearVel( pParent->body );
					math::cVec3 v(math::v3Up * 0.001f * (lv[2] < 0.0f ? -lv[2] : lv[2]) / (rayContact.fDepth / fSuspensionMax));
					//dBodyAddForce(pParent->body, v.x, v.y, v.z);
				}*/

				
				// Add forces to push the car forward/backward
				{
					math::cVec3 v(wheelAccel * dir * 100.0f);
					dBodyAddForce(pParent->body, v.x, v.y, v.z);
				}

				// Add force to turn the car
				if(bFront)
				{
					math::cVec3 v(down * pParent->fSteer * pParent->fSpeed * 100.0f);
					dBodyAddRelTorque(pParent->body, v.x, v.y, v.z);
				}



				/*float restlength=1.0;
				float delta = fSuspensionMax-(fContact-fRadius);
				float deltalength = math::sqrt(delta*delta);
				float diff = (deltalength-restlength)/deltalength;

				float fForce=1.90f*pParent->fWeight*(fSuspensionMax-(fContact-fRadius))*fSuspensionK;
				float fTorque=10.0f;
				math::cVec3 force(up.x*fForce, up.y*fForce, up.z*fForce);
				math::cVec3 torque(up.z*fTorque, up.z*fTorque, 0.0f);
				math::cVec3 pos(v3SuspensionTopRel);
				//math::cVec3 pos(v3SuspensionTop);
				//math::cVec3 pos((pParent->m*mPositionRel).GetPosition());

				//dBodyAddRelForce(pParent->body, force.x, force.y, force.z);
				//dBodyAddRelForceAtRelPos(pParent->body, force.x, force.y, force.z, pos.x, pos.y, pos.z);*/

				
				// Rolling Drag
				float fDampTorque = 100.0f;
				float fDampLinearVel = 100.0f;
				dReal const * av = dBodyGetAngularVel( pParent->body );
				dReal const * lv = dBodyGetLinearVel( pParent->body );

				// TODO: Check whether we are on our roof/side too
				//dBodyAddTorque( pParent->body, -av[0]*fDampTorque, -av[1]*fDampTorque, -av[2]*fDampTorque );
				//dBodyAddForce( pParent->body, -lv[0]*fDampLinearVel, -lv[1]*fDampLinearVel, -lv[2]*fDampLinearVel );
			}
			else
				fTraction=0.0f;
		}
	}
}
