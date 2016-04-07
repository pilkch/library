// Standard headers
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <cmath>

#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <vector>
#include <map>
#include <list>
#include <algorithm>

// Anything else
#include <GL/GLee.h>
#include <GL/glu.h>

#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/cTimer.h>

#include <spitfire/algorithm/algorithm.h>

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/settings.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/geometry.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/util/base.h>
#include <breathe/util/cVar.h>

#include <breathe/render/cContext.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cSystem.h>
#include <breathe/render/cWindow.h>
#include <breathe/render/cResourceManager.h>
#include <breathe/render/cVertexBufferObject.h>

#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

namespace breathe
{
  namespace render
  {
    // *** cCapabilities

    void cCapabilities::Clear()
    {
      bIsOpenGLTwoPointZeroOrLaterSupported = false;
      bIsOpenGLThreePointZeroOrLaterSupported = false;
      bIsShadersTwoPointZeroOrLaterSupported = false;
      bIsVertexBufferObjectSupported = false;
      bIsFrameBufferObjectSupported = false;
      bIsFSAASupported = false;
      bIsShadowsSupported = false;
      bIsCubemappingSupported = false;
      nTextureUnits = 0;
      iMaxTextureSize = 0;
    }


    // *** cSystem

    cSystem::cSystem() :
      pWindow(nullptr),
      pContext(nullptr),
      pResourceManager(nullptr)
    {
    }

    cSystem::~cSystem()
    {
      ASSERT(pResourceManager == nullptr);
      ASSERT(pContext == nullptr);
      ASSERT(pWindow == nullptr);
    }

    bool cSystem::Create()
    {
      LOG<<"cSystem::Create"<<std::endl;

      ASSERT(pContext == nullptr);
      pContext = new cContext;

      return true;
    }

    void cSystem::Destroy()
    {
      SAFE_DELETE(pContext);
    }


    cResolution cSystem::GetCurrentScreenResolution() const
    {
      return pContext->GetCurrentScreenResolution();
    }

    std::vector<cResolution> cSystem::GetAvailableScreenResolutions() const
    {
      return pContext->GetAvailableScreenResolutions();
    }

    cContext* cSystem::GetContext()
    {
      ASSERT(pContext != nullptr);
      return pContext;
    }

    cWindow* cSystem::GetWindow()
    {
      ASSERT(pWindow != nullptr);
      return pWindow;
    }

    bool cSystem::CreateContextAndWindow(const cResolution& resolution)
    {
      ASSERT(pContext != nullptr);
      ASSERT(pWindow == nullptr);

      pWindow = new cWindow;
      pWindow->Create(resolution);

      pContext->CreateSharedContextFromWindow(*pWindow);

      // These are the dodgy global variables, these have to go
      ::pContext = pContext; // TODO: Remove this

      return true;
    }

    void cSystem::DestroyContextAndWindow(cContext* _pContext, cWindow* _pWindow)
    {
      pWindow->Destroy();
      SAFE_DELETE(pWindow);

      pContext->Destroy();
    }


    cResourceManager* cSystem::GetResourceManager()
    {
      ASSERT(pResourceManager != nullptr);
      return pResourceManager;
    }

    cResourceManager* cSystem::CreateResourceManager()
    {
      ASSERT(pResourceManager == nullptr);
      pResourceManager = new cResourceManager(*pContext);
      pResourceManager->Create();

      // These are the dodgy global variables, these have to go
      ::pResourceManager = pResourceManager; // TODO: Remove this

      return pResourceManager;
    }

    void cSystem::DestroyResourceManager(cResourceManager* _pResourceManager)
    {
      pResourceManager->Destroy();
      SAFE_DELETE(pResourceManager);
    }



    // These functions are called from cApplication

    void cSystem::OnActivateWindow()
    {
    }

    void cSystem::OnDeactivateWindow()
    {
    }

    void cSystem::OnResizeWindow(size_t width, size_t height)
    {
      // TODO: For this function can we avoid destroying the window?

      cResolution resolution = pContext->GetResolution();
      ASSERT(resolution.IsWindowed()); // Resize is only appropriate for windowed mode

      // Destroy the old context and window
      pWindow->Destroy();
      pContext->Destroy();

      // Set the width and height of the new resolution
      resolution.SetWidth(width);
      resolution.SetHeight(height);

      // Create the new render
      pWindow->Create(resolution);
      pContext->CreateSharedContextFromWindow(*pWindow);

      // TODO: Is this still valid?
      pResourceManager->ReloadTextures();
    }

    void cSystem::OnToggleFullScreen()
    {
      cResolution resolution = pContext->GetResolution();

      // Destroy the old context and window
      pWindow->Destroy();
      pContext->Destroy();

      // Toggle the fullscreen or window flag of the new resolution
      if (resolution.IsFullScreen()) resolution.SetWindowed(true);
      else {
        resolution.SetFullScreen(true);

        // Find an appropriate resolution
        // TODO: Get the resolution from the list of fullscreen resolutions
        size_t width = resolution.GetWidth();
        size_t height = resolution.GetHeight();
#ifdef BUILD_DEBUG
        width = 1024;
        height = 768;
#else
        if (width < 1600) {
          if (width < 1280) {
            if (width < 1024) {
              if (width < 800) {
                width = 640;
                height = 480;
              } else {
                width = 800;
                height = 600;
              }
            } else {
              width = 1024;
              height = 768;
            }
          } else {
            width = 1280;
            height = 1024;
          }
        } else {
          width = 1600;
          height = 1280;
        }
#endif

        resolution.SetWidth(width);
        resolution.SetHeight(height);
      }

      // Create the new render
      pWindow->Create(resolution);
      pContext->CreateSharedContextFromWindow(*pWindow);

      // TODO: Is this still valid?
      pResourceManager->ReloadTextures();
    }



    cResolution::cResolution() :
      width(640),
      height(480),
      colourDepth(32),
      bIsFullScreen(false)
    {
    }

    bool cResolution::IsWideScreen() const
    {
      const float fWideScreenRatio = 16.0f / 9.0f;

      const float fRatio = (float(width) / float(height));

      return (fRatio >= fWideScreenRatio);
    }

    bool cResolution::ResolutionCompare(const cResolution& lhs, const cResolution& rhs)
    {
      if (lhs.IsWideScreen() > rhs.IsWideScreen()) return true;
      if (lhs.GetColourDepth() > rhs.GetColourDepth()) return true;

      if (lhs.GetWidth() > rhs.GetWidth()) return true;

      return (lhs.GetHeight() > rhs.GetHeight());
    }
  }
}
