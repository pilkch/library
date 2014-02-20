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

#ifndef LIBOPENGLMM_CSYSTEM_H
#define LIBOPENGLMM_CSYSTEM_H

#include <libopenglmm/libopenglmm.h>

typedef unsigned int GLenum;
struct SDL_Surface;

#ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
#ifdef __WIN__
#pragma push_macro("CreateWindow")
#undef CreateWindow
#endif
#endif

namespace opengl
{
  class cContext;
  #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
  class cWindow;
  #endif

  class cSystem
  {
  public:
    cSystem();
    ~cSystem();

    static string_t GetErrorString(GLenum error);
    static string_t GetErrorString();

    bool FindExtension(const std::string& sExt) const;

    void UpdateResolutions();
    void UpdateCapabilities();
    const cCapabilities& GetCapabilities() const { return capabilities; }

    #ifdef BUILD_DEBUG
    // Debug windowed mode resolutions for testing various aspect ratios
    static void GetWindowedTestResolution4By3(size_t& width, size_t& height) { width = 1000; height = 750; }
    static void GetWindowedTestResolution16By10(size_t& width, size_t& height) { width = 1000; height = 624; }
    static void GetWindowedTestResolution16By9(size_t& width, size_t& height) { width = 1000; height = 562; }
    #endif

    #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
    cWindow* CreateWindow(const opengl::string_t& sCaption, const cResolution& resolution, bool bIsFullScreen);
    void DestroyWindow(cWindow* pWindow);

    cContext* CreateSharedContextFromWindow(const cWindow& window);
    #endif

    #ifdef BUILD_LIBOPENGLMM_WINDOW_GTKGLEXT
    cContext* CreateSharedContextForWidget(const cResolution& resolution);
    #endif

    cContext* CreateSharedContextFromContext(const cContext& context);
    void DestroyContext(cContext* pContext);

  private:
    std::string GetExtensions() const;
    float GetShaderVersion() const;

    bool IsGPUATI() const;
    bool IsGPUNVIDIA() const;
    size_t GetGPUMemoryTotalMB() const;
    size_t GetGPUMemoryUsedMB() const;

    cCapabilities capabilities;

    size_t nContexts;
  };
}

#endif // LIBOPENGLMM_CSYSTEM_H
