#ifndef CRENDERSYSTEM_H
#define CRENDERSYSTEM_H

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

namespace breathe
{
  namespace render
  {
    class cWindow;
    class cContext;
    class cResourceManager;

    class cResolution
    {
    public:
      friend class cContext;

      cResolution();

      size_t GetWidth() const { return width; }
      void SetWidth(size_t _width) { width = _width; }
      size_t GetHeight() const { return height; }
      void SetHeight(size_t _height) { height = _height; }

      size_t GetColourDepth() const { return colourDepth; }
      void SetColourDepth(size_t _colourDepth) { colourDepth = _colourDepth; }

      bool IsFullScreen() const { return bIsFullScreen; }
      bool IsWindowed() const { return !bIsFullScreen; }
      void SetFullScreen(bool _bIsFullScreen) { bIsFullScreen = _bIsFullScreen; }
      void SetWindowed(bool _bIsWindowed) { bIsFullScreen = !_bIsWindowed; }

      bool IsWideScreen() const;
      float_t GetRatio() const { ASSERT(height != 0); return (width) / (height); }

      static bool ResolutionCompare(const cResolution& lhs, const cResolution& rhs);

    private:
      size_t width;
      size_t height;
      size_t colourDepth;
      bool bIsFullScreen;
    };

    class cCapabilities
    {
    public:
      cCapabilities() { Clear(); }

      void Clear();

      bool bIsOpenGLTwoPointZeroOrLaterSupported;
      bool bIsOpenGLThreePointZeroOrLaterSupported;
      bool bIsShadersTwoPointZeroOrLaterSupported;
      bool bIsVertexBufferObjectSupported;
      bool bIsFrameBufferObjectSupported;
      bool bIsFSAASupported;
      bool bIsShadowsSupported;
      bool bIsCubemappingSupported;
      size_t nTextureUnits;
      size_t iMaxTextureSize;
    };

    class cSystem
    {
    public:
      cSystem();
      ~cSystem();

      bool Create();
      void Destroy();

      cResolution GetCurrentScreenResolution() const;
      std::vector<cResolution> GetAvailableScreenResolutions() const;

      cContext* GetContext();
      cWindow* GetWindow();
      bool CreateContextAndWindow(const cResolution& resolution);
      void DestroyContextAndWindow(cContext* pContext, cWindow* pWindow);

      cResourceManager* GetResourceManager();
      cResourceManager* CreateResourceManager();
      void DestroyResourceManager(cResourceManager* pResourceManager);

      // These functions are called from cApplication
      void OnActivateWindow();
      void OnDeactivateWindow();
      void OnResizeWindow(size_t width, size_t height);
      void OnToggleFullScreen();

    private:
      NO_COPY(cSystem);

      cWindow* pWindow;
      cContext* pContext;
      cResourceManager* pResourceManager;
    };
  }
}

#endif // CRENDERSYSTEM_H
