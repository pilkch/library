// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cmath>

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

// OpenGL headers
#include <GL/GLee.h>
#include <GL/glu.h>

// SDL headers
#include <SDL/SDL_image.h>

// libopenglmm headers
#include <libopenglmm/cSystem.h>
#include <libopenglmm/cVertexArray.h>

namespace opengl
{
  // *** cDynamicVertexArray

  cDynamicVertexArray::cDynamicVertexArray() :
    bIsCompiled(false),
    bIs2D(false),
    vertex_size(0),
    normal_size(0),
    colour_size(0),
    texturecoordinate_size(0),
    indices_size(0),
    nTextureUnits(0)
  {
  }

  void cDynamicVertexArray::SetVertices(const std::vector<float>& _vertices)
  {
    assert(sizeof(float) == sizeof(GLfloat));
    assert(!_vertices.empty());
    assert(!IsCompiled());

    vertices = _vertices;
    vertex_size = vertices.size() * sizeof(GLfloat);
  }

  void cDynamicVertexArray::SetNormals(const std::vector<float>& _normals)
  {
    assert(sizeof(float) == sizeof(GLfloat));
    assert(!_normals.empty());
    assert(!IsCompiled());

    normals = _normals;
    normal_size = normals.size() * sizeof(GLfloat);
  }

  void cDynamicVertexArray::SetColours(const std::vector<float>& _colours)
  {
    assert(sizeof(float) == sizeof(GLfloat));
    assert(!_colours.empty());
    assert(!IsCompiled());

    colours = _colours;
    colour_size = colours.size() * sizeof(GLfloat);
  }

  void cDynamicVertexArray::SetTextureCoordinates(const std::vector<float>& _textureCoordinates)
  {
    assert(sizeof(float) == sizeof(GLfloat));
    assert(!_textureCoordinates.empty());
    assert(!IsCompiled());

    textureCoordinates = _textureCoordinates;
    texturecoordinate_size = textureCoordinates.size() * sizeof(GLfloat);
  }

  void cDynamicVertexArray::SetIndices(const std::vector<uint16_t>& _indices)
  {
    assert(sizeof(uint16_t) == sizeof(GLushort));
    assert(!_indices.empty());
    assert(!IsCompiled());

    indices = _indices;
    indices_size = indices.size() * sizeof(GLushort);
  }

  void cDynamicVertexArray::Compile(const cSystem& system)
  {
    assert(!IsCompiled());

    const size_t nVertices = vertices.size() / 3;
    const size_t nTextureCoordinates = textureCoordinates.size() / 2;
    assert(nVertices != 0);
    if (nTextureCoordinates == (nVertices + nVertices + nVertices)) nTextureUnits = 3;
    else if (nTextureCoordinates == (nVertices + nVertices)) nTextureUnits = 2;
    else if (nTextureCoordinates == nVertices) nTextureUnits = 1;
    else nTextureUnits = 0;

    //std::cout<<"cDynamicVertexArray::Compile nVertices="<<nVertices<<" nTextureUnits="<<nTextureUnits<<" indices="<<indices.size()<<" glGetError="<<system.GetErrorString()<<std::endl;

    bIsCompiled = true;
    bIs2D = false;
  }

  void cDynamicVertexArray::Compile2D(const cSystem& system)
  {
    assert(!IsCompiled());

    const size_t nVertices = vertices.size() / 2;
    const size_t nTextureCoordinates = textureCoordinates.size() / 2;
    assert(nVertices != 0);
    if (nTextureCoordinates == (nVertices + nVertices + nVertices)) nTextureUnits = 3;
    else if (nTextureCoordinates == (nVertices + nVertices)) nTextureUnits = 2;
    else if (nTextureCoordinates == nVertices) nTextureUnits = 1;
    else nTextureUnits = 0;

    //std::cout<<"cDynamicVertexArray::Compile2D nVertices="<<nVertices<<" nTextureUnits="<<nTextureUnits<<" indices="<<indices.size()<<" glGetError="<<system.GetErrorString()<<std::endl;

    bIsCompiled = true;
    bIs2D = true;
  }

  void cDynamicVertexArray::Destroy()
  {
    bIsCompiled = false;
    bIs2D = false;

    vertices.clear();
    normals.clear();
    colours.clear();
    textureCoordinates.clear();
    indices.clear();

    vertex_size = 0;
    normal_size = 0;
    colour_size = 0;
    texturecoordinate_size = 0;
    indices_size = 0;

    nTextureUnits = 0;
  }

  void cDynamicVertexArray::Bind()
  {
    assert(IsCompiled());
    assert(!bIs2D);

    // Enable vertex information
    glEnableClientState(GL_VERTEX_ARRAY);

    // Describe to OpenGL where the vertex data is
    glVertexPointer(3, GL_FLOAT, 0, vertices.data());

    // Enable normal information
    if (normal_size > 0) {
      // Describe to OpenGL where the normal coordinate data is
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, 0, normals.data());
    }

    // Enable colour information
    if (colour_size > 0) {
      // Describe to OpenGL where the colour coordinate data is
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(4, GL_FLOAT, 0, colours.data());
    }

