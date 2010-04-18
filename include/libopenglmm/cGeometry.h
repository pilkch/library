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

namespace opengl
{

  class cGeometryBuilder_v2
  {
  public:
    cGeometryBuilder_v2(std::vector<float>& vertices);

    void PushBack(const spitfire::math::cVec2& vertex);

  private:
    std::vector<float>& vertices;
  };

  class cGeometryBuilder_v2_t2
  {
  public:
    cGeometryBuilder_v2_t2(std::vector<float>& vertices, std::vector<float>& textureCoords);

    void PushBack(const spitfire::math::cVec2& vertex, const spitfire::math::cVec2& textureCoord0);

  private:
    std::vector<float>& vertices;
    std::vector<float>& textureCoords;
  };

  class cGeometryBuilder_v2_c4_t2
  {
  public:
    cGeometryBuilder_v2_c4_t2(std::vector<float>& vertices, std::vector<float>&colours, std::vector<float>& textureCoords);

    void PushBack(const spitfire::math::cVec2& vertex, const spitfire::math::cColour& colour, const spitfire::math::cVec2& textureCoord0);

  private:
    std::vector<float>& vertices;
    std::vector<float>& colours;
    std::vector<float>& textureCoords;
  };


  class cGeometryBuilder_v3_n3
  {
  public:
    cGeometryBuilder_v3_n3(std::vector<float>& vertices, std::vector<float>& normals);

    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal);

