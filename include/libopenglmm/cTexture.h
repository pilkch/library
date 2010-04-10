/*************************************************************************
 *                                                                       *
 * libopenglmm Library, Copyright (C) 2009 Onwards Chris Pilkington         *
 * All rights reserved.  Web: http://chris.iluo.net                      *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2.1 of the  *
 * License, or (at your option) any later version. The text of the GNU   *
 * General Public License is included with this library in the           *
 * file license.txt.                                                     *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 * See the file GPL.txt for more details.                                *
 *                                                                       *
 *************************************************************************/

// This is a simple wrapper around OpenGL to make it a little bit more modern and easier to work with

#ifndef LIBOPENGLMM_CTEXTURE_H
#define LIBOPENGLMM_CTEXTURE_H

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cColour.h>

//#include <libopenglmm/cTexture.h>
//#include <libopenglmm/cShader.h>
//#include <libopenglmm/cVertexBufferObject.h>
//#include <libopenglmm/cWindow.h>

struct SDL_Surface;

namespace opengl
{
  enum class TEXTURE_TYPE {
    RGBA,
    HEIGHTMAP,
    FRAMEBUFFEROBJECT
  };

  // ** cTexture

  class cTexture
  {
  public:
    cTexture();
    virtual ~cTexture();

    bool IsValid() const { return _IsValid(); }

    size_t GetWidth() const { return uiWidth; }
    size_t GetHeight() const { return uiHeight; }

    void SetWidth(size_t _uiWidth) { uiWidth = _uiWidth; }
    void SetHeight(size_t _uiHeight) { uiHeight = _uiHeight; }

    bool Load(const std::string& sFilename);

    // pData <--> pSurface -> OpenGL texture
    void CopyFromDataToSurface();

    void CopyFromSurfaceToData(unsigned int w, unsigned int h);
    void CopyFromSurfaceToData();

    void CopyFromSurfaceToTexture();


    bool SaveToBMP(const std::string& sFilename) const;

    void Transform(float& u, float& v) const;

    void Create() { _Create(); }
    void Destroy();
    void Reload();


    unsigned int uiTextureAtlas;
    unsigned int uiTexture;
    std::string sFilename;

  protected:
    unsigned int uiWidth;
    unsigned int uiHeight;

  public:
    TEXTURE_TYPE uiType;

    float fScale;
    float fU;
    float fV;

    SDL_Surface* pSurface;
    std::vector<unsigned char> data;

  private:
    virtual bool _IsValid() const { return (uiTexture != 0); }
    virtual void _Create();
  };

  inline void cTexture::Transform(float& u, float& v) const
  {
    u = (u * fScale) + fU;
    v = (v * fScale) + fV;
  }


  // ** cTextureFrameBufferObject

  const size_t DEFAULT_FBO_TEXTURE_WIDTH = 1024;
  const size_t DEFAULT_FBO_TEXTURE_HEIGHT = 1024;

  class cTextureFrameBufferObject : public cTexture
  {
  public:
    cTextureFrameBufferObject();
    ~cTextureFrameBufferObject();

    bool IsModeCubeMap() const { return bIsCubeMap; }
    void SetModeCubeMap() { bIsCubeMap = true; }

    bool CreateFrameBufferObject(size_t width, size_t height);

    void GenerateMipMapsIfRequired();

    void SelectMipMapLevelOfDetail(float fLevelOfDetail);

    unsigned int uiFBO;            // Our handle to the FBO
    unsigned int uiFBODepthBuffer; // Our handle to the depth render buffer

  private:
    bool _IsValid() const { return (uiTexture != 0) && (uiFBO != 0) && (uiFBODepthBuffer != 0); }
    void _Create();

    bool bIsUsingMipMaps;
    bool bIsCubeMap;
  };
}

#endif // LIBOPENGLMM_CTEXTURE_H
