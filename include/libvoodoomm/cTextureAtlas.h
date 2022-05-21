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

#pragma once

// Standard headers
#include <vector>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>

#include <libvoodoomm/cImage.h>

// TODO: Add a 1 pixel border around every item
//
// TODO: Use a better packing algorithm
// https://stackoverflow.com/questions/1213394/what-algorithm-can-be-used-for-packing-rectangles-of-different-sizes-into-the-sm
// https://www.csc.liv.ac.uk/~epa/surveyhtml.html
// https://blackpawn.com/texts/lightmaps/
//
// TODO: Split up the rectangle packing from the texture mapping? The packing could be moved into spitfire::math
// std::vector<rect> rects;
// rects.push_back...
// spitfire::math::cVec2 minimumSize(0.0f, 0.0f);
// spitfire::math::PackRectsFindArea(rects, minimumSize);
//
// voodoo::cTextureAtlas textureAtlas;
// textureAtlas.CreateWidthRectsAndImages(minimumSize, rects, images);

namespace voodoo
{

class cTextureAtlasEntry {
public:
  cTextureAtlasEntry();

  size_t x;
  size_t y;
  size_t width;
  size_t height;
};


class cTextureAtlas {
public:
  cTextureAtlas();

  void Init(size_t width, size_t height, voodoo::PIXELFORMAT pixelFormat);

  bool AddImage(const std::string& textureFilePath);

  size_t GetWidth() const { return textureAtlas.GetWidth(); }
  size_t GetHeight() const { return textureAtlas.GetHeight(); }

  const voodoo::cImage& GetImage() const { return textureAtlas; }

  const std::vector<cTextureAtlasEntry>& GetTextureAtlasEntries() const { return textureAtlasEntries; }

  voodoo::cImage textureAtlas;

  std::vector<cTextureAtlasEntry> textureAtlasEntries;

  size_t currentX;
  size_t currentY;
  size_t currentTallestInRow;
};

}
