#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <list>

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

#include <BREATHE/PHYSICS/cPhysicsObject.h>
#include <BREATHE/PHYSICS/cPhysics.h>
#include <BREATHE/PHYSICS/cContact.h>

#include <BREATHE/GAME/cPlayer.h>
#include <BREATHE/GAME/cPetrolBowser.h>

#include <BREATHE/VEHICLE/cPart.h>
#include <BREATHE/VEHICLE/cVehicle.h>
#include <BREATHE/VEHICLE/cSeat.h>

namespace BREATHE
{
	cPlayer::cPlayer()
			: PHYSICS::cPhysicsObject()
	{
		bBody=false;
		bDynamic=false;

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

		fDollars=0.0f;
		
		fVertical=0.0f;
		fHorizontal=0.0f;
	}

	cPlayer::~cPlayer()
	{
		pPhysics->RemovePhysicsObject(this);
	}

	void cPlayer::Update(float fTime)
	{
		if(PLAYER_STATE_DRIVE == uiState)
		{
			p = pSeat->pVehicle->m.GetPosition();
			//m.SetTranslationPart(pSeat->pVehicle->m.GetPosition());
		}
		else if(PLAYER_STATE_PASSENGER == uiState)
		{
			//m.SetTranslationPart(pSeat->pVehicle->m.GetPosition());
		}
		else
		{
			float fSpeed =	(PLAYER_STATE_WALK == uiState ? fSpeedWalk : (PLAYER_STATE_RUN == uiState ? fSpeedRun : fSpeedSprint));
				
			float fDirection = fHorizontal + MATH::toRadians(90.0f);

			if(fInputUp)
			{
				if(fInputLeft)
					fDirection += MATH::toRadians(45.0f);
				else if(fInputRight)
					fDirection -= MATH::toRadians(45.0f);
				
#ifdef BUILD_DEBUG
				if(uiCameraMode == CAMERA_FIRSTPERSONFREE)
					p.z += fSpeed * sinf(fVertical - MATH::toRadians(90.0f));
#endif
			}
			else if(fInputDown)
			{
				if(fInputLeft)
					fDirection -= MATH::toRadians(235.0f);
				else if(fInputRight)
					fDirection += MATH::toRadians(235.0f);
				else
					fDirection += MATH::toRadians(180.0f);

#ifdef BUILD_DEBUG
				if(uiCameraMode == CAMERA_FIRSTPERSONFREE)
					p.z += fSpeed * sinf(fVertical + MATH::toRadians(90.0f));
#endif
			}
			else if(fInputLeft)
				fDirection += MATH::toRadians(90.0f);
			else if(fInputRight)
				fDirection -= MATH::toRadians(90.0f);

			p.x += fSpeed * cosf(fDirection);
			p.y += fSpeed * sinf(fDirection);
		}
	}
}
