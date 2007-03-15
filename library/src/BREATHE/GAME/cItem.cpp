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

#include <BREATHE/GAME/cItem.h>

namespace BREATHE
{
	cItem::cItem()
		: cUpdateable()
	{

	}

	cItem::~cItem()
	{

	}
}
