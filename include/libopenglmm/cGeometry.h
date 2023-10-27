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

#ifndef LIBOPENGLMM_CGEOMETRY_H
#define LIBOPENGLMM_CGEOMETRY_H

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cColour.h>

#include <libopenglmm/libopenglmm.h>
#include <libopenglmm/cGeometryData.h>

namespace opengl
{
  class cGeometryBuilder_v2
  {
  public:
    explicit cGeometryBuilder_v2(cGeometryData& data);

    void PushBack(const spitfire::math::cVec2& vertex);

  private:
    cGeometryData& data;
  };

  class cGeometryBuilder_v2_c4
  {
  public:
    explicit cGeometryBuilder_v2_c4(cGeometryData& data);

    void PushBack(const spitfire::math::cVec2& vertex, const spitfire::math::cColour& colour);

  private:
    cGeometryData& data;
  };

  class cGeometryBuilder_v2_t2
  {
  public:
    explicit cGeometryBuilder_v2_t2(cGeometryData& data);

    void PushBack(const spitfire::math::cVec2& vertex, const spitfire::math::cVec2& textureCoord0);

  private:
    cGeometryData& data;
  };

  class cGeometryBuilder_v2_t2_t2
  {
  public:
    explicit cGeometryBuilder_v2_t2_t2(cGeometryData& data);

    void PushBack(const spitfire::math::cVec2& vertex, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec2& textureCoord1);
    void PushBack(const spitfire::math::cVec2& vertex, const spitfire::math::cVec2& textureCoordDuplicated);

  private:
    cGeometryData& data;
  };

  class cGeometryBuilder_v2_t2_c4
  {
  public:
    explicit cGeometryBuilder_v2_t2_c4(cGeometryData& data);

    void PushBack(const spitfire::math::cVec2& vertex, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cColour4& colour);

  private:
    cGeometryData& data;
  };


