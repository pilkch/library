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

// This is a simple wrapper to load and save 32 bit floating point images

#ifndef LIBVOODOOMM_CIMAGEFLOAT_H
#define LIBVOODOOMM_CIMAGEFLOAT_H

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>

#include <libvoodoomm/libvoodoomm.h>

namespace voodoo
{
  // ** cImageFloat

  class cImageFloat
  {
  public:
    friend class cSurface;

    cImageFloat();
    ~cImageFloat();

    cImageFloat(const cImageFloat& rhs);
    cImageFloat& operator=(const cImageFloat& rhs);

    static bool IsLoadingSupported(const string_t& sFilePath);
    bool LoadFromFile(const string_t& sFilePath);
    bool SaveToPFM(const string_t& sFilePath) const;

    bool IsValid() const { return (width != 0) && (height != 0) && !buffer.empty(); }

    size_t GetWidth() const { return width; }
    size_t GetHeight() const { return height; }
    PIXELFORMAT GetPixelFormat() const { return pixelFormat; }
    size_t GetBytesPerPixel() const;

    const float* GetPointerToBuffer() const;

  protected:
    size_t width;
    size_t height;
    PIXELFORMAT pixelFormat;

    std::vector<float> buffer;

  private:
    void Assign(const cImageFloat& rhs);

    bool IsSameFormat(const cImageFloat& rhs) const;
  };
}

#endif // LIBVOODOOMM_CIMAGEFLOAT_H
