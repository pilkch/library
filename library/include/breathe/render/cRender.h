#ifndef CRENDER_H
#define CRENDER_H

// FBO mipmaps
//#define RENDER_GENERATEFBOMIPMAPS

namespace breathe
{
	namespace render
	{
		const unsigned int MAX_TEXTURE_SIZE=1024;

		const unsigned int FBO_TEXTURE_WIDTH=1024;
		const unsigned int FBO_TEXTURE_HEIGHT=1024;

		class cVertexBufferObject;
		class cMaterial;

		namespace MODEL
		{
			class cMesh;
			class cStatic;
			class cAnimation;
		}

		class cRender
		{
		public:
			cRender();
			~cRender();

		private:
			bool FindExtension(std::string sExt);

		public:
			bool Init();

			void SetPerspective();

			void ToggleFullscreen();
			void SetAtlasWidth(unsigned int uiNewSegmentWidthPX, unsigned int uiNewSegmentSmallPX, unsigned int uiNewAtlasWidthPX);

			void BeginLoadingTextures();
			void EndLoadingTextures();

			// These are the actual calls, the previous may actually render to an FBO first
		private:
			void _BeginRenderShared();

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

			void RenderArrow(math::cVec3& from, math::cVec3& to, math::cColour& colour);
			void RenderAxisReference(float x, float y, float z);
			void RenderAxisReference(math::cVec3& position);
			void RenderWireframeBox(math::cVec3& vMin, math::cVec3& vMax);
			void RenderScreenSpaceRectangle(float x, float y, float fWidth, float fHeight);
			
			void RenderMesh(model::cMesh* pMesh);
			unsigned int RenderStaticModel(model::cStatic* p);
			unsigned int RenderStaticModel(model::cStatic* p, math::cColour& colour);

			void PushScreenSpacePosition(float x, float y);
			void PopScreenSpacePosition();


			// *** Resources

			void TransformModels();

			model::cStatic* AddModel(std::string sNewFilename);
			model::cStatic* GetModel(std::string sNewFilename);
			model::cStatic* CreateNewModel(std::string sName);

			cVertexBufferObject* AddVertexBufferObject();

			bool AddTextureNotFoundTexture(std::string sNewFilename);
			bool AddMaterialNotFoundTexture(std::string sNewFilename);

			cTexture *AddCubeMap(std::string sFilename);
			cTexture *AddTexture(std::string sNewFilename);
			cTexture *AddTextureToAtlas(std::string sNewFilename, unsigned int uiAtlas);
			
			cTexture *GetTexture(std::string sFilename);
			cTexture* GetCubeMap(std::string sFilename);



			bool SetTexture0(std::string sTexture) { return SetTexture0(GetTexture(sTexture)); }
			bool SetTexture0(cTexture* pTexture);
			bool SetTexture1(std::string sTexture) { return SetTexture1(GetTexture(sTexture)); }
			bool SetTexture1(cTexture* pTexture);

			MATERIAL::cMaterial* AddMaterial(std::string sFilename);
			MATERIAL::cMaterial* AddMaterialNotFoundMaterial(std::string sFilename);
			bool ClearMaterial();
			bool SetMaterial(std::string sMaterial) { return SetMaterial(GetMaterial(sMaterial)); }
			bool SetMaterial(MATERIAL::cMaterial* pMaterial) { return SetMaterial(pMaterial, math::cVec3()); }
			bool SetMaterial(MATERIAL::cMaterial* pMaterial, math::cVec3& pos);
			
			bool SetShaderConstant(MATERIAL::cMaterial* pMaterial, std::string sConstant, int value);
			bool SetShaderConstant(MATERIAL::cMaterial* pMaterial, std::string sConstant, float value);
			bool SetShaderConstant(MATERIAL::cMaterial* pMaterial, std::string sConstant, math::cVec3& value);

			void ClearColour();
			void SetColour(float r, float g, float b);
			void SetColour(const math::cColour& inColour);

			MATERIAL::cMaterial* GetMaterial(std::string sFilename);

			
			void ReloadTextures();

			MATERIAL::cMaterial* AddPostRenderEffect(std::string sFilename);
			void RemovePostRenderEffect();

		private:
			std::list<MATERIAL::cMaterial*> lPostRenderEffects;
			cTextureFrameBufferObject* pFrameBuffer0;
			cTextureFrameBufferObject* pFrameBuffer1;




		public:
			std::map<std::string, cTexture *> mTexture; //Map that contains filename, texture pairs
			std::map<std::string, cTexture *> mCubeMap; //Map that contains filename, cubemap texture pairs


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

			unsigned int uiActiveUnits;

			math::cVec4 v4SunPosition;


			cTexture *pTextureNotFoundTexture;
			cTexture *pMaterialNotFoundTexture;
			
			MATERIAL::cMaterial *pMaterialNotFoundMaterial;


			std::vector<render::cTextureAtlas *> vTextureAtlas; //Vector that contains texture atlases
			std::map<std::string, MATERIAL::cMaterial *> mMaterial;

			std::vector<cVertexBufferObject*>vVertexBufferObject;


			cLevel *pLevel;
			math::cFrustum *pFrustum;

			SDL_Surface *pSurface;

		private:
			unsigned int uiSegmentWidthPX;
			unsigned int uiSegmentSmallPX;
			unsigned int uiAtlasWidthPX;

			
			bool bActiveColour;
			bool bActiveShader;

			math::cColour colour;
			std::vector<MATERIAL::cLayer>vLayer;

			MATERIAL::cMaterial* pCurrentMaterial;
			
			//std::map<std::string, model::cAnimation*> mAnimation;
			std::map<std::string, model::cStatic*> mStatic;
		};

		namespace SETTINGS
		{
			class resolution
			{
			public:
				resolution(int width, int height);

				int GetWidth() const { return width; }
				int GetHeight() const { return height; }

			private:
				int width;
				int height;
			};

			class iterator
			{
			public:
				void GetStandardResolutions() { GetResolutions(false); }
				void GetWidescreenResolutions() { GetResolutions(true); }

				iterator();

				int GetWidth() const { return (*iter).GetWidth(); }
				int GetHeight() const { return (*iter).GetHeight(); }

				operator ++(int);
				operator bool() const;
				
			protected:
				void GetResolutions(bool onlyWidescreen);

				std::vector<resolution> resolutions;
				std::vector<resolution>::iterator iter;
			};

			// *** Inlines
			
			inline resolution::resolution(int _width, int _height) :
				width(_width),
				height(_height)
			{
			}

			
			inline iterator::operator ++(int)
			{
				iter++;
				return *this;
			}

			inline iterator::operator bool() const
			{
				return iter != resolutions.end();
			}
		}
	}
}

extern breathe::render::cRender* pRender;

#endif //CRENDER_H
