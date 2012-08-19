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

#ifndef LIBOPENGLMM_H
#define LIBOPENGLMM_H

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat3.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cColour.h>

// libvoodoomm headers
#include <libvoodoomm/cImage.h>

#define BUILD_LIBOPENGLMM_SDL_VERSION 120

#ifndef BUILD_LIBOPENGLMM_OPENGL_VERSION
#define BUILD_LIBOPENGLMM_OPENGL_VERSION 200
#endif

struct SDL_Surface;

namespace opengl
{
  typedef std::wstring string_t;

  namespace string
  {
    std::string ToUTF8(const string_t& source);
  }

  const size_t MAX_TEXTURE_UNITS = 3;
  const size_t MAX_TEXTURE_SIZE = 2048;
  const size_t MAX_LIGHTS = 8;

  enum class CUBE_MAP_FACE {
    POSITIVE_X,
    NEGATIVE_X,
    POSITIVE_Y,
    NEGATIVE_Y,
    POSITIVE_Z,
    NEGATIVE_Z
  };

  typedef voodoo::PIXELFORMAT PIXELFORMAT;

  using voodoo::GetBytesForPixelFormat;
  using voodoo::GetBitsForPixelFormat;

  class cResolution
  {
  public:
    cResolution();

    size_t width;
    size_t height;
    PIXELFORMAT pixelFormat;
  };

  class cCapabilities
  {
  public:
    cCapabilities() { Clear(); }

    void Clear();

    const cResolution& GetCurrentResolution() const { return currentResolution; }
    void SetCurrentResolution(const cResolution& resolution) { currentResolution = resolution; }
    const std::vector<cResolution>& GetResolutions() const { return resolutions; }
    void AddResolution(const cResolution& resolution) { resolutions.push_back(resolution); }

    bool bIsOpenGLTwoPointZeroOrLaterSupported;
    bool bIsOpenGLThreePointZeroOrLaterSupported;
    bool bIsShadersTwoPointZeroOrLaterSupported;
    bool bIsVertexBufferObjectSupported;
    bool bIsFrameBufferObjectSupported;
    bool bIsShadowsSupported;
    bool bIsCubemappingSupported;
    size_t nTextureUnits;
    size_t iMaxTextureSize;
    bool bIsFSAASupported;
    size_t nMaxFSAALevels;

  private:
    cResolution currentResolution;
    std::vector<cResolution> resolutions;
  };

  class cSystem;
  class cContext;
  class cWindow;

  class cWindowEventListener;
  class cInputEventListener;

  class cTexture;
  class cShader;
  class cStaticVertexBuffer;
  class cShader;
}

#endif // LIBOPENGLMM_H
