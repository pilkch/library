#include <sstream>
#include <map>
#include <vector>


#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cFileSystem.h>

#include <math.h>
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
#include <BREATHE/MODEL/cAnimation.h>

namespace BREATHE
{
	namespace MODEL
	{
		cAnimation::cAnimation() : cModel()
		{
			pLog->Error("Animation", "Don't use this class");
		}

		cAnimation::~cAnimation()
		{
			pLog->Error("Animation", "Don't use this class");
		}		
		
		int cAnimation::Load(std::string sFilename)
		{
			pLog->Error("Animation", "Don't use this class");

			return 0;
		}

		void cAnimation::Update(float fTime)
		{
			pLog->Error("Animation", "Don't use this class");
		}
	}
}
