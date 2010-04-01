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
  #define BUFFER_OFFSET(i) ((char *)NULL + (i))

  // *** cStaticVertexBufferObject

  cStaticVertexBufferObject::cStaticVertexBufferObject() :
    bIsCompiled(false),
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
    assert(!_vertices.empty());
    assert(!IsCompiled());

    vertices = _vertices;
    vertex_size = vertices.size() * sizeof(GLfloat);
  }

  void cStaticVertexBufferObject::SetNormals(const std::vector<float>& _normals)
  {
    assert(!_normals.empty());
    assert(!IsCompiled());

    normals = _normals;
    normal_size = normals.size() * sizeof(GLfloat);
  }

  void cStaticVertexBufferObject::SetColours(const std::vector<float>& _colours)
  {
    assert(!_colours.empty());
    assert(!IsCompiled());

    colours = _colours;
    colour_size = colours.size() * sizeof(GLfloat);
  }

  void cStaticVertexBufferObject::SetTextureCoordinates(const std::vector<float>& _textureCoordinates)
  {
    assert(!_textureCoordinates.empty());
    assert(!IsCompiled());

    textureCoordinates = _textureCoordinates;
    texturecoordinate_size = textureCoordinates.size() * sizeof(GLfloat);
  }

  void cStaticVertexBufferObject::SetIndices(const std::vector<uint16_t>& _indices)
  {
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

    std::cout<<"cStaticVertexBufferObject::Compile nVertices="<<nVertices<<" nTextureUnits="<<nTextureUnits<<" glGetError="<<system.GetErrorString()<<std::endl;

    // Create a new buffer
    glGenBuffers(1, &bufferID);
    std::cout<<"cStaticVertexBufferObject::Compile glGenBuffers glGetError="<<system.GetErrorString()<<", bufferID="<<bufferID<<std::endl;
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
        glColorPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(vertex_size + normal_size));
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
  }

  void cStaticVertexBufferObject::Destroy()
  {
    //glDeleteBuffers(1, &cubeIBO);
    glDeleteBuffers(1, &bufferID);
  }

  void cStaticVertexBufferObject::Bind()
  {
    assert(IsCompiled());

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
      glColorPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(vertex_size + normal_size));
    }

    // Enable texture coordinate information
    if (texturecoordinate_size > 0) {
      //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      // Describe to OpenGL where the texture coordinate data is in the buffer
      //glTexCoordPointer(nTextureUnits * 2, GL_FLOAT, 0, BUFFER_OFFSET(vertex_size + normal_size + colour_size));

      glActiveTexture(GL_TEXTURE0_ARB);
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(nTextureUnits * 2, GL_FLOAT, 0, BUFFER_OFFSET(vertex_size + normal_size + colour_size));

      // This is not normal!  Instead of sending 2 * 2 texture coordinates for every point, we send
      // 4 * 1 texture coordinates for every point, sending them all in one texture unit
      // if (texturecoordinate_size > 1) {
      //   glActiveTexture(GL_TEXTURE1_ARB);
      //   glClientActiveTextureARB(GL_TEXTURE1_ARB);
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
    if (texturecoordinate_size != 0) glDisableClientState(GL_TEXTURE_COORD_ARRAY);

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

  #if 1
    // Draw this many vertices of type specified by geometryType (GL_LINES, GL_TRIANGLES, strips, quads, etc.
    const size_t nVertices = vertices.size() / 3;
    glDrawArrays(geometryType, 0, nVertices);
    //glDrawElements(geometryType, NUMBER_OF_CUBE_INDICES, GL_UNSIGNED_BYTE, (GLvoid*)((char*)NULL));
  #else
    // This is just for testing, immediate mode (yuk!)
    glBegin(geometryType);

      const size_t n = vertices.size();
      size_t v = 0;
      size_t t = 0;
      if (nTextureUnits == 2) {
        // Multitexturing
        while (v < n) {
          glMultiTexCoord2fARB(GL_TEXTURE0_ARB, textureCoordinates[t], textureCoordinates[t + 1]);
          glMultiTexCoord2fARB(GL_TEXTURE1_ARB, textureCoordinates[t + 2], textureCoordinates[t + 3]);
          glVertex3f(vertices[v], vertices[v + 1], vertices[v + 2]);

          v += 3;
          t += 4;
        };
      } else if (nTextureUnits == 1) {
        // Single texturing
        while (v < n) {
          glTexCoord2f(textureCoordinates[t], textureCoordinates[t + 1]);
          glVertex3f(vertices[v], vertices[v + 1], vertices[v + 2]);

          v += 3;
          t += 2;
        };
      } else {
        while (v < n) {
          glVertex3f(vertices[v], vertices[v + 1], vertices[v + 2]);

          v += 3;
        };
      }

    glEnd();
  #endif
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
}
