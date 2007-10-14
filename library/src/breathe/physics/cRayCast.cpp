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
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/util/cVar.h>

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

#include <breathe/physics/physics.h>
#include <breathe/physics/cContact.h>
#include <breathe/physics/cRayCast.h>
#include <breathe/physics/cPhysicsObject.h>


namespace breathe
{
	namespace physics
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
