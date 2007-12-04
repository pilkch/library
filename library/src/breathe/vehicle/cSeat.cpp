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

#if (_MSC_VER >= 1300) && (WINVER < 0x0500)
#pragma warning(disable:4305)
#pragma warning(disable:4244)
#endif

namespace breathe
{
	namespace vehicle
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
			
			if(pVehicle->vSeat[0] == this) pPlayer->ChangeStateToDriving();
		}

		void cSeat::EjectPlayer()
		{
			cPlayer *p=pPlayer;

			pPlayer=NULL;

			p->pSeat=NULL;
			p->p=pVehicle->p+math::cVec3(0.0f, 0.0f, 3.0f);
			
			p->ChangeStateToRunning();
		}
	}
}
