#ifndef CRENDER_H
#define CRENDER_H

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/cFrustum.h>

#include <breathe/render/camera.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cVertexBufferObject.h>
#include <breathe/render/model/cMesh.h>

// OpenGL Multiple Contexts
// http://www.stevestreeting.com/2006/10/20/gl-thread-taming/
// 1. Create main rendering thread context
// 2. Disable main rendering thread context
// 3. Lock background init condition mutex
// 4. Start background thread
// 5. Main thread waits for init condition (this releases the init mutex and blocks the main thread)
// 6. Background thread clones context, sets up resource sharing and enables its own context
// 7. Background thread locks the init mutex, notifies parent, releases init mutex, then continues independently
// 8. Main thread wakes up, re-enables its own context, and carries on too

namespace breathe
{
  namespace game
  {
    class cRenderGraph;
    class cRenderGraph2d;
  }

  namespace render
  {
    const size_t MAX_TEXTURE_UNITS = 3;
    const size_t MAX_TEXTURE_SIZE = 1024;


    enum class CUBE_MAP_FACE {
      CUBE_MAP_FACE_POSITIVE_X,
      CUBE_MAP_FACE_NEGATIVE_X,
      CUBE_MAP_FACE_POSITIVE_Y,
      CUBE_MAP_FACE_NEGATIVE_Y,
      CUBE_MAP_FACE_POSITIVE_Z,
      CUBE_MAP_FACE_NEGATIVE_Z
    };


    namespace model
    {
      class cAnimation;
    }

    namespace material
    {
      class cMaterial;
      typedef cSmartPtr<cMaterial> cMaterialRef;
    }

    /*class cBatchModelContainer
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
    };*/

    class cResolution
    {
    public:
      friend class cRender;

      cResolution();

      size_t GetWidth() const { return width; }
      size_t GetHeight() const { return height; }

      size_t GetColourDepth() const { return colourDepth; }

      bool IsWideScreen() const;

      static bool ResolutionCompare(const cResolution& lhs, const cResolution& rhs);

    protected:
      void SetResolution(size_t width, size_t height, size_t colourDepth);

    private:
      size_t width;
      size_t height;
      size_t colourDepth;
    };

    class cStatistics
    {
    public:
      cStatistics();

      void Reset();

      size_t nStateChanges;
      size_t nTrianglesRendered;
      size_t nModelsRendered;
      size_t nVertexBufferObjectsRendered;
      size_t nSceneObjectsRendered;
    };

    class cCapabilities
    {
    public:
      cCapabilities();

      bool bIsOpenGLTwoPointZeroOrLater;
      bool bIsOpenGLThreePointZeroOrLater;
      bool bIsShadersTwoPointZeroOrLater;
      bool bIsVertexBufferObject;
      bool bIsFrameBufferObject;
      bool bIsShadows;
      size_t nTextureUnits;
    };


    class cRender
    {
    public:
      cRender();
      ~cRender();

      bool IsWireFrame() const { return bIsRenderWireframe; }

      bool IsMultiSampling() const;
      size_t GetMultiSampleLevel() const;

      string_t GetErrorString() const;
      string_t GetErrorString(GLenum error) const;

    private:
      bool FindExtension(const string_t& sExt) const;
      float GetShaderVersion() const;

    public:
      cResolution GetCurrentScreenResolution() const;
      std::vector<cResolution> GetAvailableScreenResolutions() const;

      bool PreInit();

      bool Init();
      void Destroy();

      void SetPerspective();

      void ToggleFullscreen();
      void SetAtlasWidth(unsigned int uiNewSegmentWidthPX, unsigned int uiNewSegmentSmallPX, unsigned int uiNewAtlasWidthPX);

      void BeginLoadingTextures();
      void EndLoadingTextures();

      void SaveScreenShot();

      // These are the actual calls, the previous may actually render to an FBO first
    private:
      void _BeginRenderShared();
      void _EndRenderShared();

      void _BeginRenderToScreen();
      void _EndRenderToScreen();

      void _RenderPostRenderPass(material::cMaterialRef pMaterial, cTextureFrameBufferObjectRef pFBO);

      void BeginScreenSpaceGuiRendering();
      void EndScreenSpaceGuiRendering();

    public:
      void BeginRenderToScreen();
      void EndRenderToScreen();

      void BeginRenderToTexture(cTextureFrameBufferObjectRef pTexture);
      void EndRenderToTexture(cTextureFrameBufferObjectRef pTexture);

      void BeginRenderToCubeMapTextureFace(cTextureFrameBufferObjectRef pTexture, CUBE_MAP_FACE face);
      void EndRenderToCubeMapTextureFace(cTextureFrameBufferObjectRef pTexture);

