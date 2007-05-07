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

			uiWidth=128;
			uiHeight=128;
			
			uiAtlasWidthNSegments=1;
			uiAtlasSegmentN=1;

			uiTexture=0;
		}

		cTextureAtlas::~cTextureAtlas()
		{

		}

		void cTextureAtlas::Begin(unsigned int uiNewSegmentWidthPX, unsigned int uiNewSegmentSmallPX, unsigned int uiNewAtlasWidthPX)
		{
			if(surface)
			{
				LOG.Error("TextureAtlas", "Already has a surface");
				return;
			}

			uiSegmentWidthPX=uiNewSegmentWidthPX;
			uiSegmentSmallPX=uiNewSegmentSmallPX;
			
			uiWidth=uiHeight=uiNewAtlasWidthPX;
			
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
			
			surface = SDL_CreateRGBSurface(SDL_SWSURFACE, uiWidth, uiHeight, 32, 
				rmask, gmask, bmask, amask);

			if (!surface)
				LOG.Error("TextureAtlas", "Couldn't Create Texture");
			else
				SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 0));
			
			glGenTextures(1, &uiTexture);
		}

		void cTextureAtlas::End()
		{
			CopyFromSurface();

			glBindTexture(GL_TEXTURE_2D, uiTexture);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

			//Remove this line if there are artifacts
			gluBuild2DMipmaps(GL_TEXTURE_2D, 4, surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			std::ostringstream t;
			t << uiTexture;
			LOG.Success("Texture", "Atlas: " + t.str());
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
			LOG.Success("Texture", "Loading " + sFilename);

			sFilename=pFileSystem->FindFile(sFilename);
		
			unsigned int mode=0;
			SDL_Surface *tex = IMG_Load(sFilename.c_str());

			// could not load filename
			if (!tex)
			{
				LOG.Error("Texture", "Couldn't Load Texture " + sFilename);
				return NULL;
			}

			if(tex->format->BytesPerPixel == 4)// RGBA 32bit
			{
				mode = GL_RGBA;
				LOG.Success("Texture", "RGBA Image");
			}
			else if(tex->format->BytesPerPixel == 3)
			{
				SDL_FreeSurface(tex);
				LOG.Error("Texture", "Image format must be RGBA not RGB");
				
				return NULL;
			}
			else
			{
				SDL_FreeSurface(tex);

				std::ostringstream t;
				t << "Image format must be RGBA not (";
				t << tex->format->BytesPerPixel;
				t << ") BPP";
				LOG.Error("Texture", t.str());
				
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
			
				SAFE_DELETE_ARRAY(pBuf);
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
				p->fScale=static_cast<float>(tex->w)/static_cast<float>(uiWidth);
				p->sFilename=sFilename;
				p->uiTextureAtlas=uiID;
				p->uiTexture=uiTexture;
				p->fU=static_cast<float>(foundX)/static_cast<float>(uiAtlasWidthNSegments);
				p->fV=static_cast<float>(foundY)/static_cast<float>(uiAtlasWidthNSegments);

				BlitSurface(tex, foundX*uiSegmentWidthPX, foundY*uiSegmentWidthPX);

				std::ostringstream t;
				t << "Found position for texture ";
				t << sFilename;
				t << "(";
				t << tex->w;
				t << "x";
				t << tex->h;
				t << ") @ (";
				t << foundX*uiSegmentWidthPX;
				t << "x";
				t << foundY*uiSegmentWidthPX;
				t << ") (";
				t << uiAtlasWidthNSegments*uiSegmentWidthPX;
				t << "x";
				t << uiAtlasWidthNSegments*uiSegmentWidthPX;
				t << ")";
				LOG.Success("Texture Atlas", t.str());
				
				t.str("");
				t << uiTexture;
				LOG.Success("Texture", t.str());
			}
			else
			{
				std::ostringstream t;
				t << "Couldn't find position for texture " + sFilename + " (";
				t << uiAtlasWidthNSegments*uiSegmentWidthPX;
				t << "x";
				t << uiAtlasWidthNSegments*uiSegmentWidthPX;
				t << ")",
				LOG.Error("Texture Atlas", t.str());
			}

			SDL_FreeSurface(tex);

			return p;
		}
	}
}