  class cGeometryBuilder_v3_n3
  {
  public:
    explicit cGeometryBuilder_v3_n3(cGeometryData& data);

    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal);
    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& ignoredTextureCoord0) { (void)ignoredTextureCoord0; PushBack(vertex, normal); }

  private:
    cGeometryData& data;
  };

  class cGeometryBuilder_v3_n3_i
  {
  public:
    explicit cGeometryBuilder_v3_n3_i(cGeometryData& data);

    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal);
    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& ignoredTextureCoord0) { (void)ignoredTextureCoord0; PushBack(vertex, normal); }

    void PushBackIndex(uint16_t index);

  private:
    cGeometryData& data;
  };

  class cGeometryBuilder_v3_n3_c4
  {
  public:
    explicit cGeometryBuilder_v3_n3_c4(cGeometryData& data);

    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cColour& colour);
    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& ignoredTextureCoord0, const spitfire::math::cColour& colour) { (void)ignoredTextureCoord0; PushBack(vertex, normal, colour); }

  private:
    cGeometryData& data;
  };

  class cGeometryBuilder_v3_n3_t2
  {
  public:
    explicit cGeometryBuilder_v3_n3_t2(cGeometryData& data);

    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0);

  private:
    cGeometryData& data;
  };

  class cGeometryBuilder_v3_n3_t2_i
  {
  public:
    explicit cGeometryBuilder_v3_n3_t2_i(cGeometryData& data);

    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0);

    void PushBackIndex(uint16_t index);

  private:
    cGeometryData& data;
  };

  class cGeometryBuilder_v3_n3_t2_c4
  {
  public:
    explicit cGeometryBuilder_v3_n3_t2_c4(cGeometryData& data);

    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cColour4& colour);

  private:
    cGeometryData& data;
  };

  class cGeometryBuilder_v3_n3_t2_t2
  {
  public:
    explicit cGeometryBuilder_v3_n3_t2_t2(cGeometryData& data);

    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec2& textureCoord1);
    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoordDuplicated);

  private:
    cGeometryData& data;
  };

  class cGeometryBuilder_v3_n3_t2_t2_i
  {
  public:
    cGeometryBuilder_v3_n3_t2_t2_i(cGeometryData& data);

    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec2& textureCoord1);
    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoordDuplicated);

    void PushBackIndex(uint16_t index);

  private:
    cGeometryData& data;
  };

  class cGeometryBuilder_v3_n3_t2_t2_t2
  {
  public:
    explicit cGeometryBuilder_v3_n3_t2_t2_t2(cGeometryData& data);

    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec2& textureCoord1, const spitfire::math::cVec2& textureCoord2);
    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoordDuplicated);

  private:
    cGeometryData& data;
  };

  class cGeometryBuilder_v3_n3_t2_t2_t2_i
  {
  public:
    cGeometryBuilder_v3_n3_t2_t2_t2_i(cGeometryData& data);

    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec2& textureCoord1, const spitfire::math::cVec2& textureCoord2);
    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoordDuplicated);

    void PushBackIndex(uint16_t index);

  private:
    cGeometryData& data;
  };


  class cGeometryBuilder_v3_n3_t2_user3
  {
  public:
    explicit cGeometryBuilder_v3_n3_t2_user3(cGeometryData& data);

    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec3& user0);

  private:
    cGeometryData& data;
  };



  inline cGeometryBuilder_v2::cGeometryBuilder_v2(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 2;
  }

  inline void cGeometryBuilder_v2::PushBack(const spitfire::math::cVec2& vertex)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.nVertexCount++;
  }


  inline cGeometryBuilder_v2_c4::cGeometryBuilder_v2_c4(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 2;
    data.nColoursPerPoint = 4;
  }

  inline void cGeometryBuilder_v2_c4::PushBack(const spitfire::math::cVec2& vertex, const spitfire::math::cColour& colour)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(colour.r);
    data.vertices.push_back(colour.g);
    data.vertices.push_back(colour.b);
    data.vertices.push_back(colour.a);
    data.nVertexCount++;
  }


  inline cGeometryBuilder_v2_t2::cGeometryBuilder_v2_t2(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 2;
    data.nTextureCoordinatesPerPoint = 2;
  }

  inline void cGeometryBuilder_v2_t2::PushBack(const spitfire::math::cVec2& vertex, const spitfire::math::cVec2& textureCoord0)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(textureCoord0.x);
    data.vertices.push_back(textureCoord0.y);
    data.nVertexCount++;
  }


  inline cGeometryBuilder_v2_t2_t2::cGeometryBuilder_v2_t2_t2(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 2;
    data.nTextureCoordinatesPerPoint = 4;
  }

  inline void cGeometryBuilder_v2_t2_t2::PushBack(const spitfire::math::cVec2& vertex, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec2& textureCoord1)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(textureCoord0.x);
    data.vertices.push_back(textureCoord0.y);
    data.vertices.push_back(textureCoord1.x);
    data.vertices.push_back(textureCoord1.y);
    data.nVertexCount++;
  }

  inline void cGeometryBuilder_v2_t2_t2::PushBack(const spitfire::math::cVec2& vertex, const spitfire::math::cVec2& textureCoordDuplicated)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(textureCoordDuplicated.x);
    data.vertices.push_back(textureCoordDuplicated.y);
    data.vertices.push_back(textureCoordDuplicated.x);
    data.vertices.push_back(textureCoordDuplicated.y);
    data.nVertexCount++;
  }


  inline cGeometryBuilder_v2_t2_c4::cGeometryBuilder_v2_t2_c4(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 2;
    data.nTextureCoordinatesPerPoint = 2;
    data.nColoursPerPoint = 4;
  }

  inline void cGeometryBuilder_v2_t2_c4::PushBack(const spitfire::math::cVec2& vertex, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cColour& colour)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(textureCoord0.x);
    data.vertices.push_back(textureCoord0.y);
    data.vertices.push_back(colour.r);
    data.vertices.push_back(colour.g);
    data.vertices.push_back(colour.b);
    data.vertices.push_back(colour.a);
    data.nVertexCount++;
  }


  inline cGeometryBuilder_v3_n3::cGeometryBuilder_v3_n3(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 3;
    data.nNormalsPerPoint = 3;
  }

  inline void cGeometryBuilder_v3_n3::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(vertex.z);
    data.vertices.push_back(normal.x);
    data.vertices.push_back(normal.y);
    data.vertices.push_back(normal.z);
    data.nVertexCount++;
  }


  inline cGeometryBuilder_v3_n3_i::cGeometryBuilder_v3_n3_i(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 3;
    data.nNormalsPerPoint = 3;
  }

  inline void cGeometryBuilder_v3_n3_i::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(vertex.z);
    data.vertices.push_back(normal.x);
    data.vertices.push_back(normal.y);
    data.vertices.push_back(normal.z);
    data.nVertexCount++;
  }

  inline void cGeometryBuilder_v3_n3_i::PushBackIndex(uint16_t index)
  {
    data.indices.push_back(index);
  }


  inline cGeometryBuilder_v3_n3_c4::cGeometryBuilder_v3_n3_c4(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 3;
    data.nNormalsPerPoint = 3;
    data.nColoursPerPoint = 4;
  }

  inline void cGeometryBuilder_v3_n3_c4::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cColour& colour)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(vertex.z);
    data.vertices.push_back(normal.x);
    data.vertices.push_back(normal.y);
    data.vertices.push_back(normal.z);
    data.vertices.push_back(colour.r);
    data.vertices.push_back(colour.g);
    data.vertices.push_back(colour.b);
    data.vertices.push_back(colour.a);
    data.nVertexCount++;
  }

  inline cGeometryBuilder_v3_n3_t2::cGeometryBuilder_v3_n3_t2(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 3;
    data.nNormalsPerPoint = 3;
    data.nTextureCoordinatesPerPoint = 2;
  }

  inline void cGeometryBuilder_v3_n3_t2::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(vertex.z);
    data.vertices.push_back(normal.x);
    data.vertices.push_back(normal.y);
    data.vertices.push_back(normal.z);
    data.vertices.push_back(textureCoord0.x);
    data.vertices.push_back(textureCoord0.y);
    data.nVertexCount++;
  }


  inline cGeometryBuilder_v3_n3_t2_i::cGeometryBuilder_v3_n3_t2_i(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 3;
    data.nNormalsPerPoint = 3;
    data.nTextureCoordinatesPerPoint = 2;
  }

  inline void cGeometryBuilder_v3_n3_t2_i::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(vertex.z);
    data.vertices.push_back(normal.x);
    data.vertices.push_back(normal.y);
    data.vertices.push_back(normal.z);
    data.vertices.push_back(textureCoord0.x);
    data.vertices.push_back(textureCoord0.y);
    data.nVertexCount++;
  }

  inline void cGeometryBuilder_v3_n3_t2_i::PushBackIndex(uint16_t index)
  {
    data.indices.push_back(index);
  }


  inline cGeometryBuilder_v3_n3_t2_c4::cGeometryBuilder_v3_n3_t2_c4(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 3;
    data.nNormalsPerPoint = 3;
    data.nTextureCoordinatesPerPoint = 2;
    data.nColoursPerPoint = 4;
  }

  inline void cGeometryBuilder_v3_n3_t2_c4::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cColour4& colour)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(vertex.z);
    data.vertices.push_back(normal.x);
    data.vertices.push_back(normal.y);
    data.vertices.push_back(normal.z);
    data.vertices.push_back(textureCoord0.x);
    data.vertices.push_back(textureCoord0.y);
    data.vertices.push_back(colour.r);
    data.vertices.push_back(colour.g);
    data.vertices.push_back(colour.b);
    data.vertices.push_back(colour.a);
    data.nVertexCount++;
  }


  inline cGeometryBuilder_v3_n3_t2_t2::cGeometryBuilder_v3_n3_t2_t2(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 3;
    data.nNormalsPerPoint = 3;
    data.nTextureCoordinatesPerPoint = 4;
  }

  inline void cGeometryBuilder_v3_n3_t2_t2::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec2& textureCoord1)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(vertex.z);
    data.vertices.push_back(normal.x);
    data.vertices.push_back(normal.y);
    data.vertices.push_back(normal.z);
    data.vertices.push_back(textureCoord0.x);
    data.vertices.push_back(textureCoord0.y);
    data.vertices.push_back(textureCoord1.x);
    data.vertices.push_back(textureCoord1.y);
    data.nVertexCount++;
  }

  inline void cGeometryBuilder_v3_n3_t2_t2::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoordDuplicated)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(vertex.z);
    data.vertices.push_back(normal.x);
    data.vertices.push_back(normal.y);
    data.vertices.push_back(normal.z);
    data.vertices.push_back(textureCoordDuplicated.x);
    data.vertices.push_back(textureCoordDuplicated.y);
    data.vertices.push_back(textureCoordDuplicated.x);
    data.vertices.push_back(textureCoordDuplicated.y);
    data.nVertexCount++;
  }


  inline cGeometryBuilder_v3_n3_t2_t2_i::cGeometryBuilder_v3_n3_t2_t2_i(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 3;
    data.nNormalsPerPoint = 3;
    data.nTextureCoordinatesPerPoint = 4;
  }

  inline void cGeometryBuilder_v3_n3_t2_t2_i::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec2& textureCoord1)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(vertex.z);
    data.vertices.push_back(normal.x);
    data.vertices.push_back(normal.y);
    data.vertices.push_back(normal.z);
    data.vertices.push_back(textureCoord0.x);
    data.vertices.push_back(textureCoord0.y);
    data.vertices.push_back(textureCoord1.x);
    data.vertices.push_back(textureCoord1.y);
    data.nVertexCount++;
  }

  inline void cGeometryBuilder_v3_n3_t2_t2_i::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoordDuplicated)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(vertex.z);
    data.vertices.push_back(normal.x);
    data.vertices.push_back(normal.y);
    data.vertices.push_back(normal.z);
    data.vertices.push_back(textureCoordDuplicated.x);
    data.vertices.push_back(textureCoordDuplicated.y);
    data.vertices.push_back(textureCoordDuplicated.x);
    data.vertices.push_back(textureCoordDuplicated.y);
    data.nVertexCount++;
  }

  inline void cGeometryBuilder_v3_n3_t2_t2_i::PushBackIndex(uint16_t index)
  {
    data.indices.push_back(index);
  }


  inline cGeometryBuilder_v3_n3_t2_t2_t2::cGeometryBuilder_v3_n3_t2_t2_t2(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 3;
    data.nNormalsPerPoint = 3;
    data.nTextureCoordinatesPerPoint = 6;
  }

  inline void cGeometryBuilder_v3_n3_t2_t2_t2::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec2& textureCoord1, const spitfire::math::cVec2& textureCoord2)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(vertex.z);
    data.vertices.push_back(normal.x);
    data.vertices.push_back(normal.y);
    data.vertices.push_back(normal.z);
    data.vertices.push_back(textureCoord0.x);
    data.vertices.push_back(textureCoord0.y);
    data.vertices.push_back(textureCoord1.x);
    data.vertices.push_back(textureCoord1.y);
    data.vertices.push_back(textureCoord2.x);
    data.vertices.push_back(textureCoord2.y);
    data.nVertexCount++;
  }

  inline void cGeometryBuilder_v3_n3_t2_t2_t2::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoordDuplicated)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(vertex.z);
    data.vertices.push_back(normal.x);
    data.vertices.push_back(normal.y);
    data.vertices.push_back(normal.z);
    data.vertices.push_back(textureCoordDuplicated.x);
    data.vertices.push_back(textureCoordDuplicated.y);
    data.vertices.push_back(textureCoordDuplicated.x);
    data.vertices.push_back(textureCoordDuplicated.y);
    data.vertices.push_back(textureCoordDuplicated.x);
    data.vertices.push_back(textureCoordDuplicated.y);
    data.nVertexCount++;
  }


  inline cGeometryBuilder_v3_n3_t2_t2_t2_i::cGeometryBuilder_v3_n3_t2_t2_t2_i(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 3;
    data.nNormalsPerPoint = 3;
    data.nTextureCoordinatesPerPoint = 6;
  }

  inline void cGeometryBuilder_v3_n3_t2_t2_t2_i::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec2& textureCoord1, const spitfire::math::cVec2& textureCoord2)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(vertex.z);
    data.vertices.push_back(normal.x);
    data.vertices.push_back(normal.y);
    data.vertices.push_back(normal.z);
    data.vertices.push_back(textureCoord0.x);
    data.vertices.push_back(textureCoord0.y);
    data.vertices.push_back(textureCoord1.x);
    data.vertices.push_back(textureCoord1.y);
    data.vertices.push_back(textureCoord2.x);
    data.vertices.push_back(textureCoord2.y);
    data.nVertexCount++;
  }

  inline void cGeometryBuilder_v3_n3_t2_t2_t2_i::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoordDuplicated)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(vertex.z);
    data.vertices.push_back(normal.x);
    data.vertices.push_back(normal.y);
    data.vertices.push_back(normal.z);
    data.vertices.push_back(textureCoordDuplicated.x);
    data.vertices.push_back(textureCoordDuplicated.y);
    data.vertices.push_back(textureCoordDuplicated.x);
    data.vertices.push_back(textureCoordDuplicated.y);
    data.vertices.push_back(textureCoordDuplicated.x);
    data.vertices.push_back(textureCoordDuplicated.y);
    data.nVertexCount++;
  }

  inline void cGeometryBuilder_v3_n3_t2_t2_t2_i::PushBackIndex(uint16_t index)
  {
    data.indices.push_back(index);
  }


  inline cGeometryBuilder_v3_n3_t2_user3::cGeometryBuilder_v3_n3_t2_user3(cGeometryData& _data) :
    data(_data)
  {
    data.nVerticesPerPoint = 3;
    data.nNormalsPerPoint = 3;
    data.nTextureCoordinatesPerPoint = 2;
    data.nFloatUserData0PerPoint = 3;
  }

  inline void cGeometryBuilder_v3_n3_t2_user3::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec3& user0)
  {
    data.vertices.push_back(vertex.x);
    data.vertices.push_back(vertex.y);
    data.vertices.push_back(vertex.z);
    data.vertices.push_back(normal.x);
    data.vertices.push_back(normal.y);
    data.vertices.push_back(normal.z);
    data.vertices.push_back(textureCoord0.x);
    data.vertices.push_back(textureCoord0.y);
    data.vertices.push_back(user0.x);
    data.vertices.push_back(user0.y);
    data.vertices.push_back(user0.z);
    data.nVertexCount++;
  }


  class cGeometryBuilder
  {
  public:
    void CreatePlane(float fWidth, float fDepth, cGeometryData& data, size_t nTextureUnits);
    void CreatePlane(float fWidth, float fDepth, float fTextureU, float fTextureV, cGeometryData& data, size_t nTextureUnits);
    void CreateCube(float fWidthAndDepthAndHeight, cGeometryData& data, size_t nTextureUnits);
    void CreateBox(float fWidth, float fDepth, float fHeight, cGeometryData& data, size_t nTextureUnits);
    void CreateBoxWithTopAndBottomColours(float fWidth, float fDepth, float fHeight, cGeometryData& data, size_t nTextureUnits, const spitfire::math::cColour4& colourBottom, const spitfire::math::cColour4& colourTop);
    void CreateSphere(float fRadius, size_t nSegments, cGeometryData& data, size_t nTextureUnits);
    void CreateSphere(const spitfire::math::cVec3& position, float fRadius, size_t nSegments, cGeometryData& data, size_t nTextureUnits);
    void CreateCylinder(float fRadius, float fHeight, size_t nCircleSegments, cGeometryData& data, size_t nTextureUnits);
    void CreateCylinderWithTopAndBottomColours(float fRadius, float fHeight, size_t nCircleSegments, const spitfire::math::cColour4& colourBottom, const spitfire::math::cColour4& colourTop, cGeometryData& data, size_t nTextureUnits);
    void CreateTeapot(float fRadius, size_t nSegments, cGeometryData& data, size_t nTextureUnits);
    void CreateGear(float fInnerRadius, float fOuterRadius, float fWidth, size_t nTeeth, float fToothDepth, cGeometryData& data); // This is primarily for openglmm_gears and there are no texture coordinates generated

    // 2D geometry, mainly for creating GUI elements
    static void CreateCircle(cGeometryBuilder_v2& builder, const spitfire::math::cVec2& center, float fRadius, size_t nSegments); // A full circle
    static void CreateArc(cGeometryBuilder_v2& builder, const spitfire::math::cVec2& center, float fRadius, size_t nSegments, float fStartAngleDegrees0AtTop, float fEndAngleDegrees0AtTop); // A pizza slice
    static void CreateArc(cGeometryBuilder_v2& builder, const spitfire::math::cVec2& center, float fInnerRadius, float fOuterRadius, size_t nSegments, float fStartAngleDegrees0AtTop, float fEndAngleDegrees0AtTop); // Part of the outside of a donut
  };
}

#endif // LIBOPENGLMM_CGEOMETRY_H
