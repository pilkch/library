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

#ifndef LIBVOODOOMM_CIMAGE_H
#define LIBVOODOOMM_CIMAGE_H

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cColour.h>

#include <libvoodoomm/libvoodoomm.h>

struct SDL_Surface;

// Data flow
// cImage::pData <--> cImage::pSurface

namespace voodoo
{
  enum class IMAGE_TYPE {
    BITMAP,
    HEIGHTMAP
  };

  // ** cImage

  class cImage
  {
  public:
    cImage();
    virtual ~cImage();

    cImage(const cImage& rhs);
    cImage& operator=(const cImage& rhs);

    bool IsValid() const { return (pSurface != nullptr); }

    size_t GetWidth() const { return width; }
    size_t GetHeight() const { return height; }
    PIXELFORMAT GetPixelFormat() const { return pixelFormat; }
    size_t GetBytesPerPixel() const;
    IMAGE_TYPE GetType() const { return type; }

    void SetWidth(size_t _width) { width = _width; }
    void SetHeight(size_t _height) { height = _height; }
    void SetPixelFormat(PIXELFORMAT _pixelFormat) { pixelFormat = _pixelFormat; }
    void SetType(IMAGE_TYPE _type) { type = _type; }

    const uint8_t* GetPointerToData() const;
    const uint8_t* GetPointerToSurfacePixelBuffer() const;

    bool LoadFromFile(const string_t& sFilename);
    bool CreateFromBuffer(const uint8_t* pBuffer, size_t width, size_t height, PIXELFORMAT pixelFormat);

    void CopyFromDataToSurface();

    void CopyFromSurfaceToData(size_t width, size_t height);
    void CopyFromSurfaceToData();

    void CopyFromSurfaceToTexture();

    void FlipDataVertically();
    void FlipDataHorizontally();

    bool SaveToBMP(const string_t& sFilename) const;

  protected:
    size_t width;
    size_t height;

    PIXELFORMAT pixelFormat;

    IMAGE_TYPE type;

    SDL_Surface* pSurface;
    std::vector<unsigned char> data;

  private:
    void Assign(const cImage& rhs);
  };
}

#endif // LIBVOODOOMM_CIMAGE_H
