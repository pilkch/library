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
#include <breathe/physics/cContact.h>
#include <breathe/physics/cRayCast.h>
#include <breathe/physics/cPhysicsObject.h>

#include <breathe/game/cPlayer.h>
#include <breathe/game/cPetrolBowser.h>

#include <breathe/vehicle/cPart.h>
#include <breathe/vehicle/cVehicle.h>
#include <breathe/vehicle/cSeat.h>

namespace breathe
{
	cPlayer::cPlayer() : 
		physics::cUprightCapsule()
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
		physics::RemovePhysicsObject(this);
	}


	const float fMaxDistance = 2.5f;
	
	void cPlayer::RayCast()
	{
		math::cVec3 dir = math::v3Down;

		rayContact.Clear();

		rayContact.fDepth = fMaxDistance;
		
		dGeomRaySet(geomRay, p.x, p.y, p.z, dir.x, dir.y, dir.z);
		dGeomRaySetLength(geomRay, fMaxDistance);
		dSpaceCollide2(geomRay, (dGeomID)physics::spaceStatic, this, RayCastCallback);
		dSpaceCollide2(geomRay, (dGeomID)physics::spaceDynamic, this, RayCastCallback);
	}

	void cPlayer::RayCastCallback(void* data, dGeomID g1, dGeomID g2)
	{
		cPlayer* p = ((cPlayer*)data);
		if( dGeomGetBody( g1 ) == p->body ) return;

		dContact c;
		if( dCollide( g2, g1, 1, &c.geom, sizeof(c) ) == 1 && c.geom.depth < p->rayContact.fDepth)
			p->rayContact.SetContact(c.geom, p->geom, c.geom.depth);
	}

	void cPlayer::Update(sampletime_t currentTime)
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
			if(fInputUp > math::cEPSILON || fInputDown > math::cEPSILON || 
				fInputLeft > math::cEPSILON || fInputRight > math::cEPSILON)
			{	
				float fDirection = fHorizontal + math::toRadians(90.0f);

				if(fInputUp > math::cEPSILON && fInputUp > fInputDown)
				{
					if(fInputLeft > math::cEPSILON && fInputLeft > fInputRight)
						fDirection += math::toRadians(45.0f);
					else if(fInputRight > math::cEPSILON)
						fDirection -= math::toRadians(45.0f);
					
					p.z += fSpeed * sinf(fVertical - math::toRadians(90.0f));
				}
				else if(fInputDown > math::cEPSILON)
				{
					if(fInputLeft > math::cEPSILON && fInputLeft > fInputRight)
						fDirection += math::toRadians(125.0f);
					else if(fInputRight > math::cEPSILON)
						fDirection -= math::toRadians(125.0f);
					else
						fDirection += math::toRadians(180.0f);

					p.z += fSpeed * sinf(fVertical + math::toRadians(90.0f));
				}
				else if(fInputLeft > math::cEPSILON && fInputLeft > fInputRight)
				{
					fDirection += math::toRadians(90.0f);
					fSpeed *= fInputLeft;
				}
				else if(fInputRight > math::cEPSILON)
				{
					fDirection -= math::toRadians(90.0f);
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
			physics::cUprightCapsule::Update(currentTime);

			RayCast();

			if(rayContact.bContact)
			{
				// Push us out of whatever surface we are falling towards
				// We want to negate the downwards motion with an upwards one and then 
				// add a little bit, enough to get out of the surface
				{
					const dReal* pos = dBodyGetPosition(body);
					dBodySetPosition(body, pos[0], pos[1], pos[2] + /*0.03f * rayContact.fDepth*/ + math::cEPSILON);

					const dReal* vel = dBodyGetLinearVel(body);
					dBodySetLinearVel(body, vel[0], vel[1], ((vel[2] < 0.0f) ? 0.0f : vel[2]) + math::cEPSILON);
				}

				// Handle the actual movement
				{
					// Get a unit vector that represents which way the player is heading
					math::cVec3 v(fInputRight - fInputLeft, fInputUp - fInputDown, 0.0f);
					v.Normalize();

					// Bias the inputs slightly so that walking forwards is fastest, then left/right 
					// and then backwards is slowest to make for slightly more interesting gameplay
					v.x *= 0.8f;
					if(v.y < 0.0f) v.y *= 0.5f;

					// We have a vector saying which direction to go but it is not in world coordinates, 
					// add it to our current heading vector (A whole matrix is probably not needed, just
					// makes it a lot easier)
					math::cMat4 m;
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

	bool cPlayer::IsInACar() const
	{
		return (NULL != pSeat);
	}

	bool cPlayer::IsDriving() const
	{
		return ((NULL != pSeat) && (this == pSeat->pVehicle->vSeat[0]->pPlayer));
	}
	
	bool cPlayer::IsThirdPersonCamera() const
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

		physics::RemovePhysicsObject(this);

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
			physics::AddPhysicsObject(this);

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
