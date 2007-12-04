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


// Anything else
#include <GL/Glee.h>
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

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>

namespace breathe
{	
	namespace render
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
				for (i=0;i<w;i++)
				{
					pDst[0]=pSrc[0];
					pDst[1]=pSrc[1];
					pDst[2]=pSrc[2];

					if (0==pSrc[0] && 0==pSrc[1] && 0==pSrc[2])
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
		
		cTexture *cTextureAtlas::AddTexture(const std::string& sPath)
		{
			LOG.Success("Texture", "Loading " + sFilename);

			const string_t sFilename = breathe::filesystem::FindFile(breathe::string::ToString_t(sPath));
		
			// Load the texture
			cTexture* pTexture = new cTexture();
			
			if (pTexture->Load(breathe::string::ToUTF8(sFilename)) != breathe::GOOD)
			{
				SAFE_DELETE(pTexture);
				return pRender->pTextureNotFoundTexture;
			}

			// We have a valid texture, find a spot for it
			unsigned int uiTextureWidth = pTexture->surface->w;
			unsigned int uiTextureHeight = pTexture->surface->h;
			bool bFound=false;
			bool bGood=true;

			unsigned int rowY=0;
			unsigned int lineX=0;

			unsigned int countW=0;
			unsigned int countH=0;
			unsigned int requiredW=uiTextureWidth/uiSegmentWidthPX;
			unsigned int requiredH=uiTextureHeight/uiSegmentWidthPX;

			unsigned int foundX=0;
			unsigned int foundY=0;

			unsigned int uiOffset=0;
			
			rowY=0;
			while(rowY<uiAtlasWidthNSegments && uiAtlasWidthNSegments-rowY>=requiredH && !bFound)
			{
				lineX=0;
				while(lineX<uiAtlasWidthNSegments && uiAtlasWidthNSegments-lineX>=requiredW && !bFound)
				{
					// Set bGood to true
					bGood=true;
					countH=0;
					uiOffset=rowY*uiAtlasWidthNSegments+lineX;

					for (countH=0;countH<requiredH;countH++)
					{
						for (countW=0;countW<requiredW;countW++)
						{
							// bGood = true if this vSegment has not been filled yet
							bGood = (false == vSegment[uiOffset + countH*uiAtlasWidthNSegments+countW]);
							if (!bGood)
								break;
						}

						if (!bGood)
							break;
					}

					// If bGood is still true then we have found a valid position for this texture
					if (bGood)
					{
						bFound=true;
						foundX=lineX;
						foundY=rowY;

						for (countH=0;countH<requiredH;countH++)
							for (countW=0;countW<requiredW;countW++)
								vSegment[uiOffset + countH*uiAtlasWidthNSegments+countW]=true;
					}

					lineX++;
				};

				rowY++;
			};

			// If we haven't found a spot, return
			if (!bFound)
			{
				std::ostringstream t;
				t << "Couldn't find position for texture " + breathe::string::ToUTF8(sFilename) + " (";
				t << uiAtlasWidthNSegments*uiSegmentWidthPX;
				t << "x";
				t << uiAtlasWidthNSegments*uiSegmentWidthPX;
				t << ")",
				LOG.Error("Texture Atlas", t.str());

				SAFE_DELETE(pTexture);
				return pRender->pTextureNotFoundTexture;
			}

			// Ok, we have found a spot to place this texture
			// Place it
			BlitSurface(pTexture->surface, foundX*uiSegmentWidthPX, foundY*uiSegmentWidthPX);

			// Now we delete the texture
			SAFE_DELETE(pTexture);


			// Now we return a texture that points to the right spot in the texture atlas
			pTexture=new cTexture();
			pTexture->fScale=static_cast<float>(uiTextureWidth)/static_cast<float>(uiWidth);
			pTexture->sFilename=this->sFilename;
			pTexture->uiTexture=this->uiTexture;
			pTexture->uiTextureAtlas=this->uiID;
			pTexture->fU=static_cast<float>(foundX)/static_cast<float>(uiAtlasWidthNSegments);
			pTexture->fV=static_cast<float>(foundY)/static_cast<float>(uiAtlasWidthNSegments);

			std::ostringstream t;
			t << "Found position for texture ";
			t << breathe::string::ToUTF8(sFilename);
			t << "(";
			t << uiTextureWidth;
			t << "x";
			t << uiTextureHeight;
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

			return pTexture;
		}

		void cTextureAtlas::Begin(unsigned int uiNewSegmentWidthPX, unsigned int uiNewSegmentSmallPX, unsigned int uiNewAtlasWidthPX)
		{
			if (surface)
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
			
			Create();
		}

		void cTextureAtlas::End()
		{
			CopyFromSurfaceToData();
			CopyFromSurfaceToTexture();

			std::ostringstream t;
			t << uiTexture;
			LOG.Success("Texture", "Atlas uiTexture=" + t.str());
		}
	}
}
