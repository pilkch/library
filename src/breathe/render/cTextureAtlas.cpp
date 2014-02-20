#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include <iostream>
#include <fstream>

#include <list>
#include <sstream>
#include <vector>
#include <map>
#include <string>

// Boost headers
#include <boost/shared_ptr.hpp>


// Anything else
//#include <GL/GLee.h>
//#include <GL/glu.h>

#include <SDL/SDL_image.h>


// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/string.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/geometry.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/render/cContext.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
//#include <breathe/render/cMaterial.h>
//#include <breathe/render/cResourceManager.h>

namespace breathe
{
  namespace render
  {
    // ** cTextureCoordinatesRectangle

    cTextureCoordinatesRectangle::cTextureCoordinatesRectangle()
    {
      textureCoordinates[0].Set(0.0f, 1.0f);
      textureCoordinates[1].Set(1.0f, 1.0f);
      textureCoordinates[2].Set(1.0f, 0.0f);
      textureCoordinates[3].Set(0.0f, 0.0f);
    }

    cTextureCoordinatesRectangle::cTextureCoordinatesRectangle(float fU, float fV, float fU2, float fV2)
    {
      textureCoordinates[0].Set(fU, fV2);
      textureCoordinates[1].Set(fU2, fV2);
      textureCoordinates[2].Set(fU2, fV);
      textureCoordinates[3].Set(fU, fV);
    }

    void cTextureCoordinatesRectangle::Rotate90DegreesClockWise()
    {
      const spitfire::math::cVec2 temp(textureCoordinates[0]);
      textureCoordinates[0] = textureCoordinates[1];
      textureCoordinates[1] = textureCoordinates[2];
      textureCoordinates[2] = textureCoordinates[3];
      textureCoordinates[3] = temp;
    }

    void cTextureCoordinatesRectangle::Rotate90DegreesCounterClockWise()
    {
      const spitfire::math::cVec2 temp(textureCoordinates[3]);
      textureCoordinates[3] = textureCoordinates[2];
      textureCoordinates[2] = textureCoordinates[1];
      textureCoordinates[1] = textureCoordinates[0];
      textureCoordinates[0] = temp;
    }

    void cTextureCoordinatesRectangle::Translate(const spitfire::math::cVec2& distance)
    {
      textureCoordinates[0] += distance;
      textureCoordinates[1] += distance;
      textureCoordinates[2] += distance;
      textureCoordinates[3] += distance;
    }

    void cTextureCoordinatesRectangle::Scale(const spitfire::math::cVec2& scale)
    {
      textureCoordinates[0] *= scale;
      textureCoordinates[1] *= scale;
      textureCoordinates[2] *= scale;
      textureCoordinates[3] *= scale;
    }


    // ** cTextureAtlasCustom

    cTextureAtlasCustom::cTextureAtlasCustom() :
      width(1),
      height(1)
    {
    }

    void cTextureAtlasCustom::Init(size_t _width, size_t _height)
    {
      width = _width;
      height = _height;
    }

    size_t cTextureAtlasCustom::AddRectangle(const spitfire::math::cRectangle& rectangle)
    {
      size_t i = textures.size();
      textures.push_back(rectangle);
      return i;
    }


