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
  class cTexture
  {
  public:
    cTexture();
    ~cTexture();

    bool IsValid() const;

    bool LoadFromFile(const std::string& sFileName);
  };
}

#endif // LIBOPENGLMM_CTEXTURE_H
