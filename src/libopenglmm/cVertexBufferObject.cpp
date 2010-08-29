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
#include <libopenglmm/cVertexBufferObject.h>

namespace opengl
{
  #define BUFFER_OFFSET(i) ((char *)nullptr + (i))

  // *** cStaticVertexBufferObject

  cStaticVertexBufferObject::cStaticVertexBufferObject() :
    bIsCompiled(false),
    bIs2D(false),
    vertex_size(0),
    normal_size(0),
    colour_size(0),
    texturecoordinate_size(0),
    indices_size(0),
    nTextureUnits(0),
    bufferID(0)
  {
  }

  void cStaticVertexBufferObject::SetVertices(const std::vector<float>& _vertices)
  {
    assert(sizeof(float) == sizeof(GLfloat));
    assert(!_vertices.empty());
    assert(!IsCompiled());

    vertices = _vertices;
    vertex_size = vertices.size() * sizeof(GLfloat);
  }

  void cStaticVertexBufferObject::SetNormals(const std::vector<float>& _normals)
  {
    assert(sizeof(float) == sizeof(GLfloat));
    assert(!_normals.empty());
    assert(!IsCompiled());

    normals = _normals;
    normal_size = normals.size() * sizeof(GLfloat);
  }

  void cStaticVertexBufferObject::SetColours(const std::vector<float>& _colours)
  {
    assert(sizeof(float) == sizeof(GLfloat));
    assert(!_colours.empty());
    assert(!IsCompiled());

    colours = _colours;
    colour_size = colours.size() * sizeof(GLfloat);
  }

  void cStaticVertexBufferObject::SetTextureCoordinates(const std::vector<float>& _textureCoordinates)
  {
    assert(sizeof(float) == sizeof(GLfloat));
    assert(!_textureCoordinates.empty());
    assert(!IsCompiled());

    textureCoordinates = _textureCoordinates;
    texturecoordinate_size = textureCoordinates.size() * sizeof(GLfloat);
  }

  void cStaticVertexBufferObject::SetIndices(const std::vector<uint16_t>& _indices)
  {
    assert(sizeof(uint16_t) == sizeof(GLushort));
    assert(!_indices.empty());
    assert(!IsCompiled());

    indices = _indices;
    indices_size = indices.size() * sizeof(GLushort);
  }

  void cStaticVertexBufferObject::Compile(const cSystem& system)
  {
    assert(!IsCompiled());

    const size_t nVertices = vertices.size() / 3;
    const size_t nTextureCoordinates = textureCoordinates.size() / 2;
    assert(nVertices != 0);
    if (nTextureCoordinates == (nVertices + nVertices + nVertices)) nTextureUnits = 3;
    else if (nTextureCoordinates == (nVertices + nVertices)) nTextureUnits = 2;
    else if (nTextureCoordinates == nVertices) nTextureUnits = 1;
    else nTextureUnits = 0;

    //std::cout<<"cStaticVertexBufferObject::Compile nVertices="<<nVertices<<" nTextureUnits="<<nTextureUnits<<" indices="<<indices.size()<<" glGetError="<<system.GetErrorString()<<std::endl;

    // Create a new buffer
    glGenBuffers(1, &bufferID);
    //std::cout<<"cStaticVertexBufferObject::Compile glGenBuffers glGetError="<<system.GetErrorString()<<", bufferID="<<bufferID<<std::endl;
    assert(bufferID != 0);

    // Bbind the buffer object to use
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);

    // Allocate enough memory for the whole buffer
    // Also GL_DYNAMIC_DRAW and GL_STREAM_DRAW
    glBufferData(GL_ARRAY_BUFFER, vertex_size + normal_size + colour_size + texturecoordinate_size, nullptr, GL_STATIC_DRAW);

    glEnableClientState(GL_VERTEX_ARRAY);
      // Describe to OpenGL where the vertex data is in the buffer
      glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glDisableClientState(GL_VERTEX_ARRAY);

