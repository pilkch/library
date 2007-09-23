// Basic standard library includes
#include <cmath>

// STL
#include <list>
#include <vector>

// writing on a text file
#include <iostream>
#include <fstream>
#include <sstream>

// Anything else
#include <ode/ode.h>

// Breathe
#include <BREATHE/cBreathe.h>

#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cVar.h>

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

#include <BREATHE/PHYSICS/cPhysics.h>
#include <BREATHE/PHYSICS/cContact.h>
#include <BREATHE/PHYSICS/cRayCast.h>
#include <BREATHE/PHYSICS/cPhysicsObject.h>


namespace BREATHE
{
	namespace PHYSICS
	{
		cPhysicsRayCast::cPhysicsRayCast()
		{
			geomRay = 0;
		}
		
		cPhysicsRayCast::~cPhysicsRayCast()
		{
			if(geomRay)
			{
				dGeomDestroy(geomRay);
				geomRay = 0;
			}
		}

		void cPhysicsRayCast::Create(float fLength)
		{
			rayContact.Clear();

			geomRay = dCreateRay(0, fLength);
		}
	}
}
