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

#include <libopenglmm/libopenglmm.h>
//#include <libopenglmm/cTexture.h>
//#include <libopenglmm/cShader.h>
//#include <libopenglmm/cVertexBufferObject.h>
//#include <libopenglmm/cWindow.h>

struct SDL_Surface;

// Data flow
// cImage::pData <--> cImage::pSurface -> cTexture::uiTexture OpenGL texture

namespace opengl
{
  enum class TEXTURE_TYPE {
    RGBA,
    HEIGHTMAP,
    FRAMEBUFFEROBJECT
  };

  // ** cImage

  class cTexture;
  class cTextureFrameBufferObject;

  class cImage
  {
  public:
    friend class cTexture;
    friend class cTextureFrameBufferObject;

    cImage();
    virtual ~cImage();

    cImage(const cImage& rhs);
    cImage& operator=(const cImage& rhs);

    bool IsValid() const { return (pSurface != nullptr); }

    size_t GetWidth() const { return uiWidth; }
    size_t GetHeight() const { return uiHeight; }

    void SetWidth(size_t _uiWidth) { uiWidth = _uiWidth; }
    void SetHeight(size_t _uiHeight) { uiHeight = _uiHeight; }

    bool LoadFromFile(const std::string& sFilename);
    bool CreateFromBuffer(const uint8_t* pBuffer, size_t width, size_t height, PIXELFORMAT pixelFormat);

    void CopyFromDataToSurface();

    void CopyFromSurfaceToData(size_t width, size_t height);
    void CopyFromSurfaceToData();

    void CopyFromSurfaceToTexture();


    bool SaveToBMP(const std::string& sFilename) const;

  protected:
    size_t uiWidth;
    size_t uiHeight;

    TEXTURE_TYPE uiType;

    SDL_Surface* pSurface;
    std::vector<unsigned char> data;

  private:
    void Assign(const cImage& rhs);

    size_t GetBytesPerPixel() const;
  };

  // ** cTexture

  class cTexture
  {
  public:
    cTexture();
    virtual ~cTexture();

    bool IsValid() const { return _IsValid(); }

    size_t GetWidth() const { return image.GetWidth(); }
    size_t GetHeight() const { return image.GetHeight(); }

    unsigned int GetTexture() const { return uiTexture; }

    bool CreateFromImage(const cImage& image);

    void CopyFromDataToSurface() { image.CopyFromDataToSurface(); }

    void CopyFromSurfaceToData(size_t width, size_t height) { image.CopyFromSurfaceToData(width, height); }
    void CopyFromSurfaceToData() { image.CopyFromSurfaceToData(); }

    void CopyFromSurfaceToTexture();


    bool SaveToBMP(const std::string& sFilename) const { return image.SaveToBMP(sFilename); }

    void Create() { _Create(); }
    void Destroy() { _Destroy(); }
    void Reload();

  protected:
    cImage image;

    unsigned int uiTexture;

  private:
    size_t GetBytesPerPixel() const { return image.GetBytesPerPixel(); }

    virtual bool _IsValid() const { return (uiTexture != 0); }
    virtual void _Create();
    virtual void _Destroy();
  };


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
    void _Destroy();

    bool bIsUsingMipMaps;
    bool bIsCubeMap;
  };
}

#endif // LIBOPENGLMM_CTEXTURE_H
