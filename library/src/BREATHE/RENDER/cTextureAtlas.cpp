#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>


#include <BREATHE/cMem.h>

#include <BREATHE/cLog.h>
#include <BREATHE/cBreathe.h>
#include <BREATHE/cFileSystem.h>

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
#include <BREATHE/RENDER/cTextureAtlas.h>

namespace BREATHE
{	
	namespace RENDER
	{
		cTextureAtlas::cTextureAtlas(unsigned int id)
			: cTexture()
		{
			uiID=id;

			uiSegmentWidthPX=128;
			uiSegmentSmallPX=128;
			uiAtlasWidthPX=128;
			
			uiAtlasWidthNSegments=1;
			uiAtlasSegmentN=1;

			surface=NULL;

			uiTexture=0;
		}

		cTextureAtlas::~cTextureAtlas()
		{

		}

		void cTextureAtlas::Begin(unsigned int uiNewSegmentWidthPX, unsigned int uiNewSegmentSmallPX, unsigned int uiNewAtlasWidthPX)
		{
			uiSegmentWidthPX=uiNewSegmentWidthPX;
			uiSegmentSmallPX=uiNewSegmentSmallPX;
			uiAtlasWidthPX=uiNewAtlasWidthPX;
			
			uiAtlasWidthNSegments=uiNewAtlasWidthPX/uiNewSegmentWidthPX;
			uiAtlasSegmentN=uiAtlasWidthNSegments*uiAtlasWidthNSegments;


			vSegment.insert(vSegment.begin(), uiAtlasSegmentN, false);

			
			Uint32 rmask, gmask, bmask, amask;

			/* SDL interprets each pixel as a 32-bit number, so our masks must depend
				on the endianness (byte order) of the machine */
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			rmask = 0xff000000;
			gmask = 0x00ff0000;
			bmask = 0x0000ff00;
			amask = 0x000000ff;
	#else
			rmask = 0x000000ff;
			gmask = 0x0000ff00;
			bmask = 0x00ff0000;
			amask = 0xff000000;
	#endif
			
			surface = SDL_CreateRGBSurface(SDL_SWSURFACE, uiAtlasWidthPX, uiAtlasWidthPX, 32, 
				rmask, gmask, bmask, amask);

			if (!surface)
				pLog->Error("TextureAtlas", "Couldn't Create Texture");
			else
				SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 0));
			

