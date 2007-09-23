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
	cPlayer::cPlayer() : 
		PHYSICS::cUprightCapsule()
	{
		bDynamic = true;
		bBody = false;

		p.x = p.y = 0.0f;
		p.z = 10.0f;

		pSeat = NULL;

		uiState = PLAYER_STATE_WALK;
		
#ifdef BUILD_RELEASE
		uiCameraMode = CAMERA_FIRSTPERSON;
#else
		uiCameraMode = CAMERA_FIRSTPERSONFREE;
#endif

		// Movement
		fInputUp = fInputDown = fInputLeft = fInputRight = 0.0f;
		
		bInputHandbrake = bInputClutch = false;

		bInputJump = bInputCrouch = false;

		fSpeedWalk = 1.0f;
		fSpeedRun = 2.0f;
		fSpeedSprint = 3.0f;

		fWeight = 80.0f;

		fDollars=0.0f;
		
		fVertical=0.0f;
		fHorizontal=0.0f;
	}

	cPlayer::~cPlayer()
	{
		PHYSICS::RemovePhysicsObject(this);
	}


	const float fMaxDistance = 2.5f;
	
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
		float fSpeed = (PLAYER_STATE_WALK == uiState ? fSpeedWalk : (PLAYER_STATE_RUN == uiState ? fSpeedRun : fSpeedSprint));
		
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
		else if (body)
		{
			PHYSICS::cUprightCapsule::Update(fCurrentTime);

			RayCast();

			if(rayContact.bContact)
			{
				// Push us out of whatever surface we are falling towards
				// We want to negate the downwards motion with an upwards one and then 
				// add a little bit, enough to get out of the surface
				{
					const dReal* pos = dBodyGetPosition(body);
					dBodySetPosition(body, pos[0], pos[1], pos[2] + /*0.03f * rayContact.fDepth*/ + MATH::cEPSILON);

					const dReal* vel = dBodyGetLinearVel(body);
					dBodySetLinearVel(body, vel[0], vel[1], ((vel[2] < 0.0f) ? 0.0f : vel[2]) + MATH::cEPSILON);
				}

				// Handle the actual movement
				{
					// Get a unit vector that represents which way the player is heading
					MATH::cVec3 v(fInputRight - fInputLeft, fInputUp - fInputDown, 0.0f);
					v.Normalize();

					// Bias the inputs slightly so that walking forwards is fastest, then left/right 
					// and then backwards is slowest to make for slightly more interesting gameplay
					v.x *= 0.8f;
					if(v.y < 0.0f) v.y *= 0.5f;

					// We have a vector saying which direction to go but it is not in world coordinates, 
					// add it to our current heading vector (A whole matrix is probably not needed, just
					// makes it a lot easier)
					MATH::cMat4 m;
					m.SetRotationEuler(0.0f, 0.0f, fHorizontal);

					// Multiply our movement vector by our speed
					v = m.GetRotatedVec3(v) * fSpeed * 50.0f;
					
					// Get our current velocity add our movement velocity
					const dReal* vel = dBodyGetLinearVel(body);
          dBodySetLinearVel(body, v.x, v.y, vel[2]+v.z);
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

		PHYSICS::RemovePhysicsObject(this);

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
			PHYSICS::AddPhysicsObject(this);

			cPhysicsRayCast::Create(1.0f);
#ifdef BUILD_DEBUG
		}
#endif
	}

	void cPlayer::ChangeItemUp()
	{
		
	}

	void cPlayer::ChangeItemDown()
	{
		
	}
}