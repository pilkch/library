/*************************************************************************
 *                                                                       *
 * libvoodoomm Library, Copyright (C) 2009 Onwards Chris Pilkington      *
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

// This is a simple wrapper around SDL_image to load, save and apply basic effects to images

#ifndef LIBVOODOOMM_CIMAGE_H
#define LIBVOODOOMM_CIMAGE_H

// Standard headers
#include <vector>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>

#include <libvoodoomm/libvoodoomm.h>

// Data flow
//
// cImage::pSurface <--> cSurface::data
// OR
// cImage::buffer <--> cSurface::data <--> cImage::pSurface

namespace voodoo
{
  enum class IMAGE_TYPE {
    BITMAP,
    HEIGHTMAP
  };

  class cSurface;

  // ** cImage

  class cImage
  {
  public:
    friend class cSurface;

    cImage();

    cImage(const cImage& rhs);
    cImage& operator=(const cImage& rhs);

    static bool IsLoadingSupported(const string_t& sFilePath);
    bool LoadFromFile(const string_t& sFilePath);
    bool SaveToBMP(const string_t& sFilePath) const;
    bool SaveToPNG(const string_t& sFilePath) const;

    bool CreateEmptyImage(size_t width, size_t height, PIXELFORMAT pixelFormat);
    bool CreateFromImage(const cImage& image);
    bool CreateFromBuffer(const uint8_t* pBuffer, size_t width, size_t height, PIXELFORMAT pixelFormat);

    void CreateFromImageResizeNearestNeighbour(const cImage& image, size_t width, size_t height);
    void CreateFromImageHalfSize(const cImage& image); // Skips every second pixel
    void CreateFromImageDoubleSize(const cImage& image); // Copies each pixel twice
    void CreateFromImageAndSmooth(const cImage& image, size_t iterations);

    bool IsValid() const { return (width != 0) && (height != 0) && !buffer.empty(); }

    size_t GetWidth() const { return width; }
    size_t GetHeight() const { return height; }
    PIXELFORMAT GetPixelFormat() const { return pixelFormat; }
    size_t GetBytesPerPixel() const;
    size_t GetBitsPerPixel() const;
    size_t GetBytesPerRow() const;

    void SetWidth(size_t _width) { width = _width; }
    void SetHeight(size_t _height) { height = _height; }
    void SetPixelFormat(PIXELFORMAT _pixelFormat) { pixelFormat = _pixelFormat; }

    const uint8_t* GetPointerToBuffer() const;
    uint8_t* GetPointerToBuffer();

    void FillColour(uint8_t red, uint8_t green, uint8_t blue);
    void FillBlack();
    void FillWhite();
    void FillMagenta();
    void FillTestPattern();

    void ConvertToGreyScale();
    void ConvertToNegative();

    void FlipVertically();
    void FlipHorizontally();

  protected:
    size_t width;
    size_t height;
    PIXELFORMAT pixelFormat;

    std::vector<uint8_t> buffer;

  private:
    void Assign(const cImage& rhs);

    bool IsSameFormat(const cImage& rhs) const;
  };
}

#endif // LIBVOODOOMM_CIMAGE_H
