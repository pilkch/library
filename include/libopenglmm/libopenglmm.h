/*************************************************************************
 *                                                                       *
 * libopenglmm Library, Copyright (C) 2009 Onwards Chris Pilkington      *
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

// If no window method has been defined then default to SDL window creation
#if !defined(BUILD_LIBOPENGLMM_WINDOW_WIN32) && !defined(BUILD_LIBOPENGLMM_WINDOW_GTKGLEXT) && !defined(BUILD_LIBOPENGLMM_WINDOW_SDL)
#define BUILD_LIBOPENGLMM_WINDOW_SDL
#endif

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

#ifdef BUILD_OPENGLMM_FONT
#error "BUILD_OPENGLMM_FONT is deprecated, please use BUILD_LIBOPENGLMM_FONT instead"
#endif

#if !defined(BUILD_LIBOPENGLMM_WINDOW_SDL) && !defined(BUILD_LIBOPENGLMM_WINDOW_WIN32) && !defined(BUILD_LIBOPENGLMM_WINDOW_GTKGLEXT)
#error "BUILD_LIBOPENGLMM_WINDOW_SDL or BUILD_LIBOPENGLMM_WINDOW_WIN32 or BUILD_LIBOPENGLMM_WINDOW_GTKGLEXT must be defined"
#endif

#ifndef BUILD_LIBOPENGLMM_OPENGL_VERSION
#define BUILD_LIBOPENGLMM_OPENGL_VERSION 300
#endif

struct SDL_Surface;

namespace opengl
{
  #ifdef UNICODE
  typedef std::wstring string_t;
  typedef wchar_t char_t;
  #else
  typedef std::string string_t;
  typedef char char_t;
  #endif

  namespace string
  {
    std::string ToUTF8(const string_t& source);
  }

  const size_t MAX_TEXTURE_UNITS = 4;
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


  // ** cBufferSizes
  //
  // Provides a way of passing around sizes for the parts of a vertex buffer object easily

  class cBufferSizes {
  public:
    cBufferSizes();

    size_t nVertexCount;

    size_t nVerticesPerPoint;
    size_t nNormalsPerPoint;
    size_t nColoursPerPoint;
    size_t nTextureCoordinatesPerPoint;
  };

  inline cBufferSizes::cBufferSizes() :
    nVertexCount(0),
    nVerticesPerPoint(0),
    nNormalsPerPoint(0),
    nColoursPerPoint(0),
    nTextureCoordinatesPerPoint(0)
  {
  }


  class cSystem;
  class cContext;
  #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
  class cWindow;
  #endif

  #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
  class cWindowEventListener;
  class cInputEventListener;
  #endif

  class cTexture;
  class cShader;
  class cStaticVertexBufferObject;
  class cShader;
}

#endif // LIBOPENGLMM_H
