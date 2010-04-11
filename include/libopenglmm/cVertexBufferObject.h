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

#ifndef LIBOPENGLMM_CVERTEXBUFFEROBJECT_H
#define LIBOPENGLMM_CVERTEXBUFFEROBJECT_H

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cColour.h>

#include <libopenglmm/libopenglmm.h>

typedef uint32_t GLuint;
typedef ptrdiff_t GLsizeiptr;

namespace opengl
{
  // http://playcontrol.net/ewing/jibberjabber/opengl_vertex_buffer_object.html

  class cStaticVertexBufferObject
  {
  public:
    friend class cContext;

    cStaticVertexBufferObject();

    bool IsCompiled() const { return bIsCompiled; }
    size_t GetApproximateTriangleCount() const { return vertices.size() / 4; }

    void SetVertices(const std::vector<float>& vertices);
    void SetNormals(const std::vector<float>& normals);
    void SetColours(const std::vector<float>& colours);
    void SetTextureCoordinates(const std::vector<float>& textureCoordinates);
    void SetIndices(const std::vector<uint16_t>& indices);

    void Compile(const cSystem& system);
    void Compile2D(const cSystem& system);
    void Destroy();

  protected:
    void Bind();
    void Bind2D();
    void Unbind();
    void Unbind2D() { Unbind(); }

    void RenderLines();
    void RenderTriangles();
    void RenderTriangleStrip();
    void RenderQuads();
    void RenderQuadStrip();

    void RenderQuads2D();

  private:
    void RenderGeometry(GLenum geometryType);

    void RenderGeometry2D(GLenum geometryType);

    bool bIsCompiled;

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> colours;
    std::vector<float> textureCoordinates;
    std::vector<uint16_t> indices;

    GLsizeiptr vertex_size;
    GLsizeiptr normal_size;
    GLsizeiptr colour_size;
    GLsizeiptr texturecoordinate_size;
    GLsizeiptr indices_size;

    size_t nTextureUnits;

    // Note: one buffer per cStaticVertexBufferObject,
    // but with multiple offsets for each cVertexBufferObjectArray
    GLuint bufferID;
  };
}

#endif // LIBOPENGLMM_CVERTEXBUFFEROBJECT_H
