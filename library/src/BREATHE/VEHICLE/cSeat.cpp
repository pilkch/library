#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
#include <BREATHE/VEHICLE/cWheel.h>
#include <BREATHE/VEHICLE/cSeat.h>
#include <BREATHE/VEHICLE/cVehicle.h>

#if (_MSC_VER >= 1300) && (WINVER < 0x0500)
#pragma warning(disable:4305)
#pragma warning(disable:4244)
#endif

namespace BREATHE
{
	namespace VEHICLE
	{
		cSeat::cSeat(cVehicle *v)
		{
			pPlayer=NULL;
			pVehicle=v;
		}

		cSeat::~cSeat()
		{
			
		}

		void cSeat::AssignPlayer(cPlayer *p)
		{
			pPlayer=p;
			p->pSeat=this;
		}

		void cSeat::EjectPlayer()
		{
			cPlayer *p=pPlayer;

			pPlayer=NULL;

			p->pSeat=NULL;
			p->p=pVehicle->p+MATH::cVec3(0.0f, 0.0f, 3.0f);
		}
	}
}