    /*cTextureAtlas::cTextureAtlas(unsigned int id)
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
      int nDstPitch = pSurface->pitch;
      int nPixel = nDstPitch / pSurface->w;

      unsigned char* pSrc = (unsigned char*)src->pixels;
      unsigned char* pDst = (unsigned char*)pSurface->pixels + (y * nDstPitch) + (x * nPixel);

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

    cTextureRef cTextureAtlas::AddTexture(const string_t& sPath)
    {
      LOG.Success("Texture", "Loading " + breathe::string::ToUTF8(sPath));

      string_t sFilename;
      breathe::filesystem::FindResourceFile(sPath, sFilename);

      // Load the texture
      cTextureRef pTexture(new cTexture);

      if (!pTexture->Load(sFilename)) {
        pTexture.reset();
        return pResourceManager->pTextureNotFoundTexture;
      }

      // We have a valid texture, find a spot for it
      unsigned int uiTextureWidth = pTexture->pSurface->w;
      unsigned int uiTextureHeight = pTexture->pSurface->h;
      bool bFound = false;
      bool bGood = true;

      unsigned int rowY=0;
      unsigned int lineX=0;

      unsigned int countW=0;
      unsigned int countH=0;
      unsigned int requiredW=uiTextureWidth/uiSegmentWidthPX;
      unsigned int requiredH=uiTextureHeight/uiSegmentWidthPX;

      unsigned int foundX=0;
      unsigned int foundY=0;

      unsigned int uiOffset=0;

      rowY = 0;
      while (rowY<uiAtlasWidthNSegments && uiAtlasWidthNSegments-rowY>=requiredH && !bFound) {
        lineX = 0;
        while (lineX<uiAtlasWidthNSegments && uiAtlasWidthNSegments-lineX>=requiredW && !bFound) {
          // Set bGood to true
          bGood=true;
          countH=0;
          uiOffset=rowY*uiAtlasWidthNSegments+lineX;

          for (countH=0;countH<requiredH;countH++) {
            for (countW=0;countW<requiredW;countW++) {
              // bGood = true if this vSegment has not been filled yet
              bGood = (false == vSegment[uiOffset + countH*uiAtlasWidthNSegments+countW]);
              if (!bGood) break;
            }

            if (!bGood) break;
          }

          // If bGood is still true then we have found a valid position for this texture
          if (bGood) {
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
      if (!bFound) {
        std::ostringstream t;
        t << "Couldn't find position for texture " + breathe::string::ToUTF8(sFilename) + " (";
        t << uiAtlasWidthNSegments*uiSegmentWidthPX;
        t << "x";
        t << uiAtlasWidthNSegments*uiSegmentWidthPX;
        t << ")",
        LOG.Error("Texture Atlas", t.str());

        pTexture.reset();
        return pResourceManager->pTextureNotFoundTexture;
      }

      // Ok, we have found a spot to place this texture
      // Place it
      BlitSurface(pTexture->pSurface, foundX * uiSegmentWidthPX, foundY * uiSegmentWidthPX);

      // Now we delete the texture
      pTexture.reset();


      // Now we return a texture that points to the right spot in the texture atlas
      pTexture.reset(new cTexture);
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
      if (pSurface != nullptr) {
        LOG.Error("TextureAtlas", "Already has a surface");
        return;
      }

      uiSegmentWidthPX = uiNewSegmentWidthPX;
      uiSegmentSmallPX = uiNewSegmentSmallPX;

      uiWidth = uiHeight = uiNewAtlasWidthPX;

      uiAtlasWidthNSegments = uiNewAtlasWidthPX / uiNewSegmentWidthPX;
      uiAtlasSegmentN = uiAtlasWidthNSegments * uiAtlasWidthNSegments;


      vSegment.insert(vSegment.begin(), uiAtlasSegmentN, false);


      Uint32 rmask, gmask, bmask, amask;

      // SDL interprets each pixel as a 32-bit number, so our masks must depend on the endianness (byte order) of the machine
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

      pSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, uiWidth, uiHeight, 32, rmask, gmask, bmask, amask);

      if (!pSurface) LOG.Error("TextureAtlas", "Couldn't Create Texture");
      else SDL_FillRect(pSurface, NULL, SDL_MapRGB(pSurface->format, 255, 255, 0));

      Create();
    }

    void cTextureAtlas::End()
    {
      CopyFromSurfaceToData();
      CopyFromSurfaceToTexture();

      std::ostringstream t;
      t << uiTexture;
      LOG.Success("Texture", "Atlas uiTexture=" + t.str());
    }*/
  }
}

/*void TestGUI()
{
  //cTextureAtlasCustom guiTextureAtlas;

  //const size_t GUI_WINDOW_BACKGROUND              = 0;
  //const size_t GUI_WINDOW_BORDER_TOP_LEFT         = 1;
  //const size_t GUI_WINDOW_BORDER_TOP_RIGHT        = 2;
  //const size_t GUI_WINDOW_BORDER_BOTTOM_LEFT      = 3;
  //const size_t GUI_WINDOW_BORDER_BOTTOM_RIGHT     = 4;
  //const size_t GUI_WINDOW_DECORATION_CLOSE        = 5;
  //const size_t GUI_WINDOW_DECORATION_MINIMISE     = 6;
  //const size_t GUI_WINDOW_DECORATION_MAXIMISE     = 7;

  //guiTextureAtlas.Init(pWidgetsTexture->GetWidth(), pWidgetsTexture->GetHeight());
  //guiTextureAtlas.AddRectangle(spitfire::math::cRectangle(0.0f, 0.0f, 1.0f, 0.5f));
  //guiTextureAtlas.AddRectangle(spitfire::math::cRectangle(0.0f, 0.5f, 1.0f, 0.5f));
}*/

