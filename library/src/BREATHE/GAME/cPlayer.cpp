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


#include <BREATHE/cBreathe.h>

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
#include <BREATHE/RENDER/MODEL/cStatic.h>


#include <BREATHE/GAME/cLevel.h>

#include <BREATHE/PHYSICS/cPhysics.h>
#include <BREATHE/PHYSICS/cContact.h>
#include <BREATHE/PHYSICS/cRayCast.h>
#include <BREATHE/PHYSICS/cPhysicsObject.h>

#include <BREATHE/GAME/cPlayer.h>
#include <BREATHE/GAME/cPetrolBowser.h>

#include <BREATHE/VEHICLE/cPart.h>
#include <BREATHE/VEHICLE/cVehicle.h>
#include <BREATHE/VEHICLE/cSeat.h>

namespace BREATHE
{
	cPlayer::cPlayer()
			: PHYSICS::cUprightCapsule()
	{
		p.x = p.y = p.z = 0.0f;

		bBody=false;
		bDynamic=true;

		pSeat=NULL;

		uiState = PLAYER_STATE_DRIVE;
		
#ifdef BUILD_RELEASE
		uiCameraMode = CAMERA_THIRDPERSON;
#else
		uiCameraMode = CAMERA_THIRDPERSONFREE;
#endif

		// Movement
		fInputUp = fInputDown = fInputLeft = fInputRight = 0.0f;
		
		bInputHandbrake = bInputClutch = false;

		bInputJump = bInputCrouch = false;

		fSpeedWalk = 1.0f;
		fSpeedRun = 2.0f;
		fSpeedSprint = 3.0f;

		fWeight = 20.0f;

		fDollars=0.0f;
		
		fVertical=0.0f;
		fHorizontal=0.0f;
	}

	cPlayer::~cPlayer()
	{
		pPhysics->RemovePhysicsObject(this);
	}


	const float fMaxDistance = 5.5f;
	
	void cPlayer::RayCast()
	{
		MATH::cVec3 dir = MATH::v3Down;

		rayContact.Clear();

		rayContact.fDepth = fMaxDistance;
		
		dGeomRaySet(geomRay, p.x, p.y, p.z, dir.x, dir.y, dir.z);
		dGeomRaySetLength(geomRay, fMaxDistance);
		dSpaceCollide2(geomRay, (dGeomID)PHYSICS::spaceStatic, this, RayCastCallback);
		dSpaceCollide2(geomRay, (dGeomID)PHYSICS::spaceDynamic, this, RayCastCallback);
	}

	void cPlayer::RayCastCallback( void * data, dGeomID g1, dGeomID g2 )
	{
		cPlayer* p = ((cPlayer*)data);
		if( dGeomGetBody( g1 ) == p->body ) return;

		dContact c;
		if( dCollide( g2, g1, 1, &c.geom, sizeof(c) ) == 1 && c.geom.depth < p->rayContact.fDepth)
			p->rayContact.SetContact(c.geom, p->geom, c.geom.depth);
	}

