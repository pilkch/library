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
    assert(sizeof(float) == sizeof(GLfloat)); // Vertices, normals, colours and texture coordinates
    assert(sizeof(uint16_t) == sizeof(GLushort)); // Indices
  }

  void cStaticVertexBufferObject::SetData(cGeometryDataPtr _pGeometryDataPtr)
  {
    assert(_pGeometryDataPtr != nullptr);
    assert(!_pGeometryDataPtr->vertices.empty());
    assert(!IsCompiled());

    // Set our data
    pGeometryDataPtr = _pGeometryDataPtr;

    // Set our byte sizes which finally get sent to OpenGL
    vertex_size = pGeometryDataPtr->nVertexCount * pGeometryDataPtr->nVerticesPerPoint * sizeof(GLfloat);
    normal_size = pGeometryDataPtr->nVertexCount * pGeometryDataPtr->nNormalsPerPoint * sizeof(GLfloat);
    colour_size = pGeometryDataPtr->nVertexCount * pGeometryDataPtr->nColoursPerPoint * sizeof(GLfloat);
    texturecoordinate_size = pGeometryDataPtr->nVertexCount * pGeometryDataPtr->nTextureCoordinatesPerPoint * sizeof(GLfloat);
    indices_size = pGeometryDataPtr->indices.size() * sizeof(GLushort);
  }

  size_t cStaticVertexBufferObject::GetApproximateTriangleCount() const
  {
    return (pGeometryDataPtr != nullptr) ? (pGeometryDataPtr->nVertexCount / 4) : 0;
  }

  void cStaticVertexBufferObject::Compile(const cSystem& system)
  {
    assert(!IsCompiled());

    const size_t nVertices = pGeometryDataPtr->nVertexCount;
    assert(nVertices != 0);
    nTextureUnits = pGeometryDataPtr->nTextureCoordinatesPerPoint / 2;

    std::cout<<"cStaticVertexBufferObject::Compile nVertices="<<nVertices<<" nTextureUnits="<<nTextureUnits<<" indices="<<pGeometryDataPtr->indices.size()<<std::endl;

    #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
    // Create a vertex array object
    glGenVertexArrays(1, &vertexArrayObjectID);

    // Bind our vertex array object
    glBindVertexArray(vertexArrayObjectID);
    #endif

    std::cout<<"cStaticVertexBufferObject::Compile glGetError="<<system.GetErrorString()<<std::endl;
    // Create a new buffer
    glGenBuffers(1, &bufferID);
    std::cout<<"cStaticVertexBufferObject::Compile glGenBuffers glGetError="<<system.GetErrorString()<<", bufferID="<<bufferID<<std::endl;
    assert(bufferID != 0);

    // Bind the buffer object to use
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);

    std::cout<<"cStaticVertexBufferObject::Compile x glGetError="<<opengl::cSystem::GetErrorString()<<std::endl;
    // Allocate enough memory for the whole buffer
    // Also GL_DYNAMIC_DRAW and GL_STREAM_DRAW
    glBufferData(GL_ARRAY_BUFFER, vertex_size + normal_size + colour_size + texturecoordinate_size, nullptr, GL_STATIC_DRAW);

    const size_t nVertexSize = pGeometryDataPtr->nVerticesPerPoint;
    const size_t nNormalSize = pGeometryDataPtr->nNormalsPerPoint;
    const size_t nColourSize = pGeometryDataPtr->nColoursPerPoint;
    const size_t nTextureCoordinateSize = pGeometryDataPtr->nTextureCoordinatesPerPoint;

    const size_t nVertexSizeBytes = nVertexSize * sizeof(GLfloat);
    const size_t nNormalSizeBytes = nNormalSize * sizeof(GLfloat);
    const size_t nColourSizeBytes = nColourSize * sizeof(GLfloat);

    const size_t nStrideBytes = (nVertexSize + nNormalSize + nColourSize + nTextureCoordinateSize) * sizeof(GLfloat);

    std::cout<<"cStaticVertexBufferObject::Compile y glGetError="<<opengl::cSystem::GetErrorString()<<std::endl;
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glEnableClientState(GL_VERTEX_ARRAY);
      // Describe to OpenGL where the vertex data is in the buffer
      glVertexPointer(3, GL_FLOAT, nStrideBytes, BUFFER_OFFSET(0));
    glDisableClientState(GL_VERTEX_ARRAY);

    if (normal_size != 0) {
      glEnableClientState(GL_NORMAL_ARRAY);
        // Describe to OpenGL where the normal data is in the buffer
        glNormalPointer(GL_FLOAT, nStrideBytes, BUFFER_OFFSET(nVertexSizeBytes));
      glDisableClientState(GL_NORMAL_ARRAY);
    }

    if (colour_size != 0) {
      glEnableClientState(GL_COLOR_ARRAY);
        // Describe to OpenGL where the colour data is in the buffer
        glColorPointer(4, GL_FLOAT, nStrideBytes, BUFFER_OFFSET(nVertexSizeBytes + nNormalSizeBytes));
      glDisableClientState(GL_COLOR_ARRAY);
    }

    if (texturecoordinate_size != 0) {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        // Describe to OpenGL where the texture coordinate data is in the buffer
        glTexCoordPointer(nTextureUnits, GL_FLOAT, nStrideBytes, BUFFER_OFFSET(nVertexSizeBytes + nNormalSizeBytes + nColourSizeBytes));
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    #endif

    // http://www.informit.com/articles/article.aspx?p=1377833&seqNum=7

    std::cout<<"cStaticVertexBufferObject::Compile b0 glGetError="<<opengl::cSystem::GetErrorString()<<std::endl;
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_size + normal_size + colour_size + texturecoordinate_size, pGeometryDataPtr->vertices.data());
    std::cout<<"cStaticVertexBufferObject::Compile b1 glGetError="<<opengl::cSystem::GetErrorString()<<std::endl;

    // Index buffer

    // create index buffer
    //glGenBuffers(1, &cubeIBO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO);

    // For constrast, instead of glBufferSubData and glMapBuffer,
    // we can directly supply the data in one-shot
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUMBER_OF_CUBE_INDICES*sizeof(GLubyte), s_cubeIndices, GL_STATIC_DRAW);

    #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
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

    std::cout<<"cStaticVertexBufferObject::Compile unbinding buffer, glGetError="<<system.GetErrorString()<<std::endl;

    #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
    // Unbind the vertex array
    glBindVertexArray(0);
    #endif

    // Unbind the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::cout<<"cStaticVertexBufferObject::Compile unbound buffer, glGetError="<<system.GetErrorString()<<std::endl;

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

    std::cout<<"cStaticVertexBufferObject::Compile returning, glGetError="<<system.GetErrorString()<<std::endl;

    bIsCompiled = true;
    bIs2D = false;
  }

  void cStaticVertexBufferObject::Compile2D(const cSystem& system)
  {
    assert(!IsCompiled());

    const size_t nVertices = pGeometryDataPtr->nVertexCount / 2;
    assert(nVertices != 0);
    nTextureUnits = pGeometryDataPtr->nTextureCoordinatesPerPoint / 2;

    //std::cout<<"cStaticVertexBufferObject::Compile2D nVertices="<<nVertices<<" nTextureUnits="<<nTextureUnits<<" indices="<<pGeometryDataPtr->indices.size()<<" glGetError="<<system.GetErrorString()<<std::endl;

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
    const size_t nVertexSize = pGeometryDataPtr->nVerticesPerPoint;
    const size_t nNormalSize = pGeometryDataPtr->nNormalsPerPoint;
    const size_t nColourSize = pGeometryDataPtr->nColoursPerPoint;
    const size_t nTextureCoordinateSize = pGeometryDataPtr->nTextureCoordinatesPerPoint;

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
      memcpy(pBuffer, pGeometryDataPtr->vertices.data(), vertex_size + normal_size + colour_size + texturecoordinate_size);

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

    pGeometryDataPtr.reset();

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

    #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
    // Activate the vertex array
    glBindVertexArray(vertexArrayObjectID);
    #else
    // Activate the VBOs to draw
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO);

    const size_t nVertexSize = pGeometryDataPtr->nVerticesPerPoint;
    const size_t nNormalSize = pGeometryDataPtr->nNormalsPerPoint;
    const size_t nColourSize = pGeometryDataPtr->nColoursPerPoint;
    const size_t nTextureCoordinateSize = pGeometryDataPtr->nTextureCoordinatesPerPoint;

    const size_t nVertexSizeBytes = nVertexSize * sizeof(GLfloat);
    const size_t nNormalSizeBytes = nNormalSize * sizeof(GLfloat);
    const size_t nColourSizeBytes = nColourSize * sizeof(GLfloat);

    const size_t nStrideBytes = (nVertexSize + nNormalSize + nColourSize + nTextureCoordinateSize) * sizeof(GLfloat);

    // Enable vertex information
    glEnableClientState(GL_VERTEX_ARRAY);

    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer(nVertexSize, GL_FLOAT, nStrideBytes, BUFFER_OFFSET(0));

    // Enable normal information
    if (normal_size > 0) {
      // Describe to OpenGL where the normal coordinate data is in the buffer
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, nStrideBytes, BUFFER_OFFSET(nVertexSizeBytes));
    }

    // Enable colour information
    if (colour_size > 0) {
      // Describe to OpenGL where the colour coordinate data is in the buffer
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(4, GL_FLOAT, nStrideBytes, BUFFER_OFFSET(nVertexSizeBytes + nNormalSizeBytes));
    }

    // Enable texture coordinate information
    if (texturecoordinate_size > 0) {
      for (size_t i = 0; i < nTextureUnits; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glClientActiveTexture(GL_TEXTURE0 + i);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, nStrideBytes, BUFFER_OFFSET(nVertexSizeBytes + nNormalSizeBytes + nColourSizeBytes + (2 * i * sizeof(GL_FLOAT))));
      }
    }
    #endif
  }

  void cStaticVertexBufferObject::Bind2D()
  {
    assert(IsCompiled());
    assert(bIs2D);

    Bind();
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

    if (pGeometryDataPtr->indices.empty()) {
      // Draw this many vertices of type specified by geometryType (GL_LINES, GL_TRIANGLES, strips, quads, etc.)
      glDrawArrays(geometryType, 0, pGeometryDataPtr->nVertexCount);
    } else {
      glDrawElements(geometryType, pGeometryDataPtr->indices.size(), GL_UNSIGNED_SHORT,  pGeometryDataPtr->indices.data());
    }
  }

  void cStaticVertexBufferObject::RenderGeometry2D(GLenum geometryType)
  {
    assert(IsCompiled());
    assert(bIs2D);

    if (pGeometryDataPtr->indices.empty()) {
      // Draw this many vertices of type specified by geometryType (GL_LINES, GL_TRIANGLES, strips, quads, etc.)
      glDrawArrays(geometryType, 0, pGeometryDataPtr->nVertexCount);
    } else {
      glDrawElements(geometryType, pGeometryDataPtr->indices.size(), GL_UNSIGNED_SHORT,  pGeometryDataPtr->indices.data());
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
