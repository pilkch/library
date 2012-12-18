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
#include <libopenglmm/opengl.h>

#include <libopenglmm/cGeometryData.h>

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

    void SetData(cGeometryDataPtr pGeometryDataPtr);

    size_t GetApproximateTriangleCount() const;

    bool IsCompiled() const { return bIsCompiled; }
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
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    // Quads are deprecated in OpenGL 3.1 core profile
    void RenderQuads();
    void RenderQuadStrip();
    #endif

    void RenderLines2D();
    void RenderTriangles2D();
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    // Quads are deprecated in OpenGL 3.1 core profile
    void RenderQuads2D();
    #endif

  private:
    void RenderGeometry(GLenum geometryType);
    void RenderGeometry2D(GLenum geometryType);

    bool bIsCompiled;
    bool bIs2D;

    cGeometryDataPtr pGeometryDataPtr;

    GLsizeiptr vertex_size;
    GLsizeiptr normal_size;
    GLsizeiptr colour_size;
    GLsizeiptr texturecoordinate_size;
    GLsizeiptr indices_size;
    GLsizeiptr floatUserData0_size;
    GLsizeiptr floatUserData1_size;
    GLsizeiptr floatUserData2_size;

    size_t nTextureUnits;

    #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
    GLuint vertexArrayObjectID;
    #endif

    // Note: one buffer per cStaticVertexBufferObject,
    // but with multiple offsets for each cVertexBufferObjectArray
    GLuint bufferID;
  };
}

#endif // LIBOPENGLMM_CVERTEXBUFFEROBJECT_H
