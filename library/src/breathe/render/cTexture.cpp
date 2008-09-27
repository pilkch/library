#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

// writing on a text file
#include <iostream>
#include <fstream>

#include <list>
#include <sstream>
#include <vector>
#include <map>
#include <string>

// Boost includes
#include <boost/shared_ptr.hpp>

// Anything else
#include <GL/GLee.h>
#include <GL/glu.h>

#include <SDL/SDL_image.h>

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/storage/filesystem.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cColour.h>
#include <breathe/math/geometry.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>

namespace breathe
{
	namespace render
	{
    cTexture::cTexture() :
			uiTexture(0),

			uiWidth(0),
			uiHeight(0),
			uiMode(TEXTURE_RGBA),

			fScale(1.0f),
			fU(0.0f),
			fV(0.0f),

			surface(nullptr)
    {
		}

		cTexture::~cTexture()
		{
			SDL_FreeSurface(surface);
      data.clear();
		}

    bool cTexture::Load(const string_t& inFilename)
		{
			sFilename = inFilename;

			unsigned int mode = 0;
			surface = IMG_Load(sFilename.c_str());

			// could not load filename
			if (surface == nullptr) {
				LOG.Error("Texture", "Couldn't Load Texture " + sFilename);
				return breathe::BAD;
			}



			//Check the format
			if (8 == surface->format->BitsPerPixel) {
				LOG.Success("Texture", "Greyscale Heightmap Image " + sFilename);
				uiMode = TEXTURE_HEIGHTMAP;
			} else if (16 == surface->format->BitsPerPixel) {
				LOG.Success("Texture", "Greyscale Heightmap Image " + sFilename);
				uiMode = TEXTURE_HEIGHTMAP;
			} else if (24 == surface->format->BitsPerPixel) {
				CONSOLE.Error("Texture", sFilename + " is a 24 bit RGB image");
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
			} else if (32 == surface->format->BitsPerPixel) {
        LOG.Success("Texture", sFilename + " is a 32 bit RGBA image");
				uiMode = TEXTURE_RGBA;

				// Convert if BGR
				if (surface->format->Rshift > surface->format->Bshift) {
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
			} else {
				std::ostringstream t;
				t << surface->format->BitsPerPixel;
				LOG.Error("Texture", "Error Unknown Image Format (" + t.str() + "bit) " + sFilename);
				return breathe::BAD;
			}

			uiWidth = surface->w;
			uiHeight = surface->h;

			{
				std::ostringstream t;
				t<<uiWidth;
				t<<"x";
				t<<uiHeight;
				LOG.Success("Texture", t.str());
			}
			CopyFromSurfaceToData(surface->w, surface->h);

			return breathe::GOOD;
		}

		void cTexture::CopyFromSurfaceToData(unsigned int w, unsigned int h)
		{
			// Fill out the pData structure array, we use this for when we have to reload this data
			// on a task switch or fullscreen mode change

			uiWidth = w;
			uiHeight = h;

			CopyFromSurfaceToData();
		}

		void cTexture::CopyFromSurfaceToData()
		{
      ASSERT(surface != nullptr);

			// Fill out the pData structure array, we use this for when we have to reload this data
			// on a task switch or fullscreen mode change
      if (data.empty()) data.resize(uiWidth * uiHeight * (uiMode == TEXTURE_HEIGHTMAP ? 1 : 4), 0);

			std::memcpy(&data[0], surface->pixels, uiWidth * uiHeight * (uiMode == TEXTURE_HEIGHTMAP ? 1 : 4));
		}

		void cTexture::CopyFromDataToSurface()
    {
      ASSERT(surface != nullptr);

			if (data.empty()) return;

      std::memcpy(surface->pixels, &data[0], uiWidth * uiHeight * (uiMode == TEXTURE_HEIGHTMAP ? 1 : 4));
		}

    bool cTexture::SaveToBMP(const string_t& inFilename)
		{
			SDL_SaveBMP(surface, inFilename.c_str());
			return breathe::GOOD;
		}

		void cTexture::_Create()
		{
			// Create new texture
			glGenTextures(1, &uiTexture);

			// Bind so that the next operations happen on this texture
			glBindTexture(GL_TEXTURE_2D, uiTexture);
		}

		void cTexture::Destroy()
		{
			// Destroy old texture
			glDeleteTextures(1, &uiTexture);
		}

		void cTexture::CopyFromSurfaceToTexture()
		{
			// Bind so that the next operations happen on this texture
			glBindTexture(GL_TEXTURE_2D, uiTexture);

			if (surface != nullptr) {
				// Copy from surface to texture
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

				//Remove this line if there are artifacts
				gluBuild2DMipmaps(GL_TEXTURE_2D, 4, surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

				// Settings to make the texture look a bit nicer when we do blit it to the screen
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
		}

		void cTexture::Reload()
		{
			// Delete the old texture
			Destroy();

			// Create a new one
			Create();

			// Copy from data buffer to surface
			CopyFromDataToSurface();

			// Copy from surface to texture
			CopyFromSurfaceToTexture();
		}



		// *** Frame Buffer Object
		cTextureFrameBufferObject::cTextureFrameBufferObject()
		{
			uiFBO = 0;
			uiFBODepthBuffer = 0;
			uiMode = TEXTURE_FRAMEBUFFEROBJECT;
		}

		cTextureFrameBufferObject::~cTextureFrameBufferObject()
		{
			glDeleteFramebuffersEXT(1, &uiFBO);
			glDeleteRenderbuffersEXT(1, &uiFBODepthBuffer);
			glDeleteTextures(1, &uiTexture);
		}

		void cTextureFrameBufferObject::_Create()
		{
			// Create FBO
			glGenFramebuffersEXT(1, &uiFBO);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, uiFBO);

			// Create the Render Buffer for Depth
			glGenRenderbuffersEXT(1, &uiFBODepthBuffer);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, uiFBODepthBuffer);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, FBO_TEXTURE_WIDTH, FBO_TEXTURE_HEIGHT);


			// Now setup a texture to render to
			glGenTextures(1, &uiTexture);
			glBindTexture(GL_TEXTURE_2D, uiTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  FBO_TEXTURE_WIDTH, FBO_TEXTURE_HEIGHT, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

#ifdef RENDER_GENERATEFBOMIPMAPS
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmapEXT(GL_TEXTURE_2D);
#endif // RENDER_GENERATEFBOMIPMAPS

			// And attach it to the FBO so we can render to it
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, uiTexture, 0);

			// Attach the depth render buffer to the FBO as it's depth attachment
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, uiFBODepthBuffer);


			GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
			if (status != GL_FRAMEBUFFER_COMPLETE_EXT) LOG.Error("Texture", "Frame buffer status failed");

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); // Unbind the FBO for now
		}
	}
}
