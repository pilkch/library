#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <string>
#include <iostream>
#include <sstream>
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

namespace BREATHE
{
	namespace VEHICLE
	{
		cPart::cPart()
		{
			uiType=VEHICLEPART_NONE;
			fHealth=100;
		}
		
		void cPart::Install(cVehicle *p)
		{
			pVehicle=p;
		}


		cPartTurboCharger::cPartTurboCharger()
			: cPart()
		{
			uiType=VEHICLEPART_TURBOCHARGER;
		}

		void cPartTurboCharger::Install(cVehicle *p)
		{
			cPart::Install(p);


		}

		void cPartTurboCharger::Update()
		{
			
		}

		

		cPartSuperCharger::cPartSuperCharger()
			: cPart()
		{
			uiType=VEHICLEPART_SUPERCHARGER;
		}

		void cPartSuperCharger::Install(cVehicle *p)
		{
			cPart::Install(p);


		}
		
		void cPartSuperCharger::Update()
		{
			
		}
	}
}
