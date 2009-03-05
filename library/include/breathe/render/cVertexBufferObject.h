#ifndef CVERTEXBUFFEROBJECT_H
#define CVERTEXBUFFEROBJECT_H

namespace breathe
{
  namespace render
  {
    // http://playcontrol.net/ewing/jibberjabber/opengl_vertex_buffer_object.html

    class cStaticVertexBuffer
    {
    public:
      cStaticVertexBuffer();

      bool IsCompiled() const { return bIsCompiled; }

      void SetVertices(const std::vector<float>& vertices);
      void SetNormals(const std::vector<float>& normals);
      void SetColours(const std::vector<float>& colours);
      void SetTextureCoordinates(const std::vector<float>& textureCoordinates);
      void SetIndices(const std::vector<uint16_t>& indices);

      void Compile();
      void Destroy();

      void Bind();
      void Unbind();

      void RenderLines();
      void RenderTriangles();
      void RenderTriangleStrip();
      void RenderQuads();
      void RenderQuadStrip();

    private:
      void RenderGeometry(GLenum geometryType);

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

      // Note: one buffer per cVertexBufferObject,
      // but with multiple offsets for each cVertexBufferObjectArray
      GLuint bufferID;
    };

    typedef cStaticVertexBuffer cVertexBufferObject;
    typedef cSmartPtr<cVertexBufferObject> cVertexBufferObjectRef;
  }
}

#endif // CVERTEXBUFFEROBJECT_H
