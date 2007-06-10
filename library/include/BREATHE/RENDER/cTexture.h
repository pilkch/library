#ifndef CTEXTURE_H
#define CTEXTURE_H

struct SDL_Surface;

namespace BREATHE
{
	namespace RENDER
	{
		//Texture modes for materials
		const unsigned int TEXTURE_NONE=0;
		const unsigned int TEXTURE_NORMAL=1;
		const unsigned int TEXTURE_MASK=2;
		const unsigned int TEXTURE_BLEND=3;
		const unsigned int TEXTURE_DETAIL=4;
		const unsigned int TEXTURE_CUBEMAP=5;
		
		const unsigned int TEXTURE_RGBA=0;
		const unsigned int TEXTURE_HEIGHTMAP=1;
		const unsigned int TEXTURE_FRAMEBUFFEROBJECT=2;

		class cTexture
		{
		public:
			cTexture();
			~cTexture();
			
			bool Load(std::string sFilename);

			// pData <--> surface -> OpenGL texture
			void CopyFromDataToSurface();

			void CopyFromSurfaceToData(unsigned int w, unsigned int h);
			void CopyFromSurfaceToData();

			void CopyFromSurfaceToTexture();


			bool SaveToBMP(std::string sFilename);
			
			void Transform(float& u, float& v);

			virtual void Create();
			void Destroy();
			void Reload();


			unsigned int uiTextureAtlas;
			unsigned int uiTexture;
			std::string sFilename;
			
			unsigned int uiWidth;
			unsigned int uiHeight;
			unsigned int uiMode;

			float fScale;
			float fU;
			float fV;

			SDL_Surface* surface;
			unsigned char* pData;
		};

		inline void cTexture::Transform(float& u, float& v)
		{
			u = u * fScale + fU;
			v = v * fScale + fV;
		}

		class cTextureFrameBufferObject : public cTexture
		{
		public:
			cTextureFrameBufferObject();
			~cTextureFrameBufferObject();
			void Create();

			unsigned int uiFBO;					// Our handle to the FBO
			unsigned int uiFBODepthBuffer;	// Our handle to the depth render buffer
		};
	}
}

#endif //CTEXTURE_H