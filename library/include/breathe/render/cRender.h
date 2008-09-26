#ifndef CRENDER_H
#define CRENDER_H

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cColour.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cStatic.h>

// FBO mipmaps
//#define RENDER_GENERATEFBOMIPMAPS

namespace breathe
{
  namespace render
  {
    const unsigned int MAX_TEXTURE_SIZE = 1024;

    const unsigned int FBO_TEXTURE_WIDTH = 1024;
    const unsigned int FBO_TEXTURE_HEIGHT = 1024;

    class cVertexBufferObject;
    typedef cSmartPtr<cVertexBufferObject> cVertexBufferObjectRef;

    namespace model
    {
      class cAnimation;
    }

    namespace material
    {
      class cMaterial;
      typedef cSmartPtr<cMaterial> cMaterialRef;
    }

    class cBatchModelContainer
    {
    public:
      cBatchModelContainer() : fDistanceFromCamera(0.0f) {}
      cBatchModelContainer(model::cStaticRef pModel, float fDistanceFromCamera);

      static bool SortBackToFront(const cBatchModelContainer* lhs, const cBatchModelContainer* rhs);
      static bool SortFrontToBack(const cBatchModelContainer* lhs, const cBatchModelContainer* rhs);

      float fDistanceFromCamera;
      model::cStaticRef pModel;
    };

    class cBatchList
    {
    public:
      typedef std::list<cBatchModelContainer*>::iterator iterator;
      typedef std::list<cBatchModelContainer*>::const_iterator const_iterator;

      void AddModel(model::cStaticRef pModel, float fDistanceFromCamera);
      void FinishAddingSortFrontToBack();
      void FinishAddingSortBackToFront();

    private:
      std::vector<cBatchModelContainer*> models;

      friend class cRender;
    };

    class cBatchController
    {
    public:
      typedef std::map<material::cMaterialRef, cBatchList*>::iterator iterator;
      typedef std::map<material::cMaterialRef, cBatchList*>::const_iterator const_iterator;

      void AddModel(model::cStaticRef pModel, float fDistanceFromCamera);
      void FinishAdding();

    private:
      std::map<material::cMaterialRef, cBatchList*> opaque;
      std::map<material::cMaterialRef, cBatchList*> transparent;

      friend class cRender;
    };

    class cRender
    {
    public:
			cRender();
			~cRender();

		private:
			bool FindExtension(const std::string& sExt);
      float GetShaderVersion();

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

      void _RenderPostRenderPass(material::cMaterialRef pMaterial, cTextureFrameBufferObjectRef pFBO);

		public:
			void Begin();
			void End();

			void BeginRenderToScreen();
			void EndRenderToScreen();

      void BeginRenderToTexture(cTextureFrameBufferObjectRef pTexture);
      void EndRenderToTexture(cTextureFrameBufferObjectRef pTexture);

			void BeginRenderScene();
			void EndRenderScene();

      // In this mode x is 0..1, y is 1..0
			void BeginScreenSpaceRendering();
			void EndScreenSpaceRendering();
      void BeginScreenSpaceGuiRendering();
      void EndScreenSpaceGuiRendering();

      // In this mode x is -fScale..+fScale, y is +fScale..-fScale
			void BeginScreenSpaceWorldRendering(float fScale);
			void EndScreenSpaceWorldRendering();

      void RenderArrow(const math::cVec3& from, const math::cVec3& to, const math::cColour& colour);
			void RenderAxisReference(float x, float y, float z);
			void RenderAxisReference(const math::cVec3& position);
      void RenderWireframeBox(const math::cVec3& vMin, const math::cVec3& vMax);

			void RenderScreenSpacePolygon(float fX, float fY,
        float fVertX0, float fVertY0, float fVertX1, float fVertY1,
        float fVertX2, float fVertY2, float fVertX3, float fVertY3);

			void RenderScreenSpaceRectangle(float fX, float fY, float fWidth, float fHeight);
			void RenderScreenSpaceRectangle(
				float fX, float fY, float fWidth, float fHeight,
				float fU, float fV, float fU2, float fV2);

			void RenderScreenSpaceRectangleTopLeftIsAt(float fX, float fY, float fWidth, float fHeight);
			void RenderScreenSpaceRectangleTopLeftIsAt(
				float fX, float fY, float fWidth, float fHeight,
				float fU, float fV, float fU2, float fV2);

			void RenderScreenSpaceRectangleRotated(float fX, float fY, float fWidth, float fHeight, float fRotation);

      void RenderMesh(model::cMeshRef pMesh);
      unsigned int RenderStaticModel(model::cStaticRef p);
      unsigned int RenderStaticModel(model::cStaticRef p, const math::cColour& colour);

			void PushScreenSpacePosition(float x, float y);
			void PopScreenSpacePosition();


      void EnableWireframe();
			void DisableWireframe();

      void BeginRenderingText() { if (bRenderWireframe) DisableWireframe(); }
			void EndRenderingText() { if (bRenderWireframe) EnableWireframe(); }


			// *** Resources

			void TransformModels();

      model::cStaticRef AddModel(const string_t& sNewFilename);
      model::cStaticRef GetModel(const string_t& sNewFilename);
      model::cStaticRef CreateNewModel(const string_t& sName);

      cVertexBufferObjectRef AddVertexBufferObject();

			bool AddTextureNotFoundTexture(const std::string& sNewFilename);
			bool AddMaterialNotFoundTexture(const std::string& sNewFilename);