      // In this mode x is 0..1, y is 1..0
      void BeginScreenSpaceRendering();
      void EndScreenSpaceRendering();

      // In this mode x is -fScale..+fScale, y is +fScale..-fScale
      void BeginScreenSpaceWorldRendering(float fScale);
      void EndScreenSpaceWorldRendering();

      void RenderScreenSpacePolygon(float fX, float fY,
        float fVertX0, float fVertY0, float fVertX1, float fVertY1,
        float fVertX2, float fVertY2, float fVertX3, float fVertY3);
      void RenderScreenSpaceSolidRectangleWithBorderTopLeftIsAt(float fX, float fY, float fWidth, float fHeight, const math::cColour& boxColour, const math::cColour& upperBorderColour, const math::cColour& lowerBorderColour);

      void RenderScreenSpaceGradientFilledRectangleTopLeftIsAt(float fX, float fY, float fWidth, float fHeight, const math::cColour& colour0, const math::cColour& colour1, const math::cColour& colour2, const math::cColour& colour3);

      void RenderScreenSpaceRectangle(float fX, float fY, float fWidth, float fHeight);
      void RenderScreenSpaceRectangle(
        float fX, float fY, float fWidth, float fHeight,
        float fU, float fV, float fU2, float fV2);

      void RenderScreenSpaceRectangleTopLeftIsAt(float fX, float fY, float fWidth, float fHeight);
      void RenderScreenSpaceRectangleTopLeftIsAt(
        float fX, float fY, float fWidth, float fHeight,
        float fU, float fV, float fU2, float fV2);

      void RenderScreenSpaceRectangleRotated(float fX, float fY, float fWidth, float fHeight, float fRotation);

      void RenderScreenSpaceLines(const std::vector<math::cVec2>& points);

      void RenderTriangle(const math::cVec3& v0, const math::cVec3& v1, const math::cVec3& v2);
      void RenderArrow(const math::cVec3& from, const math::cVec3& to, const math::cColour& colour);

      void RenderWireframeBox(const math::cVec3& vMin, const math::cVec3& vMax);

      void RenderBoxTextured(const math::cVec3& vMin, const math::cVec3& vMax);

      void RenderMesh(model::cMeshRef pMesh);



      //void SetClearColour(const math::cColour& colour);

      //void BindMaterial(material::cMaterialRef pMaterial);
      //void UnBindMaterial(material::cMaterialRef pMaterial);

      //void BindTexture(cTextureRef pTexture, size_t index);
      //void UnBindTexture(cTextureRef pTexture, size_t index);

      //void BindShader(cShaderRef pShader);
      //void UnBindShader(cShaderRef pShader);

      //void BindVBO(cVertexBufferObjectRef pVBO);
      //void UnBindVBO(cVertexBufferObjectRef pVBO);


      void PushProjectionMatrix();
      void PopProjectionMatrix();
      void SetProjectionMatrix(const math::cMat4& matrix);
      void MultiplyProjectionMatrix(const math::cMat4& matrix);

      void PushModelViewMatrix();
      void PopModelViewMatrix();
      void SetModelViewMatrix(const math::cMat4& matrix);
      void MultiplyModelViewMatrix(const math::cMat4& matrix);

      void PushTextureMatrix();
      void PopTextureMatrix();
      void SetTextureMatrix(const math::cMat4& matrix);
      void MultiplyTextureMatrix(const math::cMat4& matrix);

      void ApplyMatrices();

      //void RenderTriangles(cVertexBufferObjectRef pVBO);
      //void RenderTriangleStrip(cVertexBufferObjectRef pVBO);
      //void RenderQuads(cVertexBufferObjectRef pVBO);
      //void RenderQuadStrip(cVertexBufferObjectRef pVBO);

      // Set the camera to use for rendering the next scene
      void SetCamera(const cCamera& camera);

      // Render the elements of the complete render graph
      void RenderScene(game::cRenderGraph& renderGraph);

      // Render the elements of the complete 2d render graph
      void RenderScene2d(game::cRenderGraph2d& renderGraph2d);

#ifdef BUILD_DEBUG
      // Immediate mode functions for rendering basic shapes in debug mode
      // These are rendered using the current translation, rotation and scale
      // NOTE: They should not be used in release, an actual vbo should be used instead
      void RenderLine(const math::cVec3& v0, const math::cVec3& v1);
      void RenderLines(const std::vector<math::cVec3>& points);
      void RenderAxisReference(); // Rendered with a red, green and blue line, ignoring active debug colour
      void RenderAxisReference(const math::cVec3& position); // Rendered with a red, green and blue line, ignoring active debug colour
      void RenderBox(const math::cVec3& vMin, const math::cVec3& vMax);
#endif










