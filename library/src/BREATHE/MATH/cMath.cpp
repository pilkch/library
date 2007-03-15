#include <cmath>

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

namespace BREATHE
{
	namespace MATH
	{
		//Note: we can probably cut like 10 digits off of each of these,
		//would it result in a speed increase at all?  
		const float cPI=3.14159265358979323846f;
		const float cPI_PI=6.28318530717958647692f;
		const float cPI_DIV_2=1.57079632679489661923f;
		const float cPI_DIV_180=0.01745329251994329576f;
		const float c180_DIV_PI=57.2957795130823208767f;

		const float cEPSILON=0.0001f;
	}
}