			glGenTextures(1, &uiTexture);
		}

		void cTextureAtlas::End()
		{
			glBindTexture(GL_TEXTURE_2D, uiTexture);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

			//Remove this line if there are artifacts
			gluBuild2DMipmaps(GL_TEXTURE_2D, 4, surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


			// clean up
			SDL_FreeSurface(surface);

			pLog->Success("Texture", "Atlas: %d", uiTexture);
		}

		void cTextureAtlas::BlitSurface(SDL_Surface *src, unsigned int x, unsigned int y)
		{
			int nH = src->h;
			int nSrcPitch = src->pitch;
			int nDstPitch = surface->pitch;
			int nPixel=nDstPitch/surface->w;
		
			unsigned char* pSrc = (unsigned char*) src->pixels;
			unsigned char* pDst = (unsigned char*) surface->pixels + y*nDstPitch+x*nPixel;
		
			unsigned int i=0;
			unsigned int w=src->w;
			
			while (nH--)
			{
				for(i=0;i<w;i++)
				{
					pDst[0]=pSrc[0];
					pDst[1]=pSrc[1];
					pDst[2]=pSrc[2];

					if(0==pSrc[0] && 0==pSrc[1] && 0==pSrc[2])
						pDst[3]=0;
					else
						pDst[3]=255;

					pDst+=4;
					pSrc+=4;
				}
				
				//std::memcpy(pDst, pSrc, nSrcPitch);

				pSrc += nSrcPitch-src->w*4;
				pDst += nDstPitch-src->w*4;
			};
		}
		
		cTexture *cTextureAtlas::AddTexture(std::string sFilename)
		{
			pLog->Success("Texture", "Loading %s", sFilename.c_str());

			sFilename=pFileSystem->FindFile(sFilename);
		
			unsigned int mode=0;
			SDL_Surface *tex = IMG_Load(sFilename.c_str());

			// could not load filename
			if (!tex)
			{
				pLog->Error("Texture", "Couldn't Load Texture %s", sFilename.c_str());
				return NULL;
			}

			if(tex->format->BytesPerPixel == 4)// RGBA 32bit
			{
				mode = GL_RGBA;
				pLog->Success("Texture", "RGBA Image");
			}
			else if(tex->format->BytesPerPixel == 3)
			{
				SDL_FreeSurface(tex);
				pLog->Error("Texture", "Image format must be RGBA not RGB");
				
				return NULL;
			}
			else
			{
				SDL_FreeSurface(tex);
				pLog->Error("Texture", "Image format must be RGBA not (%d) BPP", tex->format->BytesPerPixel);
				
				return NULL;
			}

			{
				int nHH = tex->h / 2;
				int nPitch = tex->pitch;
			
				unsigned char* pBuf = new unsigned char[nPitch];
				unsigned char* pSrc = (unsigned char*) tex->pixels;
				unsigned char* pDst = (unsigned char*) tex->pixels + nPitch*(tex->h - 1);
			
				while (nHH--)
				{
					std::memcpy(pBuf, pSrc, nPitch);
					std::memcpy(pSrc, pDst, nPitch);
					std::memcpy(pDst, pBuf, nPitch);
			
					pSrc += nPitch;
					pDst -= nPitch;
				};
			
				delete[] pBuf;
			}



			bool bFound=false;
			bool bGood=true;

			unsigned int rowY=0;
			unsigned int lineX=0;

			unsigned int countW=0;
			unsigned int countH=0;
			unsigned int requiredW=tex->w/uiSegmentWidthPX;
			unsigned int requiredH=tex->h/uiSegmentWidthPX;

			unsigned int foundX=0;
			unsigned int foundY=0;

			unsigned int uiOffset=0;
			
			cTexture *p=NULL;

			//if(uiSegmentWidthPX

			rowY=0;
			while(rowY<uiAtlasWidthNSegments && uiAtlasWidthNSegments-rowY>=requiredH && !bFound)
			{
				lineX=0;
				while(lineX<uiAtlasWidthNSegments && uiAtlasWidthNSegments-lineX>=requiredW && !bFound)
				{
					bGood=true;
					countH=0;
					uiOffset=rowY*uiAtlasWidthNSegments+lineX;
					while(countH<requiredH && bGood)
					{
						countW=0;
						while(countW<requiredW && bGood)
						{
							bGood=!vSegment[uiOffset + countH*uiAtlasWidthNSegments+countW];
							countW++;
						};

						countH++;
					};

					if(bGood)
					{
						bFound=true;
						foundX=lineX;
						foundY=rowY;

						for(countH=0;countH<requiredH;countH++)
							for(countW=0;countW<requiredW;countW++)
								vSegment[uiOffset + countH*uiAtlasWidthNSegments+countW]=true;
					}

					lineX++;
				};

				rowY++;
			};

			if(bFound)
			{
				p=new cTexture();
				p->fScale=static_cast<float>(tex->w)/static_cast<float>(uiAtlasWidthPX);
				p->sFilename=sFilename;
				p->uiTextureAtlas=uiID;
				p->uiTexture=uiTexture;
				p->fU=static_cast<float>(foundX)/static_cast<float>(uiAtlasWidthNSegments);
				p->fV=static_cast<float>(foundY)/static_cast<float>(uiAtlasWidthNSegments);

				BlitSurface(tex, foundX*uiSegmentWidthPX, foundY*uiSegmentWidthPX);

				pLog->Success("Texture Atlas", "Found position for texture %s (%dx%d) @ (%dx%d) (%dx%d)", 
					sFilename.c_str(), tex->w, tex->h, foundX*uiSegmentWidthPX, foundY*uiSegmentWidthPX, 
					uiAtlasWidthNSegments*uiSegmentWidthPX, uiAtlasWidthNSegments*uiSegmentWidthPX);
				pLog->Success("Texture", "%d", uiTexture);
			}
			else
			{
				pLog->Error("Texture Atlas", "Couldn't find position for texture %s (%dx%d) (%dx%d)", 
					sFilename.c_str(), requiredW, requiredH, 
					uiAtlasWidthNSegments*uiSegmentWidthPX, uiAtlasWidthNSegments*uiSegmentWidthPX);
			}

			SDL_FreeSurface(tex);

			return p;
		}
	}
}
