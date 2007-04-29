#include <cmath>

#include <list>
#include <vector>

// writing on a text file
#include <iostream>
#include <fstream>


#include <ode/ode.h>

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

#include <BREATHE/PHYSICS/cPhysicsObject.h>
#include <BREATHE/PHYSICS/cPhysics.h>
#include <BREATHE/PHYSICS/cUprightCapsule.h>

namespace BREATHE
{
	namespace PHYSICS
	{	
		cUprightCapsule::cUprightCapsule()
			: cPhysicsObject()
		{

		}

		cUprightCapsule::~cUprightCapsule()
		{

		}
		
		void cUprightCapsule::Update(float fTime)
		{
			cPhysicsObject::Update(fTime);
		}
	}
}