      void PushScreenSpacePosition(float x, float y);
      void PopScreenSpacePosition();


      void EnableWireframe();
      void DisableWireframe();

      void BeginRenderingText() { if (bIsRenderWireframe) DisableWireframe(); }
      void EndRenderingText() { if (bIsRenderWireframe) EnableWireframe(); }


      // *** Resources

      //cTextureRef CreateTexture();
      //cFrameBufferObjectRef CreateFrameBufferObject();

      //cShaderRef CreateShader();

      //cStaticVertexBufferObjectRef CreateVertexBufferObject();

      void TransformModels();

      cVertexBufferObjectRef AddVertexBufferObject();

      bool AddTextureNotFoundTexture(const string_t& sNewFilename);
      bool AddMaterialNotFoundTexture(const string_t& sNewFilename);

      cTextureRef AddCubeMap(const string_t& sFilename);
      cTextureRef AddTexture(const string_t& sNewFilename);
      cTextureRef AddTextureToAtlas(const string_t& sNewFilename, unsigned int uiAtlas);

      cTextureRef GetTextureAtlas(ATLAS atlas);
      cTextureRef GetTexture(const string_t& sFilename);
      cTextureRef GetCubeMap(const string_t& sFilename);

      material::cMaterialRef GetCurrentMaterial() const;
      cTextureRef GetCurrentTexture0() const;
      cTextureRef GetCurrentTexture1() const;
      cTextureRef GetCurrentTexture2() const;

      void SelectTextureUnit0();
      void SelectTextureUnit1();
      void SelectTextureUnit2();

      bool SetTexture0(const string_t& sTexture) { return SetTexture0(GetTexture(sTexture)); }
      bool SetTexture0(ATLAS atlas);
      bool SetTexture0(cTextureRef pTexture);
      bool SetTexture1(const string_t& sTexture) { return SetTexture1(GetTexture(sTexture)); }
      bool SetTexture1(ATLAS atlas);
      bool SetTexture1(cTextureRef pTexture);

      void BindShader(cShaderRef pShader);
      void UnBindShader();

      bool SetShaderConstant(const std::string& sConstant, int value);
      bool SetShaderConstant(const std::string& sConstant, float value);
      bool SetShaderConstant(const std::string& sConstant, const math::cVec2& value);
      bool SetShaderConstant(const std::string& sConstant, const math::cVec3& value);
      bool SetShaderConstant(const std::string& sConstant, const math::cVec4& value);


      material::cMaterialRef AddMaterial(const string_t& sFilename);
      material::cMaterialRef AddMaterialNotFoundMaterial(const string_t& sFilename);
      material::cMaterialRef AddMaterialAsAlias(const string_t& sFilename, const string_t& sAlias);
      material::cMaterialRef GetMaterial(const string_t& sFilename);

      // SetMaterial and ClearMaterial are deprecated, use cApplyMaterial class or if need be, use ApplyMaterial/UnApplyMaterial instead
      bool ClearMaterial();
      //bool SetMaterial(const string_t& sMaterial) { return SetMaterial(GetMaterial(sMaterial)); }
      //bool SetMaterial(material::cMaterialRef pMaterial) { math::cVec3 pos; return SetMaterial(pMaterial, pos); }
      //bool SetMaterial(material::cMaterialRef pMaterial, const math::cVec3& pos);

      bool ApplyMaterial(material::cMaterialRef pMaterial);
      bool UnApplyMaterial(material::cMaterialRef pMaterial);

      void ClearColour();
      void SetColour(float r, float g, float b);
      void SetColour(const math::cColour& inColour);

      void SetClearColour(const math::cColour& inColour) { clearColour = inColour; }

      void ReloadTextures();

    private:
      material::cMaterialRef _GetMaterial(const string_t& sFilename);




    public:
      std::map<string_t, cTextureRef> mTexture; //Map that contains filename, texture pairs
      std::map<string_t, cTextureRef> mCubeMap; //Map that contains filename, cubemap texture pairs


      bool bFullscreen;
      size_t uiWidth;
      size_t uiHeight;
      size_t uiDepth;

      unsigned int uiFlags;

      size_t iMaxTextureSize;

      size_t uiTextureChanges;
      size_t uiTextureModeChanges;
      size_t uiTriangles;

      bool bIsCubemappingSupported;
      bool bIsShaderSupported;
      bool bIsRenderingToFrameBufferObjectSupported;
      bool bIsFSAASupported;

      bool bIsRenderWireframe;
      bool bIsRenderGui;
      bool bIsLightingEnabled;
      bool bIsCubemappingEnabled;
      bool bIsRenderWithShadersEnabled;
      bool bIsFSAAEnabled;

      unsigned int uiFSAASampleLevel;

