#include <cmath>

#include <string>

#include <breathe/breathe.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cColour.h>

#include <breathe/util/base.h>

namespace breathe
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
		if (fInLength > fRadius) fRadius = fInLength;
		if (fInHeight > fRadius) fRadius = fInHeight;

		fRadius *= math::cSQUARE_ROOT_2;
	}
	
	// Collisions
	bool cObjectSphere::Collide(cObjectSphere& rhs)
	{
		return (GetDistance(rhs) < 0.0f);
	}

	float cObjectSphere::GetDistance(cObjectSphere& rhs) const
	{
		return math::sqrf(p.GetSquaredLength() + rhs.p.GetSquaredLength());
	}


	bool cObject::Collide(cObject& rhs)
	{
		if (cObjectSphere::Collide(rhs))
			return true;

		

		return false;
	}

	float cObject::GetDistance(cObject& rhs) const
	{
		float fDistance = cObjectSphere::GetDistance(rhs);
		
		
		
		return fDistance;
	}
}
