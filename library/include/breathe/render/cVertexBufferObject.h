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

			void SetData(const std::vector<T>& inData) { vData.assign(inData.begin(), inData.end()); }
			T* GetData() { return vData.size() ? &vData[0] : NULL; }
			unsigned int GetDataSize() { return vData.size(); }

			unsigned int uiOffset;

			std::vector<T> vData;
		};

		class cVertexBufferObject : protected cRenderable
		{
		public:
			cVertexBufferObject();
			virtual ~cVertexBufferObject();

			void Destroy();
			void Init();
			unsigned int Render();
			void Update(sampletime_t currentTime) {}

			cVertexBufferObjectArray<math::cVec3> pVertex;
			cVertexBufferObjectArray<math::cVec3> pNormal;
			cVertexBufferObjectArray<math::cVec2> pTextureCoord;

		private:
			unsigned int uiVertices;

			unsigned int uiOffsetTextureUnit0;
			unsigned int uiOffsetTextureUnit1;
			unsigned int uiOffsetTextureUnit2;

			// Note: one buffer per cVertexBufferObject,
			// but with multiple offsets for each cVertexBufferObjectArray
			GLuint bufferID;
		};

    typedef cSmartPtr<cVertexBufferObject> cVertexBufferObjectRef;
	}
}

#endif // CVERTEXBUFFEROBJECT_H
