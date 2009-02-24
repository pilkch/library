#ifndef CVERTEXBUFFEROBJECT_H
#define CVERTEXBUFFEROBJECT_H

namespace breathe
{
  namespace render
  {
    template <class T>
    class cVertexBufferObjectArray
    {
    public:
      cVertexBufferObjectArray() { uiOffset = 0; }

      size_t GetDataSize() const { return vData.size(); }

      void SetOffset(size_t uiOffset);

      T* GetData() { return vData.size() ? &vData[0] : nullptr; }
      void SetData(const std::vector<T>& inData) { vData.assign(inData.begin(), inData.end()); }

      size_t uiOffset;

      std::vector<T> vData;
    };

    template <class T>
    inline void cVertexBufferObjectArray<T>::SetOffset(size_t _uiOffset)
    {
      ASSERT(_uiOffset < vData.size());
      uiOffset = _uiOffset;
    }

    class cVertexBufferObject : protected cRenderable
    {
    public:
      cVertexBufferObject();
      virtual ~cVertexBufferObject();

      void Destroy();
      void Init();
      size_t Render();
      void Update(sampletime_t currentTime) {}

      cVertexBufferObjectArray<math::cVec3> pVertex;
      cVertexBufferObjectArray<math::cVec3> pNormal;
      cVertexBufferObjectArray<math::cVec2> pTextureCoord;

    private:
      size_t uiVertices;

      size_t uiOffsetTextureUnit0;
      size_t uiOffsetTextureUnit1;
      size_t uiOffsetTextureUnit2;

      // Note: one buffer per cVertexBufferObject,
      // but with multiple offsets for each cVertexBufferObjectArray
      GLuint bufferID;
    };

    typedef cSmartPtr<cVertexBufferObject> cVertexBufferObjectRef;
















    // http://playcontrol.net/ewing/jibberjabber/opengl_vertex_buffer_object.html

    class cStaticVertexBuffer
    {
    public:
      cStaticVertexBuffer();

      bool IsCompiled() const { return bIsCompiled; }

      void SetVertices(const std::vector<float>& vertices);
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
      std::vector<float> textureCoordinates;
      std::vector<uint16_t> indices;

      GLsizeiptr vertex_size;
      GLsizeiptr texturecoordinate_size;
      GLsizeiptr indices_size;

      // Note: one buffer per cVertexBufferObject,
      // but with multiple offsets for each cVertexBufferObjectArray
      GLuint bufferID;
    };
  }
}

#endif // CVERTEXBUFFEROBJECT_H
