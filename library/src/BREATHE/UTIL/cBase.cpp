#include <cmath>

#include <string>

#include <BREATHE/cBreathe.h>

#include <BREATHE/MATH/cMath.h>
#include <BREATHE/MATH/cVec2.h>
#include <BREATHE/MATH/cVec3.h>
#include <BREATHE/MATH/cVec4.h>
#include <BREATHE/MATH/cMat4.h>
#include <BREATHE/MATH/cPlane.h>
#include <BREATHE/MATH/cQuaternion.h>
#include <BREATHE/MATH/cFrustum.h>
#include <BREATHE/MATH/cColour.h>

#include <BREATHE/UTIL/cBase.h>

namespace BREATHE
{
	cObject::cObject()
		: cUpdateable()
	{
		fRadius=0.5f;
		fWidth=1.0f;
		fHeight=1.0f;
		fLength=1.0f;

		fWeight=1.0f;
	}
}
