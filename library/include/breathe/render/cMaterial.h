#ifndef CMATERIAL_H
#define CMATERIAL_H

namespace breathe
{
	class cLevel;

	namespace render
	{
		namespace material
		{
			const unsigned int nLayers=3;

			class cLayer
			{
			public:
				cLayer();

				cTexture* pTexture;
				unsigned int uiTextureMode;
				std::string sTexture;
			};

			class cShader
			{
			public:
				cShader();

				void Init();
				void Destroy();
				
				void CheckStatusVertex();
				void CheckStatusFragment();
				void CheckStatusProgram();

				std::string sShaderVertex;
				std::string sShaderFragment;

				unsigned int uiShaderVertex;
				unsigned int uiShaderFragment;

				// Combined resource id
				unsigned int uiShaderProgram;

				bool bCameraPos;

				bool bTexUnit0;
				bool bTexUnit1;
				bool bTexUnit2;
				bool bTexUnit3;
			};

			class cMaterial
			{
			public:
				cMaterial(std::string sName);
				~cMaterial();

				//Rendering
				uint8_t chDustR;
				uint8_t chDustG; 
				uint8_t chDustB;

				std::vector<cLayer *>vLayer; 
				//0=Diffuse
				//1+=Either none, detail, cubemap or bumpmap

				//Audio
				unsigned int uiAudioScrape;
				unsigned int uiAudioBounce;

				//Lightmap Generation
				bool bShadow_cast;
				bool bShadow_receive;
				bool bLight_receive;
				bool bLight_transmit;

				//Physics
				bool bCollideTrimesh;

				float fFriction;
				float fBounce;

				float fCorrugation; //0.0f=none, 1.0f=very bouncy (stairs etc.)

				std::string sName;
				
				cShader *pShader;

				bool Load(std::string sNewFilename);
			};
		}
	}
}

#endif //CMATERIAL_H
