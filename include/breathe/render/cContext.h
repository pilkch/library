#ifndef CCONTEXT_H
#define CCONTEXT_H

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

// TODO: Remove we should not have to know about anything in cSystem.h
#include <breathe/render/cSystem.h>

// http://www.opengl.org/wiki/Tutorial1:_Creating_a_Cross_Platform_OpenGL_3.2_Context_in_SDL_%28C_/_SDL%29

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
    class cWindow;

    const size_t MAX_TEXTURE_UNITS = 3;
    const size_t MAX_TEXTURE_SIZE = 1024;
    const size_t MAX_LIGHTS = 8;

    enum class CUBE_MAP_FACE {
      POSITIVE_X,
      NEGATIVE_X,
      POSITIVE_Y,
      NEGATIVE_Y,
      POSITIVE_Z,
      NEGATIVE_Z
    };

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

    class cStatistics
    {
    public:
      cStatistics();

      void Reset();

      size_t nStateChanges; // How many times a new stateset was applied
      size_t nModelsRendered;
      size_t nSceneObjectsRendered;
      size_t nVertexBufferObjectsBound; // How many vertex buffer objects were bound (ie. 30)
      size_t nVertexBufferObjectsRendered; // How many vertex buffer objects were rendered (ie. 300)
      size_t nTrianglesRendered; // How many triangles were rendered in the whole scene
    };

    class cContext
    {
    public:
      cContext();
      ~cContext();

      cCapabilities GetCapabilities() const;

      cResolution GetCurrentScreenResolution() const;
      std::vector<cResolution> GetAvailableScreenResolutions() const;

      bool CreateSharedContextFromWindow(const cWindow& window);
      bool CreateSharedContextFromContext(const cContext& context);
      void Destroy();

      cResolution GetResolution() const;

      bool IsWireFrame() const { return bIsRenderWireframe; }

      bool IsMultiSampling() const;
      size_t GetMultiSampleLevel() const;

      string_t GetErrorString() const;
      string_t GetErrorString(GLenum error) const;

      const cStatistics& GetStatistics() const { return statistics; }

      void StatisticsIncrementStateChanges() { statistics.nStateChanges++; }
      void StatisticsIncrementVertexBufferObjectsBound() { statistics.nVertexBufferObjectsBound++; }
      void StatisticsIncrementVertexBufferObjectsRendered() { statistics.nVertexBufferObjectsRendered++; }
      void StatisticsAddTrianglesRendered(size_t nTriangles) { statistics.nTrianglesRendered += nTriangles; }

      // TODO: Is this wise?
      //void BindContext(cContext& context);
      //void UnBindContext(cContext& context);

      class cLight;
      const cLight& GetLight(size_t index) const;
      void SetLight(size_t index, const cLight& light);

    private:
      bool FindExtension(const string_t& sExt) const;
      float GetShaderVersion() const;

      //const std::set<string_t>& GetExtensions() const;

      void Setup();

    public:
      void SetPerspective();

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

      material::cMaterialRef GetCurrentMaterial() const;
      cTextureRef GetCurrentTexture0() const;
      cTextureRef GetCurrentTexture1() const;
      cTextureRef GetCurrentTexture2() const;

      void SelectTextureUnit0();
      void SelectTextureUnit1();
      void SelectTextureUnit2();

      bool SetTexture0(cTextureRef pTexture);
      bool SetTexture1(cTextureRef pTexture);

      void BindShader(cShaderRef pShader);
      void UnBindShader();

      bool SetShaderConstant(const std::string& sConstant, int value);
      bool SetShaderConstant(const std::string& sConstant, float value);
      bool SetShaderConstant(const std::string& sConstant, const math::cVec2& value);
      bool SetShaderConstant(const std::string& sConstant, const math::cVec3& value);
      bool SetShaderConstant(const std::string& sConstant, const math::cVec4& value);


      // SetMaterial and ClearMaterial are deprecated, use cApplyMaterial class or if need be, use ApplyMaterial/UnApplyMaterial instead
      bool ClearMaterial();
      //bool SetMaterial(material::cMaterialRef pMaterial) { math::cVec3 pos; return SetMaterial(pMaterial, pos); }
      //bool SetMaterial(material::cMaterialRef pMaterial, const math::cVec3& pos);

      bool ApplyMaterial(material::cMaterialRef pMaterial);
      bool UnApplyMaterial(material::cMaterialRef pMaterial);

      void ClearColour();
      void SetColour(float r, float g, float b);
      void SetColour(const math::cColour& inColour);

      void SetClearColour(const math::cColour& inColour) { clearColour = inColour; }

      const math::cFrustum& GetFrustum() { return frustum; }
      void SetFrustum(const math::cFrustum& _frustum) { frustum = _frustum; }

      void SetShaderConstants(const cShaderConstants& _shaderConstants) { shaderConstants = _shaderConstants; }

      bool bFullscreen;
      size_t uiWidth;
      size_t uiHeight;
      size_t uiDepth;

      size_t uiTextureChanges;
      size_t uiTextureModeChanges;


      bool bIsRenderWireframe;
      bool bIsRenderGui;
      bool bIsLightingEnabled;
      bool bIsCubemappingEnabled;
      bool bIsRenderWithShadersEnabled;
      bool bIsFSAAEnabled;

      unsigned int uiFSAASampleLevel;

      math::cVec4 sunPosition;

    private:
      NO_COPY(cContext);

      void QueueAddOpaqueObject();
      void QueueAddTransparentObject();


      SDL_Surface* pSurface;

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
  }
}

extern breathe::render::cContext* pContext; // TODO: Remove pContext

#endif // CCONTEXT_H
