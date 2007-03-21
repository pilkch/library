#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <list>

#include <ODE/ode.h>



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
#include <BREATHE/MODEL/cStatic.h>

#include <BREATHE/RENDER/cCamera.h>
#include <BREATHE/GAME/cLevel.h>

#include <BREATHE/PHYSICS/cPhysicsObject.h>
#include <BREATHE/PHYSICS/cPhysics.h>
#include <BREATHE/PHYSICS/cContact.h>

#include <BREATHE/GAME/cPlayer.h>
#include <BREATHE/GAME/cPetrolBowser.h>

#include <BREATHE/VEHICLE/cPart.h>
#include <BREATHE/VEHICLE/cVehicle.h>

namespace BREATHE
{
	cPlayer::cPlayer()
		: cPhysicsObject()
	{
		pSeat=NULL;

		bAccelerate=false;
		bBrake=false;
		bLeft=false;
		bRight=false;
		bHandbrake=false;

		fDollars=0.0f;
	}

	cPlayer::~cPlayer()
	{
		
	}

	void cPlayer::Update(float fTime)
	{

	}
}
