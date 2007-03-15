#ifndef CMATERIAL_H
#define CMATERIAL_H

namespace BREATHE
{
	class cLevel;

	namespace RENDER
	{
		namespace MATERIAL
		{
			const unsigned int nLayers=3;

			class cLayer
			{
			public:
				unsigned int uiTexture;
				unsigned int uiTextureMode;

				cLayer();
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

				unsigned int uiShaderVertex;
				unsigned int uiShaderFragment;

				std::string sShaderVertex;
				std::string sShaderFragment;

				// Combined resource id
				unsigned int uiShaderProgram;
			};

			class cMaterial
			{
			public:
				//Rendering
				char chDustR;
				char chDustG; 
				char chDustB;

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

				std::string sTexture0;
				std::string sTexture1;
				std::string sTexture2;

				
				cShader *pShader;

				cMaterial(std::string name);

				bool Load(std::string sNewFilename);
			};
		}
	}
}

#endif //CMATERIAL_H
