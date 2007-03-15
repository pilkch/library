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

		class cTexture
		{
		public:
			unsigned int uiTextureAtlas;
			unsigned int uiTexture;
			std::string sFilename;
			
			float fScale;
			float fU;
			float fV;

			cTexture();
			~cTexture();
			
			void Transform(float *u, float *v);
		};
	}
}

#endif //CTEXTURE_H