#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// writing on a text file
#include <iostream>
#include <fstream>

#include <list>
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

			uiWidth = 0;
			uiHeight = 0;
			
			pData=NULL;
			surface=NULL;
		}

		cTexture::~cTexture()
		{
			SDL_FreeSurface(surface);
			SAFE_DELETE_ARRAY(pData);
		}

		
		void cTexture::CopyFromSurface(unsigned int w, unsigned int h)
		{
			// Fill out the pData structure array, we use this for when we have to reload this data
			// on a task switch or fullscreen mode change

			uiWidth = w;
			uiHeight = h;

			CopyFromSurface();
		}

		void cTexture::CopyFromSurface()
		{
			// Fill out the pData structure array, we use this for when we have to reload this data
			// on a task switch or fullscreen mode change
			if(NULL == pData)
				pData = new unsigned char[uiWidth * uiHeight * 4];

			std::memcpy(pData, surface->pixels, uiWidth * uiHeight * 2);
		}

		void cTexture::CopyToSurface()
		{
			std::memcpy(surface->pixels, pData, uiWidth * uiHeight * 2);
		}

		void cTexture::Transform(float *u, float *v)
		{
			*u=(*u)*fScale+fU;
			*v=(*v)*fScale+fV;
		}
	}
}