  private:
    std::vector<float>& vertices;
    std::vector<float>& normals;
  };

  class cGeometryBuilder_v3_n3_t2
  {
  public:
    cGeometryBuilder_v3_n3_t2(std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& textureCoords);

    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0);

  private:
    std::vector<float>& vertices;
    std::vector<float>& normals;
    std::vector<float>& textureCoords;
  };

  class cGeometryBuilder_v3_n3_t2_t2
  {
  public:
    cGeometryBuilder_v3_n3_t2_t2(std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& textureCoords);

    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec2& textureCoord1);
    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoordDuplicated);

  private:
    std::vector<float>& vertices;
    std::vector<float>& normals;
    std::vector<float>& textureCoords;
  };

  class cGeometryBuilder_v3_n3_t2_t2_t2
  {
  public:
    cGeometryBuilder_v3_n3_t2_t2_t2(std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& textureCoords);

    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec2& textureCoord1, const spitfire::math::cVec2& textureCoord2);
    void PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoordDuplicated);

  private:
    std::vector<float>& vertices;
    std::vector<float>& normals;
    std::vector<float>& textureCoords;
  };



  inline cGeometryBuilder_v2::cGeometryBuilder_v2(std::vector<float>& _vertices) :
    vertices(_vertices)
  {
  }

  inline void cGeometryBuilder_v2::PushBack(const spitfire::math::cVec2& vertex)
  {
    vertices.push_back(vertex.x);
    vertices.push_back(vertex.y);
  }


  inline cGeometryBuilder_v2_t2::cGeometryBuilder_v2_t2(std::vector<float>& _vertices, std::vector<float>& _textureCoords) :
    vertices(_vertices),
    textureCoords(_textureCoords)
  {
  }

  inline void cGeometryBuilder_v2_t2::PushBack(const spitfire::math::cVec2& vertex, const spitfire::math::cVec2& textureCoord0)
  {
    vertices.push_back(vertex.x);
    vertices.push_back(vertex.y);
    textureCoords.push_back(textureCoord0.x);
    textureCoords.push_back(textureCoord0.y);
  }


  inline cGeometryBuilder_v2_c4_t2::cGeometryBuilder_v2_c4_t2(std::vector<float>& _vertices, std::vector<float>& _colours, std::vector<float>& _textureCoords) :
    vertices(_vertices),
    colours(_colours),
    textureCoords(_textureCoords)
  {
  }

  inline void cGeometryBuilder_v2_c4_t2::PushBack(const spitfire::math::cVec2& vertex, const spitfire::math::cColour& colour, const spitfire::math::cVec2& textureCoord0)
  {
    vertices.push_back(vertex.x);
    vertices.push_back(vertex.y);
    colours.push_back(colour.r);
    colours.push_back(colour.g);
    colours.push_back(colour.b);
    colours.push_back(colour.a);
    textureCoords.push_back(textureCoord0.x);
    textureCoords.push_back(textureCoord0.y);
  }


  inline cGeometryBuilder_v3_n3::cGeometryBuilder_v3_n3(std::vector<float>& _vertices, std::vector<float>& _normals) :
    vertices(_vertices),
    normals(_normals)
  {
  }

  inline void cGeometryBuilder_v3_n3::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal)
  {
    vertices.push_back(vertex.x);
    vertices.push_back(vertex.y);
    vertices.push_back(vertex.z);
    normals.push_back(normal.x);
    normals.push_back(normal.y);
    normals.push_back(normal.z);
  }


  inline cGeometryBuilder_v3_n3_t2::cGeometryBuilder_v3_n3_t2(std::vector<float>& _vertices, std::vector<float>& _normals, std::vector<float>& _textureCoords) :
    vertices(_vertices),
    normals(_normals),
    textureCoords(_textureCoords)
  {
  }

  inline void cGeometryBuilder_v3_n3_t2::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0)
  {
    vertices.push_back(vertex.x);
    vertices.push_back(vertex.y);
    vertices.push_back(vertex.z);
    normals.push_back(normal.x);
    normals.push_back(normal.y);
    normals.push_back(normal.z);
    textureCoords.push_back(textureCoord0.x);
    textureCoords.push_back(textureCoord0.y);
  }


  inline cGeometryBuilder_v3_n3_t2_t2::cGeometryBuilder_v3_n3_t2_t2(std::vector<float>& _vertices, std::vector<float>& _normals, std::vector<float>& _textureCoords) :
    vertices(_vertices),
    normals(_normals),
    textureCoords(_textureCoords)
  {
  }

  inline void cGeometryBuilder_v3_n3_t2_t2::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec2& textureCoord1)
  {
    vertices.push_back(vertex.x);
    vertices.push_back(vertex.y);
    vertices.push_back(vertex.z);
    normals.push_back(normal.x);
    normals.push_back(normal.y);
    normals.push_back(normal.z);
    textureCoords.push_back(textureCoord0.x);
    textureCoords.push_back(textureCoord0.y);
    textureCoords.push_back(textureCoord1.x);
    textureCoords.push_back(textureCoord1.y);
  }

  inline void cGeometryBuilder_v3_n3_t2_t2::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoordDuplicated)
  {
    vertices.push_back(vertex.x);
    vertices.push_back(vertex.y);
    vertices.push_back(vertex.z);
    normals.push_back(normal.x);
    normals.push_back(normal.y);
    normals.push_back(normal.z);
    textureCoords.push_back(textureCoordDuplicated.x);
    textureCoords.push_back(textureCoordDuplicated.y);
    textureCoords.push_back(textureCoordDuplicated.x);
    textureCoords.push_back(textureCoordDuplicated.y);
  }


  inline cGeometryBuilder_v3_n3_t2_t2_t2::cGeometryBuilder_v3_n3_t2_t2_t2(std::vector<float>& _vertices, std::vector<float>& _normals, std::vector<float>& _textureCoords) :
    vertices(_vertices),
    normals(_normals),
    textureCoords(_textureCoords)
  {
  }

  inline void cGeometryBuilder_v3_n3_t2_t2_t2::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoord0, const spitfire::math::cVec2& textureCoord1, const spitfire::math::cVec2& textureCoord2)
  {
    vertices.push_back(vertex.x);
    vertices.push_back(vertex.y);
    vertices.push_back(vertex.z);
    normals.push_back(normal.x);
    normals.push_back(normal.y);
    normals.push_back(normal.z);
    textureCoords.push_back(textureCoord0.x);
    textureCoords.push_back(textureCoord0.y);
    textureCoords.push_back(textureCoord1.x);
    textureCoords.push_back(textureCoord1.y);
    textureCoords.push_back(textureCoord2.x);
    textureCoords.push_back(textureCoord2.y);
  }

  inline void cGeometryBuilder_v3_n3_t2_t2_t2::PushBack(const spitfire::math::cVec3& vertex, const spitfire::math::cVec3& normal, const spitfire::math::cVec2& textureCoordDuplicated)
  {
    vertices.push_back(vertex.x);
    vertices.push_back(vertex.y);
    vertices.push_back(vertex.z);
    normals.push_back(normal.x);
    normals.push_back(normal.y);
    normals.push_back(normal.z);
    textureCoords.push_back(textureCoordDuplicated.x);
    textureCoords.push_back(textureCoordDuplicated.y);
    textureCoords.push_back(textureCoordDuplicated.x);
    textureCoords.push_back(textureCoordDuplicated.y);
    textureCoords.push_back(textureCoordDuplicated.x);
    textureCoords.push_back(textureCoordDuplicated.y);
  }



  class cGeometryBuilder
  {
  private:
    void CreatePlane(float fWidth, float fDepth, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& textureCoords, size_t nTextureUnits);
    void CreateCube(float fWidthAndDepthAndHeight, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& textureCoords, size_t nTextureUnits);
    void CreateBox(float fWidth, float fDepth, float fHeight, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& textureCoords, size_t nTextureUnits);
    void CreateSphere(float fRadius, size_t nSegments, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& textureCoords, size_t nTextureUnits);
    void CreateGear(float fInnerRadius, float fOuterRadius, float fWidth, size_t nTeeth, float fToothDepth, std::vector<float>& vertices, std::vector<float>& normals); // This is primarily for openglmm_gears
    void CreateGear(float fInnerRadius, float fOuterRadius, float fWidth, size_t nTeeth, float fToothDepth, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& textureCoords, size_t nTextureUnits);
  };
}

#endif // LIBOPENGLMM_CGEOMETRY_H
