#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include <list>
#include <sstream>
#include <vector>
#include <map>

// writing on a text file
#include <iostream>
#include <fstream>

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/util/filesystem.h>

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
#include <breathe/render/model/cStatic.h>
#include <breathe/render/model/cAnimation.h>

namespace breathe
{
	namespace render
	{
		namespace model
		{
			cAnimation::cAnimation() : cModel()
			{
				LOG.Error("Animation", "Don't use this class");
			}

			cAnimation::~cAnimation()
			{
				LOG.Error("Animation", "Don't use this class");
			}		
			
			int cAnimation::Load(std::string sFilename)
			{
				LOG.Error("Animation", "Don't use this class");

				return 0;
			}

			void cAnimation::Update(float fCurrentTime)
			{
				LOG.Error("Animation", "Don't use this class");
			}
		}
	}
}