	void cPlayer::Update(float fCurrentTime)
	{
		float fSpeed =	(PLAYER_STATE_WALK == uiState ? fSpeedWalk : (PLAYER_STATE_RUN == uiState ? fSpeedRun : fSpeedSprint));

		if(PLAYER_STATE_DRIVE == uiState)
		{
			p = pSeat->pVehicle->m.GetPosition();
		}
		else if(PLAYER_STATE_PASSENGER == uiState)
		{
			p = pSeat->pVehicle->m.GetPosition();
		}
#ifdef BUILD_DEBUG
		else if(uiCameraMode == CAMERA_FIRSTPERSONFREE)
		{
			if(fInputUp > MATH::cEPSILON || fInputDown > MATH::cEPSILON || 
				fInputLeft > MATH::cEPSILON || fInputRight > MATH::cEPSILON)
			{	
				float fDirection = fHorizontal + MATH::toRadians(90.0f);

				if(fInputUp > MATH::cEPSILON && fInputUp > fInputDown)
				{
					if(fInputLeft > MATH::cEPSILON && fInputLeft > fInputRight)
						fDirection += MATH::toRadians(45.0f);
					else if(fInputRight > MATH::cEPSILON)
						fDirection -= MATH::toRadians(45.0f);
					
					p.z += fSpeed * sinf(fVertical - MATH::toRadians(90.0f));
				}
				else if(fInputDown > MATH::cEPSILON)
				{
					if(fInputLeft > MATH::cEPSILON && fInputLeft > fInputRight)
						fDirection += MATH::toRadians(125.0f);
					else if(fInputRight > MATH::cEPSILON)
						fDirection -= MATH::toRadians(125.0f);
					else
						fDirection += MATH::toRadians(180.0f);

					p.z += fSpeed * sinf(fVertical + MATH::toRadians(90.0f));
				}
				else if(fInputLeft > MATH::cEPSILON && fInputLeft > fInputRight)
				{
					fDirection += MATH::toRadians(90.0f);
					fSpeed *= fInputLeft;
				}
				else if(fInputRight > MATH::cEPSILON)
				{
					fDirection -= MATH::toRadians(90.0f);
					fSpeed *= fInputRight;
				}

				//float fUpDown = fInputUp - fInputDown;
				//float fLeftRight = fInputLeft - fInputRight;

				//p.x += fUpDown * cosf(fDirection);
				//p.y += fLeftRight * sinf(fDirection);
				p.x += fSpeed * cosf(fDirection);
				p.y += fSpeed * sinf(fDirection);
			}
		}
#endif
		else
		{
			PHYSICS::cUprightCapsule::Update(fCurrentTime);

			RayCast();

			if(rayContact.bContact)
			{
				// Push us out of whatever surface we are falling towards
				// We want to negate the downwards motion with an upwards one and then 
				// add a little bit, enough to get out of the surface
				//if(rayContact.fDepth < 10.0f)
				
				if(body && rayContact.fDepth < 0.5f)
				{
					const dReal* pos = dBodyGetPosition(body);
					dBodySetPosition(body, pos[0], pos[1], pos[2] + MATH::cEPSILON);

					const dReal* vel = dBodyGetLinearVel(body);
					dBodySetLinearVel(body, vel[0], vel[1], (vel[2] < 0.0f) ? 0.0f : vel[2]);
				}

				if(fInputUp > MATH::cEPSILON || fInputDown > MATH::cEPSILON || 
					fInputLeft > MATH::cEPSILON || fInputRight > MATH::cEPSILON)
				{
					float fDirection = fHorizontal + MATH::toRadians(90.0f);

					if(fInputUp > MATH::cEPSILON && fInputUp > fInputDown)
					{
						if(fInputLeft > MATH::cEPSILON && fInputLeft > fInputRight)
							fDirection += MATH::toRadians(45.0f);
						else if(fInputRight > MATH::cEPSILON)
							fDirection -= MATH::toRadians(45.0f);
					}
					else if(fInputDown > MATH::cEPSILON)
					{
						if(fInputLeft > MATH::cEPSILON && fInputLeft > fInputRight)
							fDirection += MATH::toRadians(125.0f);
						else if(fInputRight > MATH::cEPSILON)
							fDirection -= MATH::toRadians(125.0f);
						else
							fDirection += MATH::toRadians(180.0f);
					}
					else if(fInputLeft > MATH::cEPSILON && fInputLeft > fInputRight)
					{
						fDirection += MATH::toRadians(90.0f);
						fSpeed *= fInputLeft;
					}
					else if(fInputRight > MATH::cEPSILON)
					{
						fDirection -= MATH::toRadians(90.0f);
						fSpeed *= fInputRight;
					}

					if(body)
					{
						// Do the actual movement
						{
							fSpeed *= 10.0f;
							MATH::cVec3 v(fSpeed * cosf(fDirection), fSpeed * sinf(fDirection), 0.0f);
							//dBodyAddForce(body, v.x, v.y, v.z);
							//dBodyAddTorque(body, v.x, v.y, v.z);
							dBodyAddForce(body, v.x/20, v.y/20, v.z/20);
							const dReal* vel = dBodyGetLinearVel(body);
							dBodySetLinearVel(body, vel[0]+v.x/20, vel[1]+v.y/20, vel[2]+v.z/20);
						}
					}
				}
			}
			else
			{
				//const dReal* v0 = dBodyGetLinearVel(body);
				//dBodySetLinearVel(body, 0.0f, 0.0f, v0[2]);
			}

			p = m.GetPosition();
		}
	}

	bool cPlayer::IsInACar()
	{
		return (NULL != pSeat);
	}

	bool cPlayer::IsDriving()
	{
		return ((NULL != pSeat) && (this == pSeat->pVehicle->vSeat[0]->pPlayer));
	}
	
	bool cPlayer::IsThirdPersonCamera()
	{
#ifdef BUILD_RELEASE
		return (uiCameraMode == CAMERA_THIRDPERSON);
#else
		return (uiCameraMode == CAMERA_THIRDPERSON) || (uiCameraMode == CAMERA_THIRDPERSONFREE);
#endif
	}

	
	void cPlayer::ChangeStateToDriving()
	{
		uiState = PLAYER_STATE_DRIVE;

		pPhysics->RemovePhysicsObject(this);

		if(geom) {
			dGeomDestroy(geom);
			geom = NULL;
		}

		if(bBody) {
      dBodyDestroy(body);
			body = NULL;
		}
		
		bBody = false;

		

		if(geomRay)
		{
			dGeomDestroy(geomRay);
			geomRay = 0;
		}
	}
		
	void cPlayer::ChangeStateToRunning()
	{
		uiState = PLAYER_STATE_RUN;
		
#ifdef BUILD_DEBUG
		if(uiCameraMode != CAMERA_FIRSTPERSONFREE)
		{
#endif
			bBody = true;
			CreateCapsule(p);
			pPhysics->AddPhysicsObject(this);

			cPhysicsRayCast::Create(1.0f);
#ifdef BUILD_DEBUG
		}
#endif
	}
}
