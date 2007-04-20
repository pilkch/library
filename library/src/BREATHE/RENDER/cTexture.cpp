#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// writing on a text file
#include <iostream>
#include <fstream>

#include <sstream>
#include <vector>
#include <map>
#include <string>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>


// Breathe
#include <BREATHE/cBreathe.h>
#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cFileSystem.h>

#include <BREATHE/MATH/cMath.h>
#include <BREATHE/MATH/cVec2.h>
#include <BREATHE/MATH/cVec3.h>
#include <BREATHE/MATH/cVec4.h>
#include <BREATHE/MATH/cMat4.h>
#include <BREATHE/MATH/cPlane.h>
#include <BREATHE/MATH/cQuaternion.h>
#include <BREATHE/MATH/cFrustum.h>
#include <BREATHE/MATH/cColour.h>

#include <BREATHE/RENDER/cTexture.h>

namespace BREATHE
{
	namespace RENDER
	{
		cTexture::cTexture()
		{
			fScale=1.0f;
			fU=0.0f;
			fV=0.0f;
		}

		cTexture::~cTexture()
		{

		}

		void cTexture::Transform(float *u, float *v)
		{
			*u=(*u)*fScale+fU;
			*v=(*v)*fScale+fV;
		}
	}
}
