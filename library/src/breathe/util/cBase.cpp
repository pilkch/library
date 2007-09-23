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
	// cObjectSphere
	cObjectSphere::cObjectSphere()
	{
		fRadius = 1.0f;
	}
	
	void cObjectSphere::SetDimensions(float fInRadius)
	{
		fRadius = fInRadius;
	}

	// cObject
	cObject::cObject()
	{
		fWidth=1.0f;
		fHeight=1.0f;
		fLength=1.0f;

		fWeight=1.0f;
	}

	
	void cObject::SetDimensions(float fInWidth, float fInLength, float fInHeight)
	{
		fWidth = fInWidth;
		fLength = fInLength;
		fHeight = fInHeight;

		fRadius = fWidth;
		if(fInLength > fRadius) fRadius = fInLength;
		if(fInHeight > fRadius) fRadius = fInHeight;

		fRadius *= MATH::cSQUARE_ROOT_2;
	}
	
	// Collisions
	bool cObjectSphere::Collide(cObjectSphere& rhs)
	{
		return (GetDistance(rhs) < 0.0f);
	}

	float cObjectSphere::GetDistance(cObjectSphere& rhs)
	{
		return MATH::sqrf(p.GetSquaredLength() + rhs.p.GetSquaredLength());
	}


	bool cObject::Collide(cObject& rhs)
	{
		if(cObjectSphere::Collide(rhs))
			return true;

		

		return false;
	}

	float cObject::GetDistance(cObject& rhs)
	{
		float fDistance = cObjectSphere::GetDistance(rhs);
		
		
		
		return fDistance;
	}
}
