#include <cstdlib>
#include <cmath>

#include <vector>
#include <limits>

// Breathe
#include <breathe/breathe.h>

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

#ifdef max
#undef max
#endif

namespace breathe
{
	namespace math
	{
		const float cPI							= 3.14159265358979323846f;
		const float cPI_PI					= 6.28318530717958647692f;
		const float cPI_DIV_2				= 1.57079632679489661923f;
		const float cPI_DIV_180			=	0.01745329251994329576f;
		const float c180_DIV_PI			=	57.2957795130823208767f;

		const float cSQUARE_ROOT_2	=	1.41421356237309504880f;

		const float cEPSILON				=	0.001f;//10e-6f //std::numeric_limits<float>::epsilon();
		const float cINFINITY				=	std::numeric_limits<float>::max(); //1e30f //std::numeric_limits<float>::infinity();
	}
}
