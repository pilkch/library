#ifndef CVERTEXBUFFEROBJECT_H
#define CVERTEXBUFFEROBJECT_H

struct SDL_Surface;

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
  }
}

#endif // CVERTEXBUFFEROBJECT_H
