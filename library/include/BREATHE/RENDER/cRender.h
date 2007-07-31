#ifndef CRENDER_H
#define CRENDER_H

// FBO mipmaps
//#define RENDER_GENERATEFBOMIPMAPS

namespace BREATHE
{
	namespace RENDER
	{
		const unsigned int MAX_TEXTURE_SIZE=1024;

		const unsigned int FBO_TEXTURE_WIDTH=1024;
		const unsigned int FBO_TEXTURE_HEIGHT=1024;

		class cVertexBufferObject;
		class cMaterial;

		class cRender
		{
		private:
			unsigned int uiSegmentWidthPX;
			unsigned int uiSegmentSmallPX;
			unsigned int uiAtlasWidthPX;

			bool FindExtension(std::string sExt);

			
			bool bActiveColour;
			bool bActiveShader;

			MATH::cColour colour;
			std::vector<MATERIAL::cLayer>vLayer;

		public:
			std::map<std::string, cTexture *> mTexture; //Map that contains filename, texture pairs
			std::map<std::string, cTexture *> mCubeMap; //Map that contains filename, cubemap texture pairs


			cLevel *pLevel;


			bool bRenderWireframe;
			bool bLight;
			bool bCubemap;
			bool bShader;

			bool bCanCubemap;
			bool bCanShader;
			bool bCanFrameBufferObject;

			bool bFullscreen;
			unsigned int uiWidth;
			unsigned int uiHeight;
			unsigned int uiDepth;

			unsigned int uiFlags;

      int iMaxTextureSize;

			unsigned int uiTextureChanges;
			unsigned int uiTextureModeChanges;
			unsigned int uiTriangles;

			MATH::cVec4 v4SunPosition;

			std::vector<RENDER::cTextureAtlas *> vTextureAtlas; //Vector that contains texture atlases

			cTexture *pTextureNotFoundTexture;
			cTexture *pMaterialNotFoundTexture;
			
			unsigned int uiActiveUnits;

			MATERIAL::cMaterial *pMaterialNotFoundMaterial;

			std::map<std::string, MATERIAL::cMaterial *> mMaterial;



			std::vector<cVertexBufferObject*>vVertexBufferObject;
			cVertexBufferObject* AddVertexBufferObject();


			MATH::cFrustum *pFrustum;

			SDL_Surface *pSurface;

			cRender();
			~cRender();

			bool Init();

			// These are the actual calls, the previous may actually render to an FBO first
		private:
			void _BeginRenderToScreen();
			void _EndRenderToScreen();

			void _RenderPostRenderPass(MATERIAL::cMaterial* pMaterial, cTextureFrameBufferObject* pFBO);

		public:
			void Begin();
			void End();
			
			void BeginRenderToScreen();
			void EndRenderToScreen();

			void BeginRenderToTexture(cTextureFrameBufferObject* pTexture);
			void EndRenderToTexture(cTextureFrameBufferObject* pTexture);

			void BeginRenderScene();
			void EndRenderScene();

			void BeginScreenSpaceRendering();
			void EndScreenSpaceRendering();

			void RenderBox(MATH::cVec3& vMin, MATH::cVec3& vMax);
			void RenderScreenSpaceRectangle(float fX, float fY, float fWidth, float fHeight);

			void ToggleFullscreen();

			/*//Font
		protected:
			int font_line;

			int font_m_iTexture;
			int font_charWidth;					// Desired width of font
			int font_charHeight;					// Desired height of font
			int font_charSpacing;				// Spacing between each character
			int font_textureFilter;			// What type of texture filtering to render the text with.
															// 0 = point AKA no filtering
															// 1 = Linear

			void *font_pVertices;				// Temp memory to work with when modify vertex buffers

			unsigned int font_base; //which display list
			int font_texture;

		public:
			virtual bool Font_Create(int newTexture, void *pDevice, void *pRenderer)=0;

			virtual void Font_Begin(bool reset)=0;
			virtual void Font_Print(int x, int y, int set, char * string, ...)=0;
			virtual void Font_Println(int set, char * string, ...)=0;
			virtual void Font_End(void)=0;*/


			

			void SetAtlasWidth(unsigned int uiNewSegmentWidthPX, unsigned int uiNewSegmentSmallPX, unsigned int uiNewAtlasWidthPX);

			void BeginLoadingTextures();
			void EndLoadingTextures();

			bool AddTextureNotFoundTexture(std::string sNewFilename);
			bool AddMaterialNotFoundTexture(std::string sNewFilename);

			cTexture *AddCubeMap(std::string sFilename);
			cTexture *AddTexture(std::string sNewFilename);
			cTexture *AddTextureToAtlas(std::string sNewFilename, unsigned int uiAtlas);
			
			cTexture *GetTexture(std::string sFilename);
			cTexture* GetCubeMap(std::string sFilename);


			MATERIAL::cMaterial* pCurrentMaterial;


			MATERIAL::cMaterial* AddMaterial(std::string sFilename);
			MATERIAL::cMaterial* AddMaterialNotFoundMaterial(std::string sFilename);
			bool ClearMaterial();
			bool SetMaterial(std::string sMaterial) { return SetMaterial(GetMaterial(sMaterial)); }
			bool SetMaterial(MATERIAL::cMaterial* pMaterial) { return SetMaterial(pMaterial, MATH::cVec3()); }
			bool SetMaterial(MATERIAL::cMaterial* pMaterial, MATH::cVec3& pos);

			void ClearColour();
			void SetColour(MATH::cColour inColour);

			MATERIAL::cMaterial* GetMaterial(std::string sFilename);

			
			void ReloadTextures();

			MATERIAL::cMaterial* AddPostRenderEffect(std::string sFilename);
			void RemovePostRenderEffect();

		private:
			std::list<MATERIAL::cMaterial*> lPostRenderEffects;
			cTextureFrameBufferObject* pFrameBuffer0;
			cTextureFrameBufferObject* pFrameBuffer1;
		};
	}
}

extern BREATHE::RENDER::cRender* pRender;

#endif //CRENDER_H