      math::cVec4 sunPosition;


      cTextureRef pTextureNotFoundTexture;
      cTextureRef pMaterialNotFoundTexture;

      material::cMaterialRef pMaterialNotFoundMaterial;


      std::vector<render::cTextureAtlasRef> vTextureAtlas; // Vector that contains texture atlases
      std::map<string_t, material::cMaterialRef> mMaterial;

      std::vector<cVertexBufferObjectRef> vVertexBufferObject;


      const math::cFrustum& GetFrustum() { return frustum; }
      void SetFrustum(const math::cFrustum& _frustum) { frustum = _frustum; }

      void SetShaderConstants(const cShaderConstants& _shaderConstants) { shaderConstants = _shaderConstants; }

    private:
      math::cFrustum frustum;

      size_t uiSegmentWidthPX;
      size_t uiSegmentSmallPX;
      size_t uiAtlasWidthPX;

      math::cColour clearColour;

      bool bIsActiveColour;
      math::cColour colour;
      std::vector<material::cLayer>vLayer;

      material::cMaterialRef pCurrentMaterial;
      cShaderRef pCurrentShader;


      cShaderConstants shaderConstants;

      // Information about the current video settings
      SDL_VideoInfo* g_info;
      const SDL_VideoInfo* videoInfo;
      SDL_Surface* pSurface;

    public:
      void QueueAddOpaqueObject();
      void QueueAddTransparentObject();

    private:
      NO_COPY(cRender);

      std::list<math::cMat4> lMatProjection;
      std::list<math::cMat4> lMatModelView;
      std::list<math::cMat4> lMatTexture;

      cStatistics statistics;
      cCapabilities capabilities;
    };



    class cScopedEnable
    {
    public:
      explicit cScopedEnable(GLenum field);
      ~cScopedEnable();

    private:
      cScopedEnable(); // Forbidden
      NO_COPY(cScopedEnable); // Forbidden

      GLenum field;
    };

    inline cScopedEnable::cScopedEnable(GLenum _field) :
      field(_field)
    {
      glEnable(field);
    }

    inline cScopedEnable::~cScopedEnable()
    {
      glDisable(field);
    }


    class cScopedAttributes
    {
    public:
      explicit cScopedAttributes(GLbitfield attribute);
      ~cScopedAttributes();

    private:
      cScopedAttributes(); // Forbidden
      NO_COPY(cScopedAttributes); // Forbidden

      GLbitfield attributes;
    };

    inline cScopedAttributes::cScopedAttributes(GLbitfield _attribute) :
      attributes(_attribute)
    {
      glPushAttrib(attributes);
    }

    inline cScopedAttributes::~cScopedAttributes()
    {
      glPopAttrib();
    }


    class ApplyTexture
    {
    public:
      explicit ApplyTexture(cTextureRef pCurrent);
      ~ApplyTexture();

    private:
      ApplyTexture(); // Forbidden
      NO_COPY(ApplyTexture); // Forbidden

      cTextureRef pLast;
    };

    class ApplyMaterial
    {
    public:
      explicit ApplyMaterial(material::cMaterialRef pMaterial);
      ~ApplyMaterial();

    private:
      ApplyMaterial(); // Forbidden
      NO_COPY(ApplyMaterial); // Forbidden

      //material::cMaterialRef pLast;
      material::cMaterialRef pMaterial;
    };



    class cRenderToScreen
    {
    public:
      cRenderToScreen();
      ~cRenderToScreen();

    private:
      NO_COPY(cRenderToScreen); // Forbidden
    };


    class cRenderToTexture
    {
    public:
      explicit cRenderToTexture(cTextureFrameBufferObjectRef pTexture);
      ~cRenderToTexture();

    private:
      cRenderToTexture(); // Forbidden
      NO_COPY(cRenderToTexture); // Forbidden

      cTextureFrameBufferObjectRef pTexture;
    };


    class cRenderToCubeMapTexture
    {
    public:
      cRenderToCubeMapTexture(cTextureFrameBufferObjectRef pTexture, CUBE_MAP_FACE face);
      ~cRenderToCubeMapTexture();

    private:
      cRenderToCubeMapTexture(); // Forbidden
      NO_COPY(cRenderToCubeMapTexture); // Forbidden

      cTextureFrameBufferObjectRef pTexture;
    };


    class cRenderScreenSpace
    {
    public:
      cRenderScreenSpace();
      ~cRenderScreenSpace();

    private:
      NO_COPY(cRenderScreenSpace); // Forbidden
    };

    typedef cRender cGraphicsContext;
  }
}

extern breathe::render::cGraphicsContext* pContext;
extern breathe::render::cRender* pRender; // TODO: Remove pRender

#endif // CRENDER_H
