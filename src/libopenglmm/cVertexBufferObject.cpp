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
#include <list>
#include <vector>

// SDL headers
#include <SDL/SDL_image.h>

// libopenglmm headers
#include <libopenglmm/cSystem.h>
#include <libopenglmm/cVertexBufferObject.h>
#include <libopenglmm/opengl.h>

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
    #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
    vertexArrayObjectID(0),
    #endif
    bufferID(0)
  {
  }

  void cStaticVertexBufferObject::SetData(const std::vector<float>& _data, const cBufferSizes& _bufferSizes)
  {
    assert(sizeof(float) == sizeof(GLfloat));
    assert(!_data.empty());
    assert(!IsCompiled());

    // Set our data
    data = _data;

    // Set our item counts
    bufferSizes = _bufferSizes;

    // Set our byte sizes which finally get sent to OpenGL
    vertex_size = bufferSizes.nVertexCount * bufferSizes.nVerticesPerPoint * sizeof(GLfloat);
    normal_size = bufferSizes.nVertexCount * bufferSizes.nNormalsPerPoint * sizeof(GLfloat);
    colour_size = bufferSizes.nVertexCount * bufferSizes.nColoursPerPoint * sizeof(GLfloat);
    texturecoordinate_size = bufferSizes.nVertexCount * bufferSizes.nTextureCoordinatesPerPoint * sizeof(GLfloat);
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

    const size_t nVertices = bufferSizes.nVertexCount;
    assert(nVertices != 0);
    nTextureUnits = bufferSizes.nTextureCoordinatesPerPoint / 2;

    //std::cout<<"cStaticVertexBufferObject::Compile nVertices="<<nVertices<<" nTextureUnits="<<nTextureUnits<<" indices="<<indices.size()<<" glGetError="<<system.GetErrorString()<<std::endl;

    #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
    // Create a vertex array object
    glGenVertexArrays(1, &vertexArrayObjectID);

    // Bind our vertex array object
    glBindVertexArray(vertexArrayObjectID);
    #endif

    // Create a new buffer
    glGenBuffers(1, &bufferID);
    //std::cout<<"cStaticVertexBufferObject::Compile glGenBuffers glGetError="<<system.GetErrorString()<<", bufferID="<<bufferID<<std::endl;
    assert(bufferID != 0);

    // Bind the buffer object to use
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);

    // Allocate enough memory for the whole buffer
    // Also GL_DYNAMIC_DRAW and GL_STREAM_DRAW
    glBufferData(GL_ARRAY_BUFFER, vertex_size + normal_size + colour_size + texturecoordinate_size, nullptr, GL_STATIC_DRAW);

    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
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
    #endif

    // http://www.informit.com/articles/article.aspx?p=1377833&seqNum=7

    std::cout<<"cStaticVertexBufferObject::Compile b glGetError="<<opengl::cSystem::GetErrorString()<<std::endl;
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_size + normal_size + colour_size + texturecoordinate_size, data.data());

    /*// Set the buffer data
    GLvoid* pVoid = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    assert(pVoid != nullptr);

      float* pBuffer = (float*)pVoid;

      // Transfer the vertex data to the VBO
      memcpy(pBuffer, vertices.data(), vertex_size);

      // Append normal data to vertex data
      if (normal_size != 0) memcpy(&pBuffer[nVerticesSize], normals.data(), normal_size);

      // Append colour data to vertex data and normal data
      if (colour_size != 0) memcpy(&pBuffer[nVerticesSize + bufferSizes.nNormalsSize], colours.data(), colour_size);

      // Append texture coordinates data to vertex data and normal data and colour data
      if (texturecoordinate_size != 0) memcpy(&pBuffer[nVerticesSize + bufferSizes.nNormalsSize + bufferSizes.nColoursSize], textureCoordinates.data(), texturecoordinate_size);

    glUnmapBuffer(GL_ARRAY_BUFFER);*/


    // Index buffer

    // create index buffer
    //glGenBuffers(1, &cubeIBO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO);

    // For constrast, instead of glBufferSubData and glMapBuffer,
    // we can directly supply the data in one-shot
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUMBER_OF_CUBE_INDICES*sizeof(GLubyte), s_cubeIndices, GL_STATIC_DRAW);

    #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
    const size_t nVertexSize = bufferSizes.nVerticesPerPoint;
    const size_t nNormalSize = bufferSizes.nNormalsPerPoint;
    const size_t nColourSize = bufferSizes.nColoursPerPoint;
    const size_t nTextureCoordinateSize = bufferSizes.nTextureCoordinatesPerPoint;

    const size_t nVertexSizeBytes = nVertexSize * sizeof(GLfloat);
    const size_t nNormalSizeBytes = nNormalSize * sizeof(GLfloat);
    const size_t nColourSizeBytes = nColourSize * sizeof(GLfloat);

    const size_t nStrideBytes = (nVertexSize + nNormalSize + nColourSize + nTextureCoordinateSize) * sizeof(GLfloat);

    unsigned int shaderAttribute = 0;

    // Tell the shader where the vertices are
    glVertexAttribPointer(shaderAttribute, nVertexSize, GL_FLOAT, GL_FALSE, nStrideBytes, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(shaderAttribute);
    shaderAttribute++;

    // Tell the shader where the normals are
    if (normal_size != 0) {
      glVertexAttribPointer(shaderAttribute, nNormalSize, GL_FLOAT, GL_FALSE, nStrideBytes, BUFFER_OFFSET(nVertexSizeBytes));
      glEnableVertexAttribArray(shaderAttribute);
      shaderAttribute++;
    }

    // Tell the shader where the colours are
    if (colour_size != 0) {
      glVertexAttribPointer(shaderAttribute, nColourSize, GL_FLOAT, GL_FALSE, nStrideBytes, BUFFER_OFFSET(nVertexSizeBytes + nNormalSizeBytes));
      glEnableVertexAttribArray(shaderAttribute);
      shaderAttribute++;
    }

    // Tell the shader where the texture coordinates are
    if (texturecoordinate_size != 0) {
      glVertexAttribPointer(shaderAttribute, nTextureCoordinateSize, GL_FLOAT, GL_FALSE, nStrideBytes, BUFFER_OFFSET(nVertexSizeBytes + nNormalSizeBytes + nColourSizeBytes));
      glEnableVertexAttribArray(shaderAttribute);
      shaderAttribute++;
    }
    #endif

    // We are now finished and are ready to unbind

    #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
    // Unbind the vertex array
    glBindVertexArray(0);
    #endif

    // Unbind the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    // Disable texture coordinate information
    if (texturecoordinate_size != 0) glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // Disable colour information
    if (colour_size != 0) glDisableClientState(GL_COLOR_ARRAY);

    // Disable normal information
    if (normal_size != 0) glDisableClientState(GL_NORMAL_ARRAY);

    // Disable vertex information
    glDisableClientState(GL_VERTEX_ARRAY);
    #endif

    bIsCompiled = true;
    bIs2D = false;
  }

  void cStaticVertexBufferObject::Compile2D(const cSystem& system)
  {
    assert(!IsCompiled());

    const size_t nVertices = bufferSizes.nVertexCount / 2;
    assert(nVertices != 0);
    nTextureUnits = bufferSizes.nTextureCoordinatesPerPoint / 2;

    //std::cout<<"cStaticVertexBufferObject::Compile2D nVertices="<<nVertices<<" nTextureUnits="<<nTextureUnits<<" indices="<<indices.size()<<" glGetError="<<system.GetErrorString()<<std::endl;

    // Create a new buffer
    glGenBuffers(1, &bufferID);
    //std::cout<<"cStaticVertexBufferObject::Compile2D glGenBuffers glGetError="<<system.GetErrorString()<<", bufferID="<<bufferID<<std::endl;
    assert(bufferID != 0);

    // Bind the buffer object to use
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);

    // Allocate enough memory for the whole buffer
    // Also GL_DYNAMIC_DRAW and GL_STREAM_DRAW
    glBufferData(GL_ARRAY_BUFFER, vertex_size + normal_size + colour_size + texturecoordinate_size, nullptr, GL_STATIC_DRAW);

    #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
    const size_t nVertexSize = bufferSizes.nVerticesPerPoint;
    const size_t nNormalSize = bufferSizes.nNormalsPerPoint;
    const size_t nColourSize = bufferSizes.nColoursPerPoint;
    const size_t nTextureCoordinateSize = bufferSizes.nTextureCoordinatesPerPoint;

    const size_t stride = nVertexSize + nNormalSize + nColourSize + nTextureCoordinateSize;

    unsigned int shaderAttribute = 0;

    // Tell the shader where the vertices are
    glVertexAttribPointer(shaderAttribute, nVertexSize, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(shaderAttribute);
    shaderAttribute++;

    // Tell the shader where the normals are
    if (normal_size != 0) {
      glVertexAttribPointer(shaderAttribute, nNormalSize, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(nVertexSize));
      glEnableVertexAttribArray(shaderAttribute);
      shaderAttribute++;
    }

    // Tell the shader where the colours are
    if (colour_size != 0) {
      glVertexAttribPointer(shaderAttribute, nColourSize, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(nVertexSize + nNormalSize));
      glEnableVertexAttribArray(shaderAttribute);
      shaderAttribute++;
    }

    // Tell the shader where the texture coordinates are
    if (texturecoordinate_size != 0) {
      glVertexAttribPointer(shaderAttribute, nTextureCoordinateSize, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(nVertexSize + nNormalSize + nColourSize));
      glEnableVertexAttribArray(shaderAttribute);
      shaderAttribute++;
    }
    #else
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
    #endif

    // Set the buffer data
    GLvoid* pVoid = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    assert(pVoid != nullptr);

      float* pBuffer = (float*)pVoid;

      // Transfer the vertex data to the VBO
      memcpy(pBuffer, data.data(), vertex_size + normal_size + colour_size + texturecoordinate_size);

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

    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    // Disable texture coordinate information
    if (texturecoordinate_size != 0) glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // Disable colour information
    if (colour_size != 0) glDisableClientState(GL_COLOR_ARRAY);

    // Disable normal information
    if (normal_size != 0) glDisableClientState(GL_NORMAL_ARRAY);

    // Disable vertex information
    glDisableClientState(GL_VERTEX_ARRAY);
    #endif

    bIsCompiled = true;
    bIs2D = true;
  }

  void cStaticVertexBufferObject::Destroy()
  {
    //glDeleteBuffers(1, &cubeIBO);
    glDeleteBuffers(1, &bufferID);

    bufferID = 0;

    #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glDeleteVertexArrays(1, &vertexArrayObjectID);

    vertexArrayObjectID = 0;
    #endif

    bIsCompiled = false;
    bIs2D = false;

    data.clear();
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


    #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
    // Activate the vertex array
    glBindVertexArray(vertexArrayObjectID);
    #else
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
    #endif
  }

  void cStaticVertexBufferObject::Bind2D()
  {
    assert(IsCompiled());
    assert(bIs2D);

    #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
    // Activate the vertex array
    glBindVertexArray(vertexArrayObjectID);
    #else
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
    #endif
  }

  void cStaticVertexBufferObject::Unbind()
  {
    assert(IsCompiled());

    #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Disable the vertex array
    glBindVertexArray(0);
    #else
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
    #endif
  }

  void cStaticVertexBufferObject::RenderGeometry(GLenum geometryType)
  {
    assert(IsCompiled());
    assert(!bIs2D);

    if (indices.empty()) {
      // Draw this many vertices of type specified by geometryType (GL_LINES, GL_TRIANGLES, strips, quads, etc.)
      glDrawArrays(geometryType, 0, bufferSizes.nVertexCount);
    } else {
      glDrawElements(geometryType, indices.size(), GL_UNSIGNED_SHORT,  indices.data());
    }
  }

  void cStaticVertexBufferObject::RenderGeometry2D(GLenum geometryType)
  {
    assert(IsCompiled());
    assert(bIs2D);

    if (indices.empty()) {
      // Draw this many vertices of type specified by geometryType (GL_LINES, GL_TRIANGLES, strips, quads, etc.)
      glDrawArrays(geometryType, 0, bufferSizes.nVertexCount);
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

  #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
  void cStaticVertexBufferObject::RenderQuads()
  {
    RenderGeometry(GL_QUADS);
  }

  void cStaticVertexBufferObject::RenderQuadStrip()
  {
    RenderGeometry(GL_QUAD_STRIP);
  }
  #endif


  void cStaticVertexBufferObject::RenderLines2D()
  {
    RenderGeometry2D(GL_LINES);
  }

  #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
  void cStaticVertexBufferObject::RenderQuads2D()
  {
    RenderGeometry2D(GL_QUADS);
  }
  #endif
}
