#include <cstring> // For memcpy
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include <list>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <memory>

// Writing to and from a text file
#include <iostream>
#include <fstream>

// Boost headers
#include <boost/shared_ptr.hpp>

#include <GL/GLee.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/geometry.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/util/base.h>

#include <breathe/render/cRender.h>
#include <breathe/render/cVertexBufferObject.h>

namespace breathe
{
  namespace render
  {
    #define BUFFER_OFFSET(i) ((char *)NULL + (i))


    // TODO: Add index arrays and use GL_TRIANGLE_STRIPS


    // Index Array
    //glEnableClientState( GL_NORMAL_ARRAY );
    //glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, index_buf);
    // Don't need this apparently? glIndexPointer(GL_UNSIGNED_SHORT, 0, BUFFER_OFFSET(pIndex.uiOffset));



    //glDrawRangeElements( GL_TRIANGLES, 0, uiIndicies, uiIndicies, GL_UNSIGNED_INT, BUFFER_OFFSET(pIndex.uiOffset));
    //glDrawElements(GL_TRIANGLES, uiIndicies, GL_UNSIGNED_SHORT, BUFFER_OFFSET(pIndex.uiOffset));



    // *** cStaticVertexBuffer

    cStaticVertexBuffer::cStaticVertexBuffer() :
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

    void cStaticVertexBuffer::SetVertices(const std::vector<float>& _vertices)
    {
      ASSERT(!_vertices.empty());
      ASSERT(!IsCompiled());

      vertices = _vertices;
      vertex_size = vertices.size() * sizeof(GLfloat);
    }

    void cStaticVertexBuffer::SetNormals(const std::vector<float>& _normals)
    {
      ASSERT(!_normals.empty());
      ASSERT(!IsCompiled());

      normals = _normals;
      normal_size = normals.size() * sizeof(GLfloat);
    }

    void cStaticVertexBuffer::SetColours(const std::vector<float>& _colours)
    {
      ASSERT(!_colours.empty());
      ASSERT(!IsCompiled());

      colours = _colours;
      colour_size = colours.size() * sizeof(GLfloat);
    }

    void cStaticVertexBuffer::SetTextureCoordinates(const std::vector<float>& _textureCoordinates)
    {
      ASSERT(!_textureCoordinates.empty());
      ASSERT(!IsCompiled());

      textureCoordinates = _textureCoordinates;
      texturecoordinate_size = textureCoordinates.size() * sizeof(GLfloat);
    }

    void cStaticVertexBuffer::SetIndices(const std::vector<uint16_t>& _indices)
    {
      ASSERT(!_indices.empty());
      ASSERT(!IsCompiled());

      indices = _indices;
      indices_size = indices.size() * sizeof(GLushort);
    }