/*void Test()
{
  // Basic wrapping case
  breathe::render::cTextureMapper mapper;
  cVec2 a = mapper.GetTextureCoordinateFromPositionInPlaneSpace(spitfire::math::cVec2(0.5f, 0.5f));
  cVec2 b = mapper.GetTextureCoordinateFromPositionInPlaneSpace(spitfire::math::cVec2(1.0f, 1.0f));

  // Basic wrapping case
  breathe::render::cTextureMapper mapper;
  cVec2 a = mapper.GetTextureCoordinateFromPositionInPlaneSpace(spitfire::math::cVec2(0.5f, 0.5f));
  cVec2 b = mapper.GetTextureCoordinateFromPositionInPlaneSpace(spitfire::math::cVec2(10.0f, 10.0f));

  set texture wrap when rendering


  // We know the world position but not how many times to wrap
  breathe::render::cTextureMapperRealWorld mapper;
  mapper.SetTextureRealWorldDimensions(5.0f, 5.0f);
  cVec2 a = mapper.GetTextureCoordinateFromPositionInPlaneSpace(spitfire::math::cVec2(0.5f, 0.5f));
  cVec2 b = mapper.GetTextureCoordinateFromPositionInPlaneSpace(spitfire::math::cVec2(10.0f, 10.0f));

  set texture wrap when rendering


  // Basic case from atlas
  breathe::render::cTextureAtlas atlas;

  spitfire::math::cRectangle rectangle;
  rectangle.SetPosition(0.4f, 0.4f);
  rectangle.SetSize(0.2f, 0.2f);

  breathe::render::cTextureAtlasSubTexture texture;
  texture.SetRectangle(atlas, rectangle);

  cVec2 a = atlas.GetTextureCoordinatesFromPointInSubTexture(texture, spitfire::math::cVec2(0.5f, 0.5f));
  cVec2 b = atlas.GetTextureCoordinatesFromPointInSubTexture(texture, spitfire::math::cVec2(1.0f, 0.5f));

  Add a simple 3x3 texture that we can then "cut" pieces out of
  Put an icon in each one and a seamless wood grain texture at coordinate (2, 1)

  const size_t nSegmentsHorizontal = 3;
  const size_t nSegmentsVertical = 3;

  const float fRectangleWidth = 1.0f / nSegmentsHorizontal;
  const float fRectangleHeight = 1.0f / nSegmentsVertical;

  spitfire::math::cRectangle rectangles[9];
  for (size_t y = 0; y < nSegmentsHorizontal; y++) {
     for (size_t x = 0; x < nSegmentsVertical; x++) {
        rectangles[i].SetPosition(float(x) * fRectangleWidth, float(y) * fRectangleHeight);
        rectangles[i].SetSize(fRectangleWidth, fRectangleHeight);
     }
  }

  // The wood grain texture is at (2, 1)
  const spitfire::math::cRectangle& rectangleWoodGrain = rectangles[(1 * nSegmentsHorizontal) + 2)];

  breathe::render::cTextureAtlasSubTexture textureWoodGrain;
  textureWoodGrain.SetRectangle(atlas, rectangleWoodGrain);

  do not set texture wrap when rendering, to get a repeated texture we need to repeat geometry
  const spitfire::math::cVec2 textureCoord0 = atlas.GetTextureCoordinatesFromPointInSubTexture(textureWoodGrain, spitfire::math::cVec2(0.0f, 1.0f));
  const spitfire::math::cVec2 textureCoord1 = atlas.GetTextureCoordinatesFromPointInSubTexture(textureWoodGrain, spitfire::math::cVec2(1.0f, 1.0f));
  const spitfire::math::cVec2 textureCoord2 = atlas.GetTextureCoordinatesFromPointInSubTexture(textureWoodGrain, spitfire::math::cVec2(1.0f, 0.0f));
  const spitfire::math::cVec2 textureCoord3 = atlas.GetTextureCoordinatesFromPointInSubTexture(textureWoodGrain, spitfire::math::cVec2(0.0f, 0.0f));
  for (size_t i = 0; i < 5; i++) {
     x = float(i);
     ...
  }
}*/
