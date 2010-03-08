#ifndef CRESOURCEMANAGER_H
#define CRESOURCEMANAGER_H

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
#include <breathe/render/cDevice.h>

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
    class cResourceManager
    {
    public:
      cResourceManager(cDevice& device);
      ~cResourceManager();

      bool Create();
      void Destroy();

      string_t GetErrorString() const;
      string_t GetErrorString(GLenum error) const;


      material::cMaterialRef CreateMaterial(const std::string& sName, const string_t& sFileName);

      cTextureRef CreateTexture(const std::string& sName, const string_t& sFileName);
      cTextureRef CreateTextureForRenderToTexture(const std::string& sName, size_t width, size_t height);

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



      material::cMaterialRef AddMaterial(const string_t& sFilename);
      material::cMaterialRef AddMaterialNotFoundMaterial(const string_t& sFilename);
      material::cMaterialRef AddMaterialAsAlias(const string_t& sFilename, const string_t& sAlias);
      material::cMaterialRef GetMaterial(const string_t& sFilename);


      void SetAtlasWidth(unsigned int uiNewSegmentWidthPX, unsigned int uiNewSegmentSmallPX, unsigned int uiNewAtlasWidthPX);

      void BeginLoadingTextures();
      void EndLoadingTextures();

      void ReloadTextures();

      cTextureRef pTextureNotFoundTexture;
      cTextureRef pMaterialNotFoundTexture;

      material::cMaterialRef pMaterialNotFoundMaterial;

      std::map<string_t, cTextureRef> mTexture; //Map that contains filename, texture pairs
      std::map<string_t, cTextureRef> mCubeMap; //Map that contains filename, cubemap texture pairs


      std::vector<render::cTextureAtlasRef> vTextureAtlas; // Vector that contains texture atlases
      std::map<string_t, material::cMaterialRef> mMaterial;

    private:
      NO_COPY(cResourceManager);

      material::cMaterialRef _GetMaterial(const string_t& sFilename);


      cDevice& device;


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

      std::vector<cVertexBufferObjectRef> vVertexBufferObject;
    };
  }
}

extern breathe::render::cResourceManager* pResourceManager; // TODO: Remove pResourceManager

#endif // CRESOURCEMANAGER_H
