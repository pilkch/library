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

// liblibopenglmm headers
#include <libopenglmm/libopenglmm.h>
//#include <libopenglmm/cTexture.h>
//#include <libopenglmm/cShader.h>
//#include <libopenglmm/cVertexBufferObject.h>
//#include <libopenglmm/cWindow.h>

// Data flow
// cImage::buffer <--> cTexture::uiTexture OpenGL texture

// TODO: SDL 1.3 There is now a texture class, can this be used instead of uiTexture?
// http://wiki.libsdl.org/moin.cgi/MigrationGuide#Hardware_surfaces

namespace opengl
{
  // ** cTexture

  class cTexture
  {
  public:
    cTexture();
    virtual ~cTexture();

    bool IsValid() const { return _IsValid(); }

    bool IsRectangle() const { return (GetWidth() != GetHeight()); }

    size_t GetWidth() const { return image.GetWidth(); }
    size_t GetHeight() const { return image.GetHeight(); }

    unsigned int GetTexture() const { return uiTexture; }

    void SetDoNotUseMipMaps() { bIsUsingMipMaps = false; }

    bool CreateFromImage(const voodoo::cImage& image);

    void CopyFromImageToTexture();


    bool SaveToBMP(const opengl::string_t& sFilename) const { return image.SaveToBMP(sFilename); }

    void Create() { _Create(); }
    void Destroy() { _Destroy(); }
    void Reload();

  protected:
    voodoo::cImage image;

    unsigned int uiTexture;

    bool bIsUsingMipMaps;

  private:
    size_t GetBytesPerPixel() const { return image.GetBytesPerPixel(); }

    virtual bool _IsValid() const { return (uiTexture != 0); }
    virtual void _Create();
    virtual void _Destroy();
  };


  // ** cTextureCubeMap

  class cTextureCubeMap
  {
  public:
    cTextureCubeMap();
    ~cTextureCubeMap();

    bool IsValid() const { return (uiTexture != 0); }

    unsigned int GetTexture() const { return uiTexture; }

    bool CreateFromFilePaths(
      const opengl::string_t& filePathPositiveX,
      const opengl::string_t& filePathNegativeX,
      const opengl::string_t& filePathPositiveY,
      const opengl::string_t& filePathNegativeY,
      const opengl::string_t& filePathPositiveZ,
      const opengl::string_t& filePathNegativeZ
    );
    void Destroy();

  protected:
    unsigned int uiTexture;
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

    bool CreateFrameBufferObject(size_t width, size_t height, bool bColourBuffer, bool bDepthBuffer, bool bDepthShadow);

    void GenerateMipMapsIfRequired();

    void SelectMipMapLevelOfDetail(float fLevelOfDetail);

    bool IsDepthTexturePresent() const;
    unsigned int GetDepthTexture() const;

    unsigned int uiFBO;            // Our handle to the FBO
    unsigned int uiFBODepthBuffer; // Our handle to the depth render buffer

  private:
    bool _IsValid() const { return ((uiTexture != 0) && (uiFBO != 0)) || ((uiDepthTexture != 0) && (uiFBODepthBuffer != 0)); }
    void Create(bool bColourBuffer, bool bDepthBuffer, bool bDepthShadow);
    void _Destroy();

    unsigned int uiDepthTexture;

    bool bIsCubeMap;
  };
}

#endif // LIBOPENGLMM_CTEXTURE_H
