#ifndef CTEXTURE_H
#define CTEXTURE_H

struct SDL_Surface;

namespace BREATHE
{
	namespace RENDER
	{
		//Texture modes for materials
		// TODO: Move this to 
		const unsigned int TEXTURE_NONE=0;
		const unsigned int TEXTURE_NORMAL=1;
		const unsigned int TEXTURE_MASK=2;
		const unsigned int TEXTURE_BLEND=3;
		const unsigned int TEXTURE_DETAIL=4;
		const unsigned int TEXTURE_CUBEMAP=5;
		
		const unsigned int TEXTURE_RGBA=0;
		const unsigned int TEXTURE_HEIGHTMAP=0;

		class cTexture
		{
		public:
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

			cTexture();
			~cTexture();
			
			bool Load(std::string sFilename);

			void CopyFromSurface(unsigned int w, unsigned int h);
			void CopyFromSurface();
			void CopyToSurface();

			bool SaveToBMP(std::string sFilename);

			void GenerateOpenGLTexture();
			
			void Transform(float *u, float *v);
		};
	}
}

#endif //CTEXTURE_H