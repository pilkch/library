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
#include <breathe/vehicle/cVehicle.h>

namespace breathe
{
	namespace vehicle
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