    // Enable texture coordinate information
    if (texturecoordinate_size > 0) {
      //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      // Describe to OpenGL where the texture coordinate data is
      //glTexCoordPointer(nTextureUnits * 2, GL_FLOAT, 0, textureCoordinates.data());

      for (size_t i = 0; i < nTextureUnits; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glClientActiveTexture(GL_TEXTURE0 + i);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 2 * nTextureUnits * sizeof(GL_FLOAT), textureCoordinates.data() + (2 * i * sizeof(GL_FLOAT)));
      }

      // This is not normal!  Instead of sending 2 * 2 texture coordinates for every point, we send
      // 4 * 1 texture coordinates for every point, sending them all in one texture unit
      // if (texturecoordinate_size > 1) {
      //   glActiveTexture(GL_TEXTURE1);
      //   glClientActiveTexture(GL_TEXTURE1);
      //   glEnable(GL_TEXTURE_2D);
      //   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      //   glTexCoordPointer(4, GL_FLOAT, 0, textureCoordinates.data());
      // }
    }
  }

  void cDynamicVertexArray::Bind2D()
  {
    assert(IsCompiled());
    assert(bIs2D);

    // Enable vertex information
    glEnableClientState(GL_VERTEX_ARRAY);

    // Describe to OpenGL where the vertex data is
    glVertexPointer(2, GL_FLOAT, 0, vertices.data());

    // Enable normal information
    if (normal_size > 0) {
      // Describe to OpenGL where the normal coordinate data is
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, 0, normals.data());
    }

    // Enable colour information
    if (colour_size > 0) {
      // Describe to OpenGL where the colour coordinate data is
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(4, GL_FLOAT, 0, colours.data());
    }

    // Enable texture coordinate information
    if (texturecoordinate_size > 0) {
      //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      // Describe to OpenGL where the texture coordinate data is
      //glTexCoordPointer(nTextureUnits * 2, GL_FLOAT, 0, textureCoordinates.data());

      for (size_t i = 0; i < nTextureUnits; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glClientActiveTexture(GL_TEXTURE0 + i);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 2 * nTextureUnits * sizeof(GL_FLOAT), textureCoordinates.data() + (2 * i * sizeof(GL_FLOAT)));
      }

      // This is not normal!  Instead of sending 2 * 2 texture coordinates for every point, we send
      // 4 * 1 texture coordinates for every point, sending them all in one texture unit
      // if (texturecoordinate_size > 1) {
      //   glActiveTexture(GL_TEXTURE1);
      //   glClientActiveTexture(GL_TEXTURE1);
      //   glEnable(GL_TEXTURE_2D);
      //   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      //   glTexCoordPointer(4, GL_FLOAT, 0, textureCoordinates.data() + (2 * i * sizeof(GL_FLOAT)));
      // }
    }
  }

  void cDynamicVertexArray::Unbind()
  {
    assert(IsCompiled());

    // Disable texture coordinate information
    if (texturecoordinate_size != 0) {
      for (size_t i = 0; i < nTextureUnits; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glClientActiveTexture(GL_TEXTURE0 + i);

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      }
    }

    // Disable colour information
    if (colour_size != 0) glDisableClientState(GL_COLOR_ARRAY);

    // Disable normal information
    if (normal_size != 0) glDisableClientState(GL_NORMAL_ARRAY);

    // Disable vertex information
    glDisableClientState(GL_VERTEX_ARRAY);
  }

  void cDynamicVertexArray::RenderGeometry(GLenum geometryType)
  {
    assert(IsCompiled());
    assert(!bIs2D);

    if (indices.empty()) {
      // Draw this many vertices of type specified by geometryType (GL_LINES, GL_TRIANGLES, strips, quads, etc.)
      const size_t nVertices = vertices.size() / 3;
      glDrawArrays(geometryType, 0, nVertices);
    } else {
      glDrawElements(geometryType, indices.size(), GL_UNSIGNED_SHORT,  indices.data());
    }
  }

  void cDynamicVertexArray::RenderGeometry2D(GLenum geometryType)
  {
    assert(IsCompiled());
    assert(bIs2D);

    if (indices.empty()) {
      // Draw this many vertices of type specified by geometryType (GL_LINES, GL_TRIANGLES, strips, quads, etc.)
      const size_t nVertices = vertices.size() / 2;
      glDrawArrays(geometryType, 0, nVertices);
    } else {
      glDrawElements(geometryType, indices.size(), GL_UNSIGNED_SHORT,  indices.data());
    }
  }

  void cDynamicVertexArray::RenderLines()
  {
    RenderGeometry(GL_LINES);
  }

  void cDynamicVertexArray::RenderTriangles()
  {
    RenderGeometry(GL_TRIANGLES);
  }

  void cDynamicVertexArray::RenderTriangleStrip()
  {
    RenderGeometry(GL_TRIANGLE_STRIP);
  }

  void cDynamicVertexArray::RenderQuads()
  {
    RenderGeometry(GL_QUADS);
  }

  void cDynamicVertexArray::RenderQuadStrip()
  {
    RenderGeometry(GL_QUAD_STRIP);
  }


  void cDynamicVertexArray::RenderLines2D()
  {
    RenderGeometry2D(GL_LINES);
  }

  void cDynamicVertexArray::RenderQuads2D()
  {
    RenderGeometry2D(GL_QUADS);
  }
}
