#ifndef CVERTEXBUFFEROBJECT_H
#define CVERTEXBUFFEROBJECT_H

struct SDL_Surface;

namespace BREATHE
{
	namespace RENDER
	{
		template <class T>
		class cVertexBufferObjectArray
		{
		public:
			cVertexBufferObjectArray() { uiOffset = 0; }

			void SetData(std::vector<T>& inData) { vData.assign(inData.begin(), inData.end()); }
			T* GetData() { return vData.size() ? &vData[0] : NULL; }
			unsigned int GetDataSize() { return vData.size(); }

			unsigned int uiOffset;

			std::vector<T> vData;
		};

		class cVertexBufferObject : protected cRenderable
		{
		public:
			cVertexBufferObject();
			~cVertexBufferObject();

			void Destroy();
			void Init();
			unsigned int Render();
			void Update(float fCurrentTime) {}

			cVertexBufferObjectArray<MATH::cVec3> pVertex;
			cVertexBufferObjectArray<MATH::cVec3> pNormal;
			cVertexBufferObjectArray<MATH::cVec2> pTextureCoord;

		private:
			unsigned int uiVertices;

			unsigned int uiOffsetTextureUnit0;
			unsigned int uiOffsetTextureUnit1;
			unsigned int uiOffsetTextureUnit2;

			// Note: one buffer per cVertexBufferObject, 
			// but with multiple offsets for each cVertexBufferObjectArray
			GLuint bufferID;
		};
	}
}

#endif //CVERTEXBUFFEROBJECT_H
