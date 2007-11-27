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

		namespace model
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
			bool FindExtension(const std::string& sExt);

		public:
			// Candidate for removal
			bool PreInit();
			
			bool Init();
			void Destroy();

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

			void _RenderPostRenderPass(material::cMaterial* pMaterial, cTextureFrameBufferObject* pFBO);

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
			void RenderScreenSpaceRectangle(
				float fX, float fY, float fWidth, float fHeight,
				float fU, float fV, float fU2, float fV2);
			
			void RenderMesh(model::cMesh* pMesh);
			unsigned int RenderStaticModel(model::cStatic* p);
			unsigned int RenderStaticModel(model::cStatic* p, math::cColour& colour);

			void PushScreenSpacePosition(float x, float y);
			void PopScreenSpacePosition();


			// *** Resources

			void TransformModels();

			model::cStatic* AddModel(const std::string& sNewFilename);
			model::cStatic* GetModel(const std::string& sNewFilename);
			model::cStatic* CreateNewModel(const std::string& sName);

			cVertexBufferObject* AddVertexBufferObject();

			bool AddTextureNotFoundTexture(const std::string& sNewFilename);
			bool AddMaterialNotFoundTexture(const std::string& sNewFilename);

			cTexture* AddCubeMap(const std::string& sFilename);
			cTexture* AddTexture(const std::string& sNewFilename);
			cTexture* AddTextureToAtlas(const std::string& sNewFilename, unsigned int uiAtlas);
			
			cTexture* GetTextureAtlas(ATLAS atlas);
			cTexture* GetTexture(const std::string& sFilename);
			cTexture* GetCubeMap(const std::string& sFilename);

			material::cMaterial* GetCurrentMaterial() const;
			cTexture* GetCurrentTexture0() const;
			cTexture* GetCurrentTexture1() const;
			cTexture* GetCurrentTexture2() const;

			void SelectTextureUnit0();
			void SelectTextureUnit1();
			void SelectTextureUnit2();

			bool SetTexture0(const std::string& sTexture) { return SetTexture0(GetTexture(sTexture)); }
			bool SetTexture0(ATLAS atlas);
			bool SetTexture0(cTexture* pTexture);
			bool SetTexture1(const std::string& sTexture) { return SetTexture1(GetTexture(sTexture)); }
			bool SetTexture1(ATLAS atlas);
			bool SetTexture1(cTexture* pTexture);

			material::cMaterial* AddMaterial(const std::string& sFilename);
			material::cMaterial* AddMaterialNotFoundMaterial(const std::string& sFilename);
			bool ClearMaterial();
			bool SetMaterial(const std::string& sMaterial) { return SetMaterial(GetMaterial(sMaterial)); }
			bool SetMaterial(material::cMaterial* pMaterial) { return SetMaterial(pMaterial, math::cVec3()); }
			bool SetMaterial(material::cMaterial* pMaterial, math::cVec3& pos);
			
			bool SetShaderConstant(material::cMaterial* pMaterial, std::string sConstant, int value);
			bool SetShaderConstant(material::cMaterial* pMaterial, std::string sConstant, float value);
			bool SetShaderConstant(material::cMaterial* pMaterial, std::string sConstant, math::cVec3& value);

			void ClearColour();
			void SetColour(float r, float g, float b);
			void SetColour(const math::cColour& inColour);

			material::cMaterial* GetMaterial(const std::string& sFilename);

			
			void ReloadTextures();

			material::cMaterial* AddPostRenderEffect(const std::string& sFilename);
			void RemovePostRenderEffect();

		private:
			std::list<material::cMaterial*> lPostRenderEffects;
			cTextureFrameBufferObject* pFrameBuffer0;
			cTextureFrameBufferObject* pFrameBuffer1;




		public:
			std::map<std::string, cTexture*> mTexture; //Map that contains filename, texture pairs
			std::map<std::string, cTexture*> mCubeMap; //Map that contains filename, cubemap texture pairs


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


			cTexture* pTextureNotFoundTexture;
			cTexture* pMaterialNotFoundTexture;
			
			material::cMaterial* pMaterialNotFoundMaterial;


			std::vector<render::cTextureAtlas*> vTextureAtlas; //Vector that contains texture atlases
			std::map<std::string, material::cMaterial*> mMaterial;

			std::vector<cVertexBufferObject*>vVertexBufferObject;


			cLevel* pLevel;
			math::cFrustum* pFrustum;

		private:
			unsigned int uiSegmentWidthPX;
			unsigned int uiSegmentSmallPX;
			unsigned int uiAtlasWidthPX;

			
			bool bActiveColour;
			bool bActiveShader;

			math::cColour colour;
			std::vector<material::cLayer>vLayer;

			material::cMaterial* pCurrentMaterial;
			
			// Information about the current video settings
			SDL_VideoInfo* g_info;
			const SDL_VideoInfo* videoInfo;
			SDL_Surface* pSurface;

			//std::map<std::string, model::cAnimation*> mAnimation;
			std::map<std::string, model::cStatic*> mStatic;

		public:
			void QueueAddOpaqueObject();
			void QueueAddTransparentObject();

		private:
			cRender(const cRender&);
			cRender& operator=(const cRender&);
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

		class ApplyTexture
		{
		public:
			explicit ApplyTexture(cTexture* pCurrent);
			~ApplyTexture();

		private:
			ApplyTexture();
			NO_COPY(ApplyTexture);

			cTexture* pLast;
		};

		class ApplyMaterial
		{
		public:
			explicit ApplyMaterial(material::cMaterial* pCurrent);
			~ApplyMaterial();

		private:
			ApplyMaterial();
			NO_COPY(ApplyMaterial);

			material::cMaterial* pLast;
		};
	}
}

extern breathe::render::cRender* pRender;

#endif //CRENDER_H
