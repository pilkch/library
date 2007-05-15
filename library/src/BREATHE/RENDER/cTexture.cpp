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
			uiMode = TEXTURE_RGBA;
			
			pData=NULL;
			surface=NULL;
		}

		cTexture::~cTexture()
		{
			SDL_FreeSurface(surface);
			SAFE_DELETE_ARRAY(pData);
		}
		
		bool cTexture::Load(std::string inFilename)
		{
			sFilename = inFilename;

			unsigned int mode = 0;
			surface = IMG_Load(sFilename.c_str());

			// could not load filename
			if (!surface)
			{
				LOG.Error("Texture", "Couldn't Load Texture " + sFilename);
				return BREATHE::BAD;
			}

			

			//Check the format
			if (8 == surface->format->BitsPerPixel)
			{
				LOG.Success("Texture", "Greyscale Heightmap Image " + sFilename);
				uiMode = TEXTURE_HEIGHTMAP;
			}
			else if (16 == surface->format->BitsPerPixel)
			{
				LOG.Success("Texture", "Greyscale Heightmap Image " + sFilename);
				uiMode = TEXTURE_HEIGHTMAP;
			}
			else if (24 == surface->format->BitsPerPixel)
			{
				// Add alpha channel
				SDL_PixelFormat format = {
					NULL, 32, 4, 0, 0, 0, 0, 
					0, 8, 16, 24, 
					0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000, 
					0, 255
				};
				SDL_Surface *pConvertedSurface = SDL_ConvertSurface(surface, &format, SDL_SWSURFACE);
				SDL_FreeSurface(surface);
				surface = pConvertedSurface;
			}
			else if (32 == surface->format->BitsPerPixel)
			{
				LOG.Success("Texture", "RGBA Image " + sFilename);
				uiMode = TEXTURE_RGBA;

				// Convert if BGR
				if (surface->format->Rshift > surface->format->Bshift)
				{
					SDL_PixelFormat format = {
						NULL, 32, 4, 0, 0, 0, 0, 
						0, 8, 16, 24, 
						0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000, 
						0, 255};
					SDL_Surface *pConvertedSurface = SDL_ConvertSurface(surface, &format, SDL_SWSURFACE);
					SDL_FreeSurface(surface);
					surface = pConvertedSurface;
				}
				
				/*int nHH = surface->h / 2;
				int nPitch = surface->pitch;
			
				unsigned char* pBuf = new unsigned char[nPitch];
				unsigned char* pSrc = (unsigned char*) surface->pixels;
				unsigned char* pDst = (unsigned char*) surface->pixels + nPitch*(surface->h - 1);
			
				while (nHH--)
				{
					std::memcpy(pBuf, pSrc, nPitch);
					std::memcpy(pSrc, pDst, nPitch);
					std::memcpy(pDst, pBuf, nPitch);
			
					pSrc += nPitch;
					pDst -= nPitch;
				};
			
				SAFE_DELETE_ARRAY(pBuf);*/
			}
			else
			{
				std::ostringstream t;
				t << surface->format->BitsPerPixel;
				LOG.Error("Texture", "Error Unknown Image Format (" + t.str() + "bit) " + sFilename);
				return BREATHE::BAD;
			}

			CopyFromSurface(surface->w, surface->h);

			return BREATHE::GOOD;
		}

		void cTexture::GenerateOpenGLTexture()
		{
			if(uiMode == TEXTURE_RGBA)
			{
				// create one texture name
				glGenTextures(1, &uiTexture);

				// tell opengl to use the generated texture name
				glBindTexture(GL_TEXTURE_2D, uiTexture);

				// this reads from the sdl surface and puts it into an opengl texture
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

				// these affect how this texture is drawn later on...
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				std::ostringstream s;
				s<<sFilename<<" "<<uiTexture<<"\0";
				LOG.Success("Texture", s.str().c_str());
			}
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
				pData = new unsigned char[uiWidth * uiHeight * (uiMode == TEXTURE_HEIGHTMAP ? 1 : 4)];

			std::memcpy(pData, surface->pixels, uiWidth * uiHeight * (uiMode == TEXTURE_HEIGHTMAP ? 1 : 4));
		}

		void cTexture::CopyToSurface()
		{
			if(pData)
        std::memcpy(surface->pixels, pData, uiWidth * uiHeight * (uiMode == TEXTURE_HEIGHTMAP ? 1 : 4));
		}

		void cTexture::Transform(float *u, float *v)
		{
			*u=(*u)*fScale+fU;
			*v=(*v)*fScale+fV;
		}

		bool cTexture::SaveToBMP(std::string inFilename)
		{
			SDL_SaveBMP(surface, inFilename.c_str());
			return BREATHE::GOOD;
		}
	}
}