    if (normal_size != 0) {
      glEnableClientState(GL_NORMAL_ARRAY);
        // Describe to OpenGL where the normal data is in the buffer
        glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(vertex_size));
      glDisableClientState(GL_NORMAL_ARRAY);
    }

    if (colour_size != 0) {
      glEnableClientState(GL_COLOR_ARRAY);
        // Describe to OpenGL where the colour data is in the buffer
        glColorPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(vertex_size + normal_size));
      glDisableClientState(GL_COLOR_ARRAY);
    }

    if (texturecoordinate_size != 0) {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        // Describe to OpenGL where the texture coordinate data is in the buffer
        glTexCoordPointer(nTextureUnits, GL_FLOAT, 0, BUFFER_OFFSET(vertex_size + normal_size + colour_size));
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }


    // http://www.informit.com/articles/article.aspx?p=1377833&seqNum=7

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_size, vertices.data());
    std::cout<<"cStaticVertexBufferObject::Compile b glGetError="<<opengl::cSystem::GetErrorString()<<std::endl;

    if (normal_size != 0) glBufferSubData(GL_ARRAY_BUFFER, vertex_size, normal_size, normals.data());

    if (colour_size != 0) glBufferSubData(GL_ARRAY_BUFFER, vertex_size + normal_size, colour_size, colours.data());

    if (texturecoordinate_size != 0) glBufferSubData(GL_ARRAY_BUFFER, vertex_size + normal_size + colour_size, texturecoordinate_size, textureCoordinates.data());

    /*// Set the buffer data
    GLvoid* pVoid = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    assert(pVoid != nullptr);

      float* pBuffer = (float*)pVoid;

      // Transfer the vertex data to the VBO
      memcpy(pBuffer, vertices.data(), vertex_size);

      // Append normal data to vertex data
      if (normal_size != 0) memcpy(&pBuffer[vertices.size()], normals.data(), normal_size);

      // Append colour data to vertex data and normal data
      if (colour_size != 0) memcpy(&pBuffer[vertices.size() + normals.size()], colours.data(), colour_size);

      // Append texture coordinates data to vertex data and normal data and colour data
      if (texturecoordinate_size != 0) memcpy(&pBuffer[vertices.size() + normals.size() + colours.size()], textureCoordinates.data(), texturecoordinate_size);

    glUnmapBuffer(GL_ARRAY_BUFFER);*/


    // Index buffer

    // create index buffer
    //glGenBuffers(1, &cubeIBO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO);

    // For constrast, instead of glBufferSubData and glMapBuffer,
    // we can directly supply the data in one-shot
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUMBER_OF_CUBE_INDICES*sizeof(GLubyte), s_cubeIndices, GL_STATIC_DRAW);


    // We are now finished and are ready to unbind

    // Unbind the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Disable texture coordinate information
    if (texturecoordinate_size != 0) glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // Disable colour information
    if (colour_size != 0) glDisableClientState(GL_COLOR_ARRAY);

    // Disable normal information
    if (normal_size != 0) glDisableClientState(GL_NORMAL_ARRAY);

    // Disable vertex information
    glDisableClientState(GL_VERTEX_ARRAY);

    bIsCompiled = true;
    bIs2D = false;
  }

  void cStaticVertexBufferObject::Compile2D(const cSystem& system)
  {
    assert(!IsCompiled());

    const size_t nVertices = vertices.size() / 2;
    const size_t nTextureCoordinates = textureCoordinates.size() / 2;
    assert(nVertices != 0);
    if (nTextureCoordinates == (nVertices + nVertices + nVertices)) nTextureUnits = 3;
    else if (nTextureCoordinates == (nVertices + nVertices)) nTextureUnits = 2;
    else if (nTextureCoordinates == nVertices) nTextureUnits = 1;
    else nTextureUnits = 0;

    //std::cout<<"cStaticVertexBufferObject::Compile2D nVertices="<<nVertices<<" nTextureUnits="<<nTextureUnits<<" indices="<<indices.size()<<" glGetError="<<system.GetErrorString()<<std::endl;

    // Create a new buffer
    glGenBuffers(1, &bufferID);
    //std::cout<<"cStaticVertexBufferObject::Compile2D glGenBuffers glGetError="<<system.GetErrorString()<<", bufferID="<<bufferID<<std::endl;
    assert(bufferID != 0);

    // Bbind the buffer object to use
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);

    // Allocate enough memory for the whole buffer
    // Also GL_DYNAMIC_DRAW and GL_STREAM_DRAW
    glBufferData(GL_ARRAY_BUFFER, vertex_size + normal_size + colour_size + texturecoordinate_size, nullptr, GL_STATIC_DRAW);

    glEnableClientState(GL_VERTEX_ARRAY);
      // Describe to OpenGL where the vertex data is in the buffer
      glVertexPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glDisableClientState(GL_VERTEX_ARRAY);

    if (normal_size != 0) {
      glEnableClientState(GL_NORMAL_ARRAY);
        // Describe to OpenGL where the normal data is in the buffer
        glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(vertex_size));
      glDisableClientState(GL_NORMAL_ARRAY);
    }

    if (colour_size != 0) {
      glEnableClientState(GL_COLOR_ARRAY);
        // Describe to OpenGL where the colour data is in the buffer
        glColorPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(vertex_size + normal_size));
      glDisableClientState(GL_COLOR_ARRAY);
    }

    if (texturecoordinate_size != 0) {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        // Describe to OpenGL where the texture coordinate data is in the buffer
        glTexCoordPointer(nTextureUnits * 2, GL_FLOAT, 0, BUFFER_OFFSET(vertex_size + normal_size + colour_size));
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }


    // Set the buffer data
    GLvoid* pVoid = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    assert(pVoid != nullptr);

      float* pBuffer = (float*)pVoid;

      // Transfer the vertex data to the VBO
      memcpy(pBuffer, vertices.data(), vertex_size);

      // Append normal data to vertex data
      if (normal_size != 0) memcpy(&pBuffer[vertices.size()], normals.data(), normal_size);

      // Append colour data to vertex data and normal data
      if (colour_size != 0) memcpy(&pBuffer[vertices.size() + normals.size()], colours.data(), colour_size);

      // Append texture coordinates data to vertex data and normal data and colour data
      if (texturecoordinate_size != 0) memcpy(&pBuffer[vertices.size() + normals.size() + colours.size()], textureCoordinates.data(), texturecoordinate_size);

    glUnmapBuffer(GL_ARRAY_BUFFER);


    // Index buffer

    // create index buffer
    //glGenBuffers(1, &cubeIBO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO);

    // For constrast, instead of glBufferSubData and glMapBuffer,
    // we can directly supply the data in one-shot
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUMBER_OF_CUBE_INDICES*sizeof(GLubyte), s_cubeIndices, GL_STATIC_DRAW);


    // We are now finished and are ready to unbind

    // Unbind the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Disable texture coordinate information
    if (texturecoordinate_size != 0) glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // Disable colour information
    if (colour_size != 0) glDisableClientState(GL_COLOR_ARRAY);

    // Disable normal information
    if (normal_size != 0) glDisableClientState(GL_NORMAL_ARRAY);

    // Disable vertex information
    glDisableClientState(GL_VERTEX_ARRAY);

    bIsCompiled = true;
    bIs2D = true;
  }

  void cStaticVertexBufferObject::Destroy()
  {
    //glDeleteBuffers(1, &cubeIBO);
    glDeleteBuffers(1, &bufferID);

    bufferID = 0;

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

  void cStaticVertexBufferObject::Bind()
  {
    assert(IsCompiled());
    assert(!bIs2D);

    // Activate the VBOs to draw
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO);

    // Enable vertex information
    glEnableClientState(GL_VERTEX_ARRAY);

    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));

    // Enable normal information
    if (normal_size > 0) {
      // Describe to OpenGL where the normal coordinate data is in the buffer
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(vertex_size));
    }

    // Enable colour information
    if (colour_size > 0) {
      // Describe to OpenGL where the colour coordinate data is in the buffer
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(vertex_size + normal_size));
    }

    // Enable texture coordinate information
    if (texturecoordinate_size > 0) {
      //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      // Describe to OpenGL where the texture coordinate data is in the buffer
      //glTexCoordPointer(nTextureUnits * 2, GL_FLOAT, 0, BUFFER_OFFSET(vertex_size + normal_size + colour_size));

      for (size_t i = 0; i < nTextureUnits; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glClientActiveTexture(GL_TEXTURE0 + i);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 2 * nTextureUnits * sizeof(GL_FLOAT), BUFFER_OFFSET(vertex_size + normal_size + colour_size + (2 * i * sizeof(GL_FLOAT))));
      }

      // This is not normal!  Instead of sending 2 * 2 texture coordinates for every point, we send
      // 4 * 1 texture coordinates for every point, sending them all in one texture unit
      // if (texturecoordinate_size > 1) {
      //   glActiveTexture(GL_TEXTURE1);
      //   glClientActiveTexture(GL_TEXTURE1);
      //   glEnable(GL_TEXTURE_2D);
      //   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      //   glTexCoordPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(vertex_size + normal_size colour_size + (2 * sizeof(GL_FLOAT))));
      // }
    }
  }

  void cStaticVertexBufferObject::Bind2D()
  {
    assert(IsCompiled());
    assert(bIs2D);

    // Activate the VBOs to draw
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO);

    // Enable vertex information
    glEnableClientState(GL_VERTEX_ARRAY);

    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));

    // Enable normal information
    if (normal_size > 0) {
      // Describe to OpenGL where the normal coordinate data is in the buffer
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(vertex_size));
    }

    // Enable colour information
    if (colour_size > 0) {
      // Describe to OpenGL where the colour coordinate data is in the buffer
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(vertex_size + normal_size));
    }

    // Enable texture coordinate information
    if (texturecoordinate_size > 0) {
      //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      // Describe to OpenGL where the texture coordinate data is in the buffer
      //glTexCoordPointer(nTextureUnits * 2, GL_FLOAT, 0, BUFFER_OFFSET(vertex_size + normal_size + colour_size));

      for (size_t i = 0; i < nTextureUnits; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glClientActiveTexture(GL_TEXTURE0 + i);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 2 * nTextureUnits * sizeof(GL_FLOAT), BUFFER_OFFSET(vertex_size + normal_size + colour_size + (2 * i * sizeof(GL_FLOAT))));
      }

      // This is not normal!  Instead of sending 2 * 2 texture coordinates for every point, we send
      // 4 * 1 texture coordinates for every point, sending them all in one texture unit
      // if (texturecoordinate_size > 1) {
      //   glActiveTexture(GL_TEXTURE1);
      //   glClientActiveTexture(GL_TEXTURE1);
      //   glEnable(GL_TEXTURE_2D);
      //   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      //   glTexCoordPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(vertex_size + normal_size colour_size + (2 * sizeof(GL_FLOAT))));
      // }
    }
  }

  void cStaticVertexBufferObject::Unbind()
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

    // Unbind the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void cStaticVertexBufferObject::RenderGeometry(GLenum geometryType)
  {
    assert(IsCompiled());
    assert(!bIs2D);

    if (indices.empty()) {
      // Draw this many vertices of type specified by geometryType (GL_LINES, GL_TRIANGLES, strips, quads, etc.
      const size_t nVertices = vertices.size() / 3;
      glDrawArrays(geometryType, 0, nVertices);
    } else {
      glDrawElements(geometryType, indices.size(), GL_UNSIGNED_SHORT,  indices.data());
    }
  }

  void cStaticVertexBufferObject::RenderGeometry2D(GLenum geometryType)
  {
    assert(IsCompiled());
    assert(bIs2D);

    if (indices.empty()) {
      // Draw this many vertices of type specified by geometryType (GL_LINES, GL_TRIANGLES, strips, quads, etc.
      const size_t nVertices = vertices.size() / 2;
      glDrawArrays(geometryType, 0, nVertices);
    } else {
      glDrawElements(geometryType, indices.size(), GL_UNSIGNED_SHORT,  indices.data());
    }
  }

  void cStaticVertexBufferObject::RenderLines()
  {
    RenderGeometry(GL_LINES);
  }

  void cStaticVertexBufferObject::RenderTriangles()
  {
    RenderGeometry(GL_TRIANGLES);
  }

  void cStaticVertexBufferObject::RenderTriangleStrip()
  {
    RenderGeometry(GL_TRIANGLE_STRIP);
  }

  void cStaticVertexBufferObject::RenderQuads()
  {
    RenderGeometry(GL_QUADS);
  }

  void cStaticVertexBufferObject::RenderQuadStrip()
  {
    RenderGeometry(GL_QUAD_STRIP);
  }


  void cStaticVertexBufferObject::RenderLines2D()
  {
    RenderGeometry2D(GL_LINES);
  }

  void cStaticVertexBufferObject::RenderQuads2D()
  {
    RenderGeometry2D(GL_QUADS);
  }
}