    void cStaticVertexBuffer::Compile()
    {
      ASSERT(!IsCompiled());

      const size_t nVertices = vertices.size() / 3;
      const size_t nTextureCoordinates = textureCoordinates.size() / 2;
      ASSERT(nVertices != 0);
      if (nTextureCoordinates == (nVertices + nVertices + nVertices)) nTextureUnits = 3;
      else if (nTextureCoordinates == (nVertices + nVertices)) nTextureUnits = 2;
      else if (nTextureCoordinates == nVertices) nTextureUnits = 1;
      else nTextureUnits = 0;

      LOG<<"cStaticVertexBuffer::Compile nVertices="<<nVertices<<" nTextureUnits="<<nTextureUnits<<" glGetError="<<pRender->GetErrorString()<<std::endl;

      // Create a new buffer
      glGenBuffers(1, &bufferID);
      LOG<<"cStaticVertexBuffer::Compile glGenBuffers glGetError="<<pRender->GetErrorString()<<", bufferID="<<bufferID<<std::endl;
      ASSERT(bufferID != 0);

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
      ASSERT(pVoid != nullptr);

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

    void cStaticVertexBuffer::Destroy()
    {
      //glDeleteBuffers(1, &cubeIBO);
      glDeleteBuffers(1, &bufferID);
    }

    void cStaticVertexBuffer::Bind()
    {
      ASSERT(IsCompiled());

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

    void cStaticVertexBuffer::Unbind()
    {
      ASSERT(IsCompiled());

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

    void cStaticVertexBuffer::RenderGeometry(GLenum geometryType)
    {
      ASSERT(IsCompiled());

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

    void cStaticVertexBuffer::RenderLines()
    {
      RenderGeometry(GL_LINES);
    }

    void cStaticVertexBuffer::RenderTriangles()
    {
      RenderGeometry(GL_TRIANGLES);
    }

    void cStaticVertexBuffer::RenderTriangleStrip()
    {
      RenderGeometry(GL_TRIANGLE_STRIP);
    }

    void cStaticVertexBuffer::RenderQuads()
    {
      RenderGeometry(GL_QUADS);
    }

    void cStaticVertexBuffer::RenderQuadStrip()
    {
      RenderGeometry(GL_QUAD_STRIP);
    }




    // *** cDynamicVertexBuffer

    cDynamicVertexBuffer::cDynamicVertexBuffer() :
      bIsCompiled(false),
      vertex_size(0),
      normal_size(0),
      colour_size(0),
      texturecoordinate_size(0),
      indices_size(0),
      nTextureUnits(0)
    {
    }

    void cDynamicVertexBuffer::SetVertices(const std::vector<float>& _vertices)
    {
      bIsCompiled = false;

      vertices = _vertices;
      vertex_size = vertices.size() * sizeof(GLfloat);
    }

    void cDynamicVertexBuffer::SetNormals(const std::vector<float>& _normals)
    {
      bIsCompiled = false;

      normals = _normals;
      normal_size = normals.size() * sizeof(GLfloat);
    }

    void cDynamicVertexBuffer::SetColours(const std::vector<float>& _colours)
    {
      bIsCompiled = false;

      colours = _colours;
      colour_size = colours.size() * sizeof(GLfloat);
    }

    void cDynamicVertexBuffer::SetTextureCoordinates(const std::vector<float>& _textureCoordinates)
    {
      bIsCompiled = false;

      textureCoordinates = _textureCoordinates;
      texturecoordinate_size = textureCoordinates.size() * sizeof(GLfloat);
    }

    void cDynamicVertexBuffer::SetIndices(const std::vector<uint16_t>& _indices)
    {
      bIsCompiled = false;

      indices = _indices;
      indices_size = indices.size() * sizeof(GLushort);
    }

    void cDynamicVertexBuffer::Compile()
    {
      ASSERT(!IsCompiled());

      const size_t nVertices = vertices.size() / 3;
      const size_t nTextureCoordinates = textureCoordinates.size() / 2;
      ASSERT(nVertices != 0);
      if (nTextureCoordinates == (nVertices + nVertices + nVertices)) nTextureUnits = 3;
      else if (nTextureCoordinates == (nVertices + nVertices)) nTextureUnits = 2;
      else if (nTextureCoordinates == nVertices) nTextureUnits = 1;
      else nTextureUnits = 0;

      LOG<<"cDynamicVertexBuffer::Compile nVertices="<<nVertices<<" nTextureUnits="<<nTextureUnits<<std::endl;

      bIsCompiled = true;
    }

    void cDynamicVertexBuffer::Destroy()
    {
      bIsCompiled = false;
    }

    void cDynamicVertexBuffer::Bind()
    {
      ASSERT(IsCompiled());
    }

    void cDynamicVertexBuffer::Unbind()
    {
      ASSERT(IsCompiled());
    }

    void cDynamicVertexBuffer::RenderGeometry(GLenum geometryType)
    {
      ASSERT(IsCompiled());

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
    }

    void cDynamicVertexBuffer::RenderLines()
    {
      RenderGeometry(GL_LINES);
    }

    void cDynamicVertexBuffer::RenderTriangles()
    {
      RenderGeometry(GL_TRIANGLES);
    }

    void cDynamicVertexBuffer::RenderTriangleStrip()
    {
      RenderGeometry(GL_TRIANGLE_STRIP);
    }

    void cDynamicVertexBuffer::RenderQuads()
    {
      RenderGeometry(GL_QUADS);
    }

    void cDynamicVertexBuffer::RenderQuadStrip()
    {
      RenderGeometry(GL_QUAD_STRIP);
    }
  }
}
