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

struct SDL_Surface;

namespace opengl
{
  class cContext;
  class cWindow;

  class cSystem
  {
  public:
    cSystem();
    ~cSystem();

    static std::string GetErrorString(GLenum error);
    static std::string GetErrorString();

    bool FindExtension(const std::string& sExt) const;

    void UpdateResolutions();
    void UpdateCapabilities();
    const cCapabilities& GetCapabilities() const { return capabilities; }

    cWindow* CreateWindow(const cResolution& resolution, bool bIsFullScreen);
    void DestroyWindow(cWindow* pWindow);

    cContext* CreateSharedContextFromWindow(const cWindow& window);
    cContext* CreateSharedContextFromContext(const cContext& context);
    void DestroyContext(cContext* pContext);

  private:
    float GetShaderVersion() const;

    cCapabilities capabilities;
  };
}

#endif // LIBOPENGLMM_CSYSTEM_H