      cTextureRef AddCubeMap(const string_t& sFilename);
      cTextureRef AddTexture(const std::string& sNewFilename);
      cTextureRef AddTextureToAtlas(const std::string& sNewFilename, unsigned int uiAtlas);

      cTextureRef GetTextureAtlas(ATLAS atlas);
      cTextureRef GetTexture(const std::string& sFilename);
      cTextureRef GetCubeMap(const string_t& sFilename);

      material::cMaterialRef GetCurrentMaterial() const;
      cTextureRef GetCurrentTexture0() const;
      cTextureRef GetCurrentTexture1() const;
      cTextureRef GetCurrentTexture2() const;

			void SelectTextureUnit0();
			void SelectTextureUnit1();
			void SelectTextureUnit2();

			bool SetTexture0(const std::string& sTexture) { return SetTexture0(GetTexture(sTexture)); }
			bool SetTexture0(ATLAS atlas);
      bool SetTexture0(cTextureRef pTexture);
			bool SetTexture1(const std::string& sTexture) { return SetTexture1(GetTexture(sTexture)); }
			bool SetTexture1(ATLAS atlas);
      bool SetTexture1(cTextureRef pTexture);

      material::cMaterialRef AddMaterial(const std::string& sFilename);
      material::cMaterialRef AddMaterialNotFoundMaterial(const std::string& sFilename);
			bool ClearMaterial();
			bool SetMaterial(const std::string& sMaterial) { return SetMaterial(GetMaterial(sMaterial)); }
      bool SetMaterial(material::cMaterialRef pMaterial) { math::cVec3 pos; return SetMaterial(pMaterial, pos); }
      bool SetMaterial(material::cMaterialRef pMaterial, const math::cVec3& pos);

      bool SetShaderConstant(material::cMaterialRef pMaterial, const std::string& sConstant, int value);
      bool SetShaderConstant(material::cMaterialRef pMaterial, const std::string& sConstant, float value);
      bool SetShaderConstant(material::cMaterialRef pMaterial, const std::string& sConstant, const math::cVec3& value);

			void ClearColour();
			void SetColour(float r, float g, float b);
			void SetColour(const math::cColour& inColour);

      material::cMaterialRef GetMaterial(const std::string& sFilename);


			void ReloadTextures();

      material::cMaterialRef AddPostRenderEffect(const std::string& sFilename);
			void RemovePostRenderEffect();

		private:
      std::list<material::cMaterialRef> lPostRenderEffects;
      cTextureFrameBufferObjectRef pFrameBuffer0;
      cTextureFrameBufferObjectRef pFrameBuffer1;




		public:
      std::map<std::string, cTextureRef> mTexture; //Map that contains filename, texture pairs
      std::map<string_t, cTextureRef> mCubeMap; //Map that contains filename, cubemap texture pairs


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

			math::cVec4 sunPosition;


      cTextureRef pTextureNotFoundTexture;
      cTextureRef pMaterialNotFoundTexture;

      material::cMaterialRef pMaterialNotFoundMaterial;


      std::vector<render::cTextureAtlasRef> vTextureAtlas; //Vector that contains texture atlases
      std::map<std::string, material::cMaterialRef> mMaterial;

      std::vector<cVertexBufferObjectRef>vVertexBufferObject;


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

      material::cMaterialRef pCurrentMaterial;

			// Information about the current video settings
			SDL_VideoInfo* g_info;
			const SDL_VideoInfo* videoInfo;
			SDL_Surface* pSurface;

			//std::map<std::string, model::cAnimationRef> mAnimation;
      std::map<string_t, model::cStaticRef> mStatic;

		public:
			void QueueAddOpaqueObject();
			void QueueAddTransparentObject();

		private:
			cRender(const cRender&);
			cRender operator=(const cRender&);
		};

		namespace SETTINGS
		{
			class resolution
			{
			public:
				resolution(int width, int height);

				int GetWidth() const { return width; }
				int GetHeight() const { return height; }



			  class iterator
			  {
			  public:
				  void GetStandardResolutions() { GetResolutions(false); }
				  void GetWidescreenResolutions() { GetResolutions(true); }

				  iterator();

				  int GetWidth() const { return (*iter).GetWidth(); }
				  int GetHeight() const { return (*iter).GetHeight(); }

				  operator bool() const;

				  iterator operator ++(int);

			  protected:
				  void GetResolutions(bool onlyWidescreen);

				  std::vector<resolution> resolutions;
				  std::vector<resolution>::iterator iter;
			  };

			private:
				int width;
				int height;
			};

			// *** Inlines

			inline resolution::resolution(int _width, int _height) :
				width(_width),
				height(_height)
			{
			}


      inline resolution::iterator resolution::iterator::operator ++(int)
			{
				iter++;
				return *this;
			}

      inline resolution::iterator::operator bool() const
			{
				return iter != resolutions.end();
			}
		}

		class ApplyTexture
		{
		public:
      explicit ApplyTexture(cTextureRef pCurrent);
			~ApplyTexture();

		private:
			ApplyTexture();
			NO_COPY(ApplyTexture);

      cTextureRef pLast;
		};

		class ApplyMaterial
		{
		public:
      explicit ApplyMaterial(material::cMaterialRef pCurrent);
			~ApplyMaterial();

		private:
			ApplyMaterial();
			NO_COPY(ApplyMaterial);

      material::cMaterialRef pLast;
		};
	}
}

extern breathe::render::cRender* pRender;

#endif // CRENDER_H
