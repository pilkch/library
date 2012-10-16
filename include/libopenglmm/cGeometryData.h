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

#ifndef LIBOPENGLMM_CGEOMETRYDATA_H
#define LIBOPENGLMM_CGEOMETRYDATA_H

// Standard headers
#include <memory>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <libopenglmm/libopenglmm.h>

namespace opengl
{
  struct cGeometryData
  {
  public:
    cGeometryData();

    std::vector<float> vertices;
    std::vector<uint16_t> indices;

    size_t nVertexCount;

    size_t nVerticesPerPoint;
    size_t nNormalsPerPoint;
    size_t nColoursPerPoint;
    size_t nTextureCoordinatesPerPoint;
    size_t nFloatUserData0PerPoint;
    size_t nFloatUserData1PerPoint;
    size_t nFloatUserData2PerPoint;
  };

  typedef std::shared_ptr<cGeometryData> cGeometryDataPtr;


  inline cGeometryData::cGeometryData() :
    nVertexCount(0),
    nVerticesPerPoint(0),
    nNormalsPerPoint(0),
    nColoursPerPoint(0),
    nTextureCoordinatesPerPoint(0),
    nFloatUserData0PerPoint(0),
    nFloatUserData1PerPoint(0),
    nFloatUserData2PerPoint(0)
  {
  }


  inline cGeometryDataPtr CreateGeometryData()
  {
    return cGeometryDataPtr(new cGeometryData);
  }
}

#endif // LIBOPENGLMM_CGEOMETRYDATA_H
