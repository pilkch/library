/*************************************************************************
 *                                                                       *
 * libvoodoomm Library, Copyright (C) 2009 Onwards Chris Pilkington        *
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

// This is a simple wrapper around SDL_image to to load, save and apply basic effects to images

#ifndef LIBVOODOOMM_H
#define LIBVOODOOMM_H

struct SDL_Surface;

namespace voodoo
{
  #ifdef UNICODE
  typedef std::wstring string_t;
  #else
  typedef std::string string_t;
  #endif

  namespace string
  {
    std::string ToUTF8(const string_t& source);
  }

  enum class CUBE_MAP_FACE {
    POSITIVE_X,
    NEGATIVE_X,
    POSITIVE_Y,
    NEGATIVE_Y,
    POSITIVE_Z,
    NEGATIVE_Z
  };

  enum class PIXELFORMAT {
    R8G8B8A8,
    R8G8B8,
    R5G6B5,
    RGB16F, // RGB, 3 half float 16 bit channels
    RGB32F, // RGB, 3 float 32 bit channels
    H8,  // uint8_t heightmap
    H16, // uint16_t heightmap
    H32  // float heightmap, single float 32 bit channel
  };

  inline constexpr size_t GetBytesForPixelFormat(PIXELFORMAT pixelFormat)
  {
    if (pixelFormat == PIXELFORMAT::H8) return 1;
    else if (pixelFormat == PIXELFORMAT::H16) return 2;
    else if (pixelFormat == PIXELFORMAT::H32) return 4;
    else if (pixelFormat == PIXELFORMAT::R5G6B5) return 2;
    else if (pixelFormat == PIXELFORMAT::R8G8B8) return 3;
    else if (pixelFormat == PIXELFORMAT::R8G8B8A8) return 4;
    else if (pixelFormat == PIXELFORMAT::RGB16F) return 6;
    else if (pixelFormat == PIXELFORMAT::RGB32F) return 12;

    return 0;
  }

  inline constexpr size_t GetBitsForPixelFormat(PIXELFORMAT pixelFormat)
  {
    return 8 * GetBytesForPixelFormat(pixelFormat);
  }

  class cImage;
  class cSurface;
}

#endif // LIBVOODOOMM_H
