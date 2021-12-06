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

#pragma once

// Standard headers
#include <vector>

// Spitfire headers
#include <spitfire/spitfire.h>

// libvoodoomm headers
#include <libvoodoomm/libvoodoomm.h>

namespace voodoo
{
  enum class DDS_IMAGE_TYPE {
    RGB,
    DXT1,
    DXT3,
    DXT5,
    UNKNOWN,
  };

  bool read_dds(const std::vector<uint8_t>& data, DDS_IMAGE_TYPE& out_dds_type, cImage& out_image);
}
