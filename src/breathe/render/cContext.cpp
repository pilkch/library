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

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cContext.h>
#include <breathe/render/cVertexBufferObject.h>

#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

breathe::cVar fDetailScale = 0.5f;
const float fMaximumViewDistance = 3000.0f;

namespace breathe
{
  namespace render
  {
    // *** cStatistics

    cStatistics::cStatistics()
    {
      Reset();
    }

    void cStatistics::Reset()
    {
      nStateChanges = 0;
      nTrianglesRendered = 0;
      nModelsRendered = 0;
      nSceneObjectsRendered = 0;
      nVertexBufferObjectsBound = 0;
      nVertexBufferObjectsRendered = 0;
    }


    class cDynamicShadowMap;

    class cShadowMapGenerator
    {
    public:
      void SetAmbientShadowColour(const math::cColour& colour) { ambientShadowColour = colour; }
      void SetSunColour(const math::cColour& colour) { sunColour = colour; }
      void SetSunPosition(const math::cVec3& position) { sunPosition = position; }

      void Apply(cDynamicShadowMap& shadowMap);

    private:
      // Returns true if there is a collision between positionFrom and positionTo
      bool DoesRayCollide(cDynamicShadowMap& shadowMap, const math::cVec3& positionFrom, const math::cVec3& positionTo);

      math::cColour ambientShadowColour;
      math::cColour sunColour;
      math::cVec3 sunPosition;
    };


    // Generated from node local x, y so there may be artifacts at the edges between one node and the next as we don't take into account
    // geometry/pixels in the next node
    class cDynamicShadowMap
    {
    public:
      typedef math::cColour pixel_t;

      friend class cShadowMapGenerator;

      void GenerateLighting(cShadowMapGenerator& generator);

    private:
      float fWidthUnitsPerPixel;
      float fHeightUnitsPerPixel;
      math::cVec3 position;
      cDynamicContainer2D<pixel_t> pixels;
    };


    // Returns true if there is a collision between positionFrom and positionTo
    bool cShadowMapGenerator::DoesRayCollide(cDynamicShadowMap& shadowMap, const math::cVec3& positionFrom, const math::cVec3& positionTo)
    {
      return true;
    }

    void cShadowMapGenerator::Apply(cDynamicShadowMap& shadowMap)
    {
      //const size_t n = shadowMap.pixels.size();
      const size_t width = shadowMap.pixels.GetWidth();
      const size_t height = shadowMap.pixels.GetHeight();
      float fZ = 10.0f;

      // Generate lighting for each pixel
      for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
          //const size_t i = y * width + x;
          cDynamicShadowMap::pixel_t& pixel = shadowMap.pixels.GetElement(x, y);

          // All pixels start off as being the ambient shadow colour
          pixel = ambientShadowColour;

          // Add the sun colour as well if there is nothing in the way
          if (!DoesRayCollide(shadowMap, shadowMap.position + math::cVec3(x * shadowMap.fWidthUnitsPerPixel, y * shadowMap.fHeightUnitsPerPixel, fZ), sunPosition)) pixel += sunColour;
        }
      }

      // Gaussian blur the whole image to soften the edges of the shadowmap
      // Blur horizontally
      // Blur vertically
    }



    void cDynamicShadowMap::GenerateLighting(cShadowMapGenerator& generator)
    {
      generator.Apply(*this);
    }


    // *** cBatchController

/*    void cBatchController::FinishAdding()
    {
      // Render all opaque front-to-back
      // Render all transparent back-to-front

      cBatchList* pList = nullptr;

      // Opaque
      iterator iter = opaque.begin();
      iterator iterEnd = opaque.end();
      while (iter != iterEnd) {
        pList = iter->second;
        pList->FinishAddingSortFrontToBack();

        iter++;
      };

      // Transparent
      iter = transparent.begin();
      iterEnd = transparent.end();
      while (iter != iterEnd) {
        pList = iter->second;
        pList->FinishAddingSortBackToFront();

        iter++;
      };
    }

    cBatchModelContainer::cBatchModelContainer(model::cStaticRef _pModel, float _fDistanceFromCamera)
     : fDistanceFromCamera(_fDistanceFromCamera), pModel(_pModel)
    {}

    bool cBatchModelContainer::SortBackToFront(const cBatchModelContainer* lhs, const cBatchModelContainer* rhs)
    {
      return (lhs->fDistanceFromCamera > rhs->fDistanceFromCamera);
    }

    bool cBatchModelContainer::SortFrontToBack(const cBatchModelContainer* lhs, const cBatchModelContainer* rhs)
    {
      return (lhs->fDistanceFromCamera < rhs->fDistanceFromCamera);
    }

    void cBatchList::FinishAddingSortFrontToBack()
    {
      std::sort(models.begin(), models.end(), cBatchModelContainer::SortFrontToBack);
    }

    void cBatchList::FinishAddingSortBackToFront()
    {
      std::sort(models.begin(), models.end(), cBatchModelContainer::SortBackToFront);
    }*/

    // *** cContext

    cContext::cContext() :

#ifdef BUILD_DEBUG
      bFullscreen(false),
#else
      bFullscreen(true),
#endif

      uiWidth(1024),
      uiHeight(768),
      uiDepth(32),

      bIsRenderWireframe(false),
      bIsRenderGui(true),
      bIsLightingEnabled(true),
      bIsCubemappingEnabled(false),
      bIsRenderWithShadersEnabled(false),
      bIsFSAAEnabled(false),

      uiFSAASampleLevel(8),


      clearColour(1.0f, 0.0f, 1.0f, 1.0f),
      bIsActiveColour(false)
    {
      pContext = this;

      sunPosition.Set(10.0f, 10.0f, 5.0f, 0.0f);

      for (size_t i = 0; i < material::nLayers; i++) {
        material::cLayer layer;
        vLayer.push_back(layer);
      }
    }

    cContext::~cContext()
    {
      // Delete layers
      vLayer.clear();
    }

    string_t cContext::GetErrorString(GLenum error) const
    {
      switch (error) {
        case GL_NO_ERROR: return TEXT("GL_NO_ERROR");
        case GL_INVALID_ENUM: return TEXT("GL_INVALID_ENUM");
        case GL_INVALID_VALUE: return TEXT("GL_INVALID_VALUE");
        case GL_INVALID_OPERATION: return TEXT("GL_INVALID_OPERATION");
        case GL_STACK_OVERFLOW: return TEXT("GL_STACK_OVERFLOW");
        case GL_STACK_UNDERFLOW: return TEXT("GL_STACK_UNDERFLOW");
        case GL_OUT_OF_MEMORY: return TEXT("GL_OUT_OF_MEMORY");
      };

      return TEXT("Unknown error");
    }

    string_t cContext::GetErrorString() const
    {
      return GetErrorString(glGetError());
    }

    bool cContext::FindExtension(const string_t& sExt) const
    {
      std::ostringstream t;
      t<<const_cast<const unsigned char*>(glGetString(GL_EXTENSIONS));

      return (breathe::string::ToString_t(t.str()).find(sExt) != string_t::npos);
    }

    float cContext::GetShaderVersion() const
    {
      float fGLVersion = 0.0f;
      {
        const std::string sVersion((const char*)glGetString(GL_VERSION));

        fGLVersion = breathe::string::ToFloat(breathe::string::ToString_t(sVersion));
      }

      float fGLSLVersion = 0.0f;
      if (fGLVersion >= 2.0f) {
        const char* ptr = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

        if (ptr != nullptr) {
          std::string temp(ptr);

          std::istringstream stm(temp);
          stm >> fGLSLVersion;
        }
      }

      // Shader Model 4.0
      if (fGLSLVersion >= 4.0f) return fGLSLVersion;
      if (FindExtension(TEXT("GL_NV_gpu_program4")) || FindExtension(TEXT("GL_NV_geometry_program4")) ||
        FindExtension(TEXT("GL_NV_vertex_program4")) || FindExtension(TEXT("GL_NV_fragment_program4")) ||
        FindExtension(TEXT("GL_EXT_gpu_shader4")) || FindExtension(TEXT("GL_EXT_geometry_shader4"))) return 4.0f;

      // Shader Model 3.0
      if (fGLSLVersion >= 3.0f) return fGLSLVersion;
      if (FindExtension(TEXT("GL_NV_vertex_program3")) || FindExtension(TEXT("GL_NV_fragment_program2")) ||
        FindExtension(TEXT("GL_ATI_shader_texture_lod"))) return 3.0f;

      // Shader Model 2.0
      if (fGLSLVersion >= 2.0f) return fGLSLVersion;

      // Shader Model 1.0
      if (fGLSLVersion >= 1.0f) return fGLSLVersion;
      if (FindExtension(TEXT("GL_ARB_shading_language_100"))) return 1.0f;

      // Shader Model before 1.0
      return 0.0f;
    }

    cResolution cContext::GetResolution() const
    {
      cResolution resolution;
      resolution.SetWidth(uiWidth);
      resolution.SetHeight(uiHeight);
      resolution.SetColourDepth(uiDepth);
      resolution.SetFullScreen(bFullscreen);

      return resolution;
    }

    bool cContext::IsMultiSampling() const
    {
      GLint iBuffers = 0;
      glGetIntegerv(GL_SAMPLE_BUFFERS_ARB, &iBuffers);

      return (iBuffers != 0);
    }

    size_t cContext::GetMultiSampleLevel() const
    {
      ASSERT(IsMultiSampling());

      GLint iSampleLevel = 0;
      glGetIntegerv(GL_SAMPLES_ARB, &iSampleLevel);

      if (iSampleLevel < 0) iSampleLevel = 0;

      return size_t(iSampleLevel);
    }

    cCapabilities cContext::GetCapabilities() const
    {
      LOG<<"cContext::GetCapabilities"<<std::endl;

      cCapabilities capabilities;

      // Fetch the video info
      const SDL_VideoInfo* pVideoInfo = SDL_GetVideoInfo();

      if (pVideoInfo == nullptr) {
        LOG.Error("SDL", std::string("Video query failed: ") + SDL_GetError());
        ASSERT(false);
      }

      {
        CONSOLE<<"Resolutions"<<std::endl;
        const std::vector<cResolution>& resolutions = GetAvailableScreenResolutions();

        const size_t n = resolutions.size();
        for (size_t i = 0; i < n; i++) {
          const cResolution& r = resolutions[i];
          std::cout<<r.GetWidth()<<"x"<<r.GetHeight()<<"x"<<r.GetColourDepth()<<" "<<(r.IsWideScreen() ? "widescreen" : "standard")<<std::endl;
        };
      }

      return capabilities;
    }


    bool cContext::CreateSharedContextFromWindow(const cWindow& window)
    {
      LOG<<"cContext::CreateSharedContextFromWindow"<<std::endl;

      return false;
    }

    bool cContext::CreateSharedContextFromContext(const cContext& context)
    {
      LOG<<"cContext::CreateSharedContextFromContext"<<std::endl;

      return false;
    }

#if 0
    bool cContext::Create(const cResolution& resolution)
    {
      LOG<<"cContext::Create"<<std::endl;


      unsigned int uiFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;


      if (bFullscreen) {
        LOG.Success("App", "Going to fullscreen");
        uiFlags |= SDL_FULLSCREEN;
      } else {
        LOG.Success("App", "Going to windowed");
        uiFlags &= ~SDL_FULLSCREEN;
      }



      const SDL_VideoInfo* pVideoInfo = SDL_GetVideoInfo();

      if (pVideoInfo == nullptr) {
        LOG.Error("SDL", std::string("Video query failed: ") + SDL_GetError());
        return false;
      }


      // This checks to see if surfaces can be stored in memory
      if (pVideoInfo->hw_available) {
        uiFlags |= SDL_HWSURFACE;
        uiFlags &= ~SDL_SWSURFACE;
      } else {
        LOG.Error("SDL", "SOFTWARE SURFACE");
        uiFlags |= SDL_SWSURFACE;
        uiFlags &= ~SDL_HWSURFACE;
      }

      // This checks if hardware blits can be done
      if (pVideoInfo->blit_hw) uiFlags |= SDL_HWACCEL;
      else {
        LOG.Error("SDL", "SOFTWARE BLIT");
        uiFlags &= ~SDL_HWACCEL;
      }

      // Sets up OpenGL double buffering
      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

      // We definitely want the OpenGL flag for SDL_SetVideoMode
      ASSERT(uiFlags & SDL_OPENGL);





      capabilities = GetCapabilities();


      if (!GLeeInit()) {
        LOG.Error("cContext::Create", "GLeeInit Failed");
        LOG<<GLeeGetErrorString()<<std::endl;
      } else {
        LOG.Success("cContext::Create", "GLeeInit Succeeded");
        LOG<<GLeeGetErrorString()<<std::endl;
      }


      // Create an SDL surface
      LOG<<"cContext::Create Calling SDL_SetVideoMode"<<std::endl;
      pSurface = SDL_SetVideoMode(uiWidth, uiHeight, uiDepth, uiFlags);

      // Verify there is a surface
      if (pSurface == nullptr) {
        LOG.Error("SDL", std::string("Video mode set failed: ") + SDL_GetError());
        return false;
      }



      std::ostringstream t;
      LOG.Success("Render", "");

      const char* szValue = nullptr;

      szValue = (const char*)glGetString(GL_VENDOR);
      ASSERT(szValue != nullptr);
      LOG.Success("Render", std::string("Vendor     : ") + szValue);
      szValue = (const char*)glGetString(GL_RENDERER);
      ASSERT(szValue != nullptr);
      LOG.Success("Render", std::string("Renderer   : ") + szValue);
      szValue = (const char*)glGetString(GL_VERSION);
      ASSERT(szValue != nullptr);
      LOG.Success("Render", std::string("Version    : ") + szValue);
      szValue = (const char*)glGetString(GL_EXTENSIONS);
      ASSERT(szValue != nullptr);
      LOG.Success("Render", std::string("Extensions : ") + szValue);

      std::ostringstream tVendor;
      tVendor<<glGetString(GL_VENDOR);

      const std::string sVendor(tVendor.str());
      bool bIsNVIDIA = (sVendor.find("NVIDIA") != std::string::npos);
      bool bIsATI = (sVendor.find("ATI") != std::string::npos) || (sVendor.find("AMD") != std::string::npos);
      if (!bIsNVIDIA && !bIsATI) {
        LOG.Error("Render", std::string("Vendor is neither ATI nor NVIDIA") + sVendor);
      }

      if (FindExtension(TEXT("GL_ARB_multitexture"))) LOG.Success("Render", "Found GL_ARB_multitexture");
      else {
        LOG.Error("Render", "Not Found GL_ARB_multitexture");
        ASSERT(false);
      }


      GLint iValue = 0;
      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &iValue);
      ASSERT(iValue > 0);
      capabilities.iMaxTextureSize = iValue;

      t.str("");
      t << capabilities.iMaxTextureSize;
      if (capabilities.iMaxTextureSize >= MAX_TEXTURE_SIZE) {
        LOG.Success("Render", std::string("Max Texture Size : ") + t.str());
        capabilities.iMaxTextureSize = MAX_TEXTURE_SIZE;
      } else LOG.Error("Render", std::string("Max Texture Size : ") + t.str());

      // Cube Map Support
      if (FindExtension(TEXT("GL_ARB_texture_cube_map"))) {
        LOG.Success("Render", "Found GL_ARB_texture_cube_map");
        capabilities.bIsCubemappingSupported = true;
      } else {
        LOG.Error("Render", "Not Found GL_ARB_texture_cube_map");
      }


      // GLSL Version
      float fShaderVersion = GetShaderVersion();
      {
        std::ostringstream stm;
        stm<<fShaderVersion;

        if (fShaderVersion >= 1.0f) {
          LOG.Success("Render", "Found Shader" + stm.str());
          capabilities.bIsShadersTwoPointZeroOrLaterSupported = true;
        } else {
          LOG.Error("Render", "Not Found Shader1.1, version found is Shader" + stm.str());
          capabilities.bIsShadersTwoPointZeroOrLaterSupported = false;
        }
      }

      if (capabilities.bIsShadersTwoPointZeroOrLaterSupported) LOG.Success("Render", "Can use shaders, shaders turned on");
      else LOG.Success("Render", "Cannot use shaders, shaders turned off");


      // Frame Buffer Object Support
      if (FindExtension(TEXT("GL_EXT_framebuffer_object"))) {
        LOG.Success("Render", "Found GL_EXT_framebuffer_object");
        capabilities.bIsFrameBufferObjectSupported = true;
      } else LOG.Error("Render", "Not Found GL_EXT_framebuffer_object");


      capabilities.bIsFSAASupported = (GL_ARB_multisample != 0);
      if (capabilities.bIsFSAASupported) {
        // Only sample at 2, 4, 8 or 16
        if (uiFSAASampleLevel > 16) uiFSAASampleLevel = 16;
        else if (uiFSAASampleLevel > 8) uiFSAASampleLevel = 8;
        else if (uiFSAASampleLevel > 4) uiFSAASampleLevel = 4;
        else uiFSAASampleLevel = 2;

        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, uiFSAASampleLevel);
      }





      bIsFSAAEnabled = false;

      if (capabilities.bIsFSAASupported) {
        if (!IsMultiSampling()) {
            bIsFSAAEnabled = false;
        } else {
            size_t uiActualSampleLevel = GetMultiSampleLevel();
            if (uiFSAASampleLevel != uiActualSampleLevel) {
              LOG<<"cContext::Create Requested sample level is "<<uiFSAASampleLevel<<", actual sample level is "<<uiActualSampleLevel<<std::endl;
            }

            if ((uiActualSampleLevel == 2) || (uiActualSampleLevel == 4) || (uiActualSampleLevel == 8) || (uiActualSampleLevel == 16)) bIsFSAAEnabled = true;
        }
      }

      Setup();

      SetPerspective();

      return true;
    }
#endif

    void cContext::Destroy()
    {
      ASSERT(pSurface != nullptr);

      SDL_FreeSurface(pSurface);
      pSurface = nullptr;
    }

    void cContext::Setup()
    {
      // Clear the background to our requested colour
      glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);

      // Enable clearing Of The depth buffer
      glClearDepth(1.0);

      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LEQUAL);

      glCullFace(GL_BACK);
      glFrontFace(GL_CCW);
      glEnable(GL_CULL_FACE);

      glEnable(GL_TEXTURE_2D);
      glShadeModel(GL_SMOOTH);
      glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
      glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);


      math::cColour LightAmbient(1.0f, 1.0f, 1.0f, 1.0f);
      math::cColour LightDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
      math::cColour LightSpecular(1.0f, 1.0f, 1.0f, 1.0f);

      math::cColour LightModelAmbient(0.2f, 0.2f, 0.2f, 1.0f);

      math::cColour MaterialSpecular(1.0f, 1.0f, 1.0f, 1.0f);
      math::cColour MaterialEmission(0.5f, 0.5f, 0.5f, 1.0f);

      glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.000008f); //2.5f);
      glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.00002f); //0.25f);
      glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f); //0.1f);

      glLightfv(GL_LIGHT0, GL_POSITION, sunPosition.GetPointerConst());

      glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient.GetPointerConst());
      glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse.GetPointerConst());
      glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular.GetPointerConst());

      glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient.GetPointerConst());

      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);

      glEnable(GL_COLOR_MATERIAL);
      glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

      glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular.GetPointerConst());
      glMaterialfv(GL_FRONT, GL_EMISSION, MaterialEmission.GetPointerConst());
    }

    void cContext::SetPerspective()
    {
      // Protect against a divide by zero
      ASSERT(uiHeight != 0);

      // Height / width ratio
      const GLfloat ratio = (GLfloat)uiWidth / (GLfloat)uiHeight;

      // Setup our viewport
      glViewport(0, 0, (GLint)uiWidth, (GLint)uiHeight);

      // change to the projection matrix and set our viewing volume
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();

      // Set our perspective
      gluPerspective(45.0f, ratio, 0.1f, fMaximumViewDistance);

      // Make sure we're changing the model view and not the projection
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
    }

    void cContext::EnableWireframe()
    {
      glDisable(GL_CULL_FACE);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void cContext::DisableWireframe()
    {
      glEnable(GL_CULL_FACE);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void cContext::_BeginRenderShared()
    {
#ifndef BUILD_DEBUG
      // If we are in a release build then use the colour specified
      glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);
#endif
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      if (bIsFSAAEnabled) glEnable(GL_MULTISAMPLE_ARB);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glMultMatrixf(frustum.m.GetOpenGLMatrixPointer());

      if (bIsRenderWireframe) EnableWireframe();
      else DisableWireframe();

      if (bIsLightingEnabled) {
        glLightfv(GL_LIGHT0, GL_POSITION, sunPosition.GetPointerConst());
        glEnable(GL_LIGHTING);
      } else glDisable(GL_LIGHTING);

      ClearMaterial();
    }

    void cContext::_EndRenderShared()
    {
      if (bIsFSAAEnabled) glDisable(GL_MULTISAMPLE_ARB);
    }

    void cContext::BeginRenderToTexture(cTextureFrameBufferObjectRef pTexture)
    {
      ASSERT(pTexture->IsValid());
      ASSERT(!pTexture->IsModeCubeMap());

      glEnable(GL_TEXTURE_2D);

      // First we bind the FBO so we can render to it
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, pTexture->uiFBO);

      // Save the view port settings and set it to the size of the texture
      glPushAttrib(GL_VIEWPORT_BIT);
      glViewport(0, 0, pTexture->GetWidth(), pTexture->GetHeight());

      _BeginRenderShared();
    }

    void cContext::EndRenderToTexture(cTextureFrameBufferObjectRef pTexture)
    {
      _EndRenderShared();

      // Restore old view port settings and set rendering back to default frame buffer
      glPopAttrib();
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

      pTexture->GenerateMipMapsIfRequired();

      glDisable(GL_TEXTURE_2D);
    }

    void cContext::BeginRenderToCubeMapTextureFace(cTextureFrameBufferObjectRef pTexture, CUBE_MAP_FACE face)
    {
      ASSERT(pTexture->IsValid());
      ASSERT(pTexture->IsModeCubeMap());

      glEnable(GL_TEXTURE_2D);

      // First we bind the FBO so we can render to it
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, pTexture->uiFBO);


      GLenum openGLFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
      if (face == CUBE_MAP_FACE::NEGATIVE_X) openGLFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
      else if (face == CUBE_MAP_FACE::POSITIVE_Y) openGLFace = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
      else if (face == CUBE_MAP_FACE::NEGATIVE_Y) openGLFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
      else if (face == CUBE_MAP_FACE::POSITIVE_Z) openGLFace = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
      else if (face == CUBE_MAP_FACE::NEGATIVE_Z) openGLFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;

      // Bind the actual face we want to render to
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, openGLFace, pTexture->uiTexture, 0);


      // Save the view port settings and set it to the size of the texture
      glPushAttrib(GL_VIEWPORT_BIT);
      glViewport(0, 0, pTexture->GetWidth(), pTexture->GetHeight());

      _BeginRenderShared();
    }

    void cContext::EndRenderToCubeMapTextureFace(cTextureFrameBufferObjectRef pTexture)
    {
      EndRenderToTexture(pTexture);
    }

    void cContext::_BeginRenderToScreen()
    {
      // Set viewport
      glViewport(0, 0, uiWidth, uiHeight);

      _BeginRenderShared();

      uiTextureModeChanges = uiTextureChanges = 0;

      statistics.Reset();
    }

    void cContext::_EndRenderToScreen()
    {
      _EndRenderShared();

      SDL_GL_SwapWindow(pWindow);
    }

    void cContext::BeginRenderToScreen()
    {
      _BeginRenderToScreen();
    }

    void cContext::EndRenderToScreen()
    {
      _EndRenderToScreen();
    }

    void cContext::_RenderPostRenderPass(material::cMaterialRef pMaterial, cTextureFrameBufferObjectRef pTexture)
    {
      ASSERT(pMaterial != nullptr);
      ASSERT(pTexture != nullptr);
      ASSERT(pTexture->IsValid());

      BeginScreenSpaceRendering();
        ApplyMaterial(pMaterial);
          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D, pTexture->uiTexture);
          RenderScreenSpaceRectangleTopLeftIsAt(0.0f, 0.0f, 1.0f, 1.0f);
        UnApplyMaterial(pMaterial);
      EndScreenSpaceRendering();
    }

    void cContext::SaveScreenShot()
    {
      int value = 0;
      GetApplicationUserSetting(TEXT("ScreenShot"), TEXT("Count"), value);
      SetApplicationUserSetting(TEXT("ScreenShot"), TEXT("Count"), value + 1);

      ostringstream_t o;
      o<<filesystem::GetHomeImagesDirectory();
      o<<TEXT("screenshot");
      o<<value;
      o<<TEXT(".bmp");

      const string_t sFilename = o.str();

      SDL_Surface* pScreenSurface = pSurface;
      ASSERT(pScreenSurface != nullptr);

      if (!(pScreenSurface->flags & SDL_OPENGL)) {
        SDL_SaveBMP(pScreenSurface, breathe::string::ToUTF8(sFilename).c_str());
        return;
      }

      SDL_Surface* pTempSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, pScreenSurface->w, pScreenSurface->h, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);
      if (pTempSurface == nullptr) return;

      // TODO: Can we use new/delete please?
      unsigned char* pPixels = (unsigned char*)malloc(3 * pScreenSurface->w * pScreenSurface->h);
      ASSERT(pPixels != nullptr);

      glReadPixels(0, 0, pScreenSurface->w, pScreenSurface->h, GL_RGB, GL_UNSIGNED_BYTE, pPixels);

      ASSERT(pScreenSurface->h >= 0);
      const size_t n = pScreenSurface->h;
      for (size_t i = 0; i < n; i++) memcpy(((char*)pTempSurface->pixels) + pTempSurface->pitch * i, pPixels + 3 * pScreenSurface->w * (pScreenSurface->h-i - 1), pScreenSurface->w * 3);

      free(pPixels);
      pPixels = nullptr;

      SDL_SaveBMP(pTempSurface, breathe::string::ToUTF8(sFilename).c_str());
      SDL_FreeSurface(pTempSurface);
    }


    void cContext::BeginScreenSpaceRendering()
    {
      glDisable(GL_LIGHTING);

      ClearMaterial();


      glFrontFace(GL_CW);

      // Our screen coordinates look like this
      // 0.0f, 0.0f            1.0f, 0.0f
      //
      //
      // 0.0f, 1.0f            1.0f, 1.0f

      // Setup projection matrix
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f); // Invert Y axis so increasing Y goes down.

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
          glLoadIdentity();
    }

    void cContext::EndScreenSpaceRendering()
    {
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

      glMatrixMode(GL_PROJECTION);  // Select Projection
      glPopMatrix();                  // Pop The Matrix

      // TODO: Can we remove glCullFace and glEnable here?
      glCullFace(GL_BACK);
      glFrontFace(GL_CCW);
      glEnable(GL_CULL_FACE);

      glEnable(GL_LIGHTING);
    }

    void cContext::BeginScreenSpaceGuiRendering()
    {
      glFrontFace(GL_CW);

      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f); // Invert Y axis so increasing Y goes down.

        // Setup modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
          // TODO: Uncomment this for console what is going on? ???????????????
          //glLoadIdentity();
    }

    void cContext::EndScreenSpaceGuiRendering()
    {
          glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();

      glFrontFace(GL_CCW);
    }

    // In this mode y is 1..0
    void cContext::BeginScreenSpaceWorldRendering(float fScale)
    {
      glFrontFace(GL_CCW);

      glMatrixMode(GL_PROJECTION); // Start modifying the projection matrix.
      glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0f, fScale, fScale, 0.0f, -1.0f, 1.0f);

        // Setup modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
    }

    void cContext::EndScreenSpaceWorldRendering()
    {
          glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();

      glFrontFace(GL_CW);
    }

    void cContext::PushScreenSpacePosition(float x, float y)
    {
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
        //glLoadIdentity();
        glTranslatef(x, -y, 0.0f);
    }

    void cContext::PopScreenSpacePosition()
    {
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
    }


    // *** Screen space rendering

    // Our screen coordinates look like this
    // 0.0f, 0.0f            1.0f, 0.0f
    //
    //
    // 0.0f, 1.0f            1.0f, 1.0f

    void cContext::RenderScreenSpacePolygon(float fX, float fY,
        float fVertX0, float fVertY0, float fVertX1, float fVertY1,
        float fVertX2, float fVertY2, float fVertX3, float fVertY3)
    {
      glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(fX + fVertX0, fY + fVertY0);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(fX + fVertX1, fY + fVertY1);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(fX + fVertX2, fY + fVertY2);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(fX + fVertX3, fY + fVertY3);
      glEnd();
    }

    // Render a solid filled rectangle with one colour and no texturing and a separately coloured border
    void cContext::RenderScreenSpaceSolidRectangleWithBorderTopLeftIsAt(float fX, float fY, float fWidth, float fHeight, const math::cColour& boxColour, const math::cColour& upperBorderColour, const math::cColour& lowerBorderColour)
    {
      glColor3f(boxColour.r, boxColour.g, boxColour.b);

      glBegin(GL_QUADS);
        glVertex2f(fX, fY);
        glVertex2f(fX + fWidth, fY);
        glVertex2f(fX + fWidth, fY + fHeight);
        glVertex2f(fX, fY + fHeight);
      glEnd();


      // Draw an outline around the button with width 3
      glLineWidth(3);

      glColor3f(upperBorderColour.r, upperBorderColour.g, upperBorderColour.b);

      glBegin(GL_LINE_STRIP);
        glVertex2f(fX + fWidth, fY);
        glVertex2f(fX, fY);
        glVertex2f(fX, fY + fHeight);
      glEnd();

      glColor3f(lowerBorderColour.r, lowerBorderColour.g, lowerBorderColour.b);

      glBegin(GL_LINE_STRIP);
        glVertex2f(fX, fY + fHeight);
        glVertex2f(fX + fWidth, fY + fHeight);
        glVertex2f(fX + fWidth, fY);
      glEnd();

      glLineWidth(1);
    }

    void cContext::RenderScreenSpaceGradientFilledRectangleTopLeftIsAt(float fX, float fY, float fWidth, float fHeight, const math::cColour& colour0, const math::cColour& colour1, const math::cColour& colour2, const math::cColour& colour3)
    {
      glBegin(GL_QUADS);
        glColor3f(colour0.r, colour0.g, colour0.b); glVertex2f(fX, fY);
        glColor3f(colour1.r, colour1.g, colour1.b); glVertex2f(fX + fWidth, fY);
        glColor3f(colour2.r, colour2.g, colour2.b); glVertex2f(fX + fWidth, fY + fHeight);
        glColor3f(colour3.r, colour3.g, colour3.b); glVertex2f(fX, fY + fHeight);
      glEnd();
    }

    void cContext::RenderScreenSpaceRectangle(float fX, float fY, float fWidth, float fHeight)
    {
      const float fHalfWidth = fWidth * 0.5f;
      const float fHalfHeight = fHeight * 0.5f;
      glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(fX - fHalfWidth, fY - fHalfHeight);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(fX + fHalfWidth, fY - fHalfHeight);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(fX + fHalfWidth, fY + fHalfHeight);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(fX - fHalfWidth, fY + fHalfHeight);
      glEnd();
    }

    void cContext::RenderScreenSpaceRectangle(
      float fX, float fY, float fWidth, float fHeight,
      float fU, float fV, float fU2, float fV2)
    {
      fV *= -1.0f;
      fV2 *= -1.0f;

      const float fHalfWidth = fWidth * 0.5f;
      const float fHalfHeight = fHeight * 0.5f;
      glBegin(GL_QUADS);
        glTexCoord2f(fU, fV + fV2);       glVertex2f(fX - fHalfWidth, fY - fHalfHeight);
        glTexCoord2f(fU + fU2, fV + fV2); glVertex2f(fX + fHalfWidth, fY - fHalfHeight);
        glTexCoord2f(fU + fU2, fV);       glVertex2f(fX + fHalfWidth, fY + fHalfHeight);
        glTexCoord2f(fU, fV);             glVertex2f(fX - fHalfWidth, fY + fHalfHeight);
      glEnd();
    }

    void cContext::RenderScreenSpaceRectangleTopLeftIsAt(float fX, float fY, float fWidth, float fHeight)
    {
      glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(fX, fY);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(fX + fWidth, fY);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(fX + fWidth, fY + fHeight);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(fX, fY + fHeight);
      glEnd();
    }

    void cContext::RenderScreenSpaceRectangleTopLeftIsAt(
      float fX, float fY, float fWidth, float fHeight,
      float fU, float fV, float fU2, float fV2)
    {
      glBegin(GL_QUADS);
        glTexCoord2f(fU, fV);             glVertex2f(fX, fY);
        glTexCoord2f(fU + fU2, fV);       glVertex2f(fX + fWidth, fY);
        glTexCoord2f(fU + fU2, fV + fV2); glVertex2f(fX + fWidth, fY + fHeight);
        glTexCoord2f(fU, fV + fV2);       glVertex2f(fX, fY + fHeight);
      glEnd();
    }

    void cContext::RenderScreenSpaceRectangleRotated(float fX, float fY, float fWidth, float fHeight, float fRotation)
    {
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
        //const float fHalfWidth = fWidth * 0.5f;
        //const float fHalfHeight = fHeight * 0.5f;
        glTranslatef(fX, fY, 0.0f);
        glRotatef(fRotation, 0.0f, 0.0f, 1.0f);

        RenderScreenSpaceRectangle(0.0f, 0.0f, fWidth, fHeight);
      glPopMatrix();
    }

    void cContext::RenderScreenSpaceLines(const std::vector<math::cVec2>& points)
    {
      glBegin(GL_LINE_STRIP);
        const size_t n = points.size();
        ASSERT(n != 0);
        for (size_t i = 0; i < n; i++) glVertex2f(points[i].x, points[i].y);
      glEnd();
    }


    // *** World space rendering

    void cContext::RenderLines(const std::vector<math::cVec3>& points)
    {
      glBegin(GL_LINES);
        const size_t n = points.size();
        ASSERT(n != 0);
        for (size_t i = 0; i < n; i++) glVertex3f(points[i].x, points[i].y, points[i].z);
      glEnd();
    }

    void cContext::RenderMesh(model::cMeshRef pMesh)
    {
      ASSERT(false);
    }

    void cContext::RenderArrow(const math::cVec3& from, const math::cVec3& to, const math::cColour& colour)
    {
      SetColour(colour);
      glBegin(GL_LINES);
        glVertex3f(from.x, from.y, from.z);
        glVertex3f(to.x, to.y, to.z);
      glEnd();
    }

    void cContext::RenderAxisReference()
    {
      const math::cVec3 zero;
      RenderAxisReference(zero);
    }

    void cContext::RenderAxisReference(const math::cVec3& position)
    {
      const float fWidth = 20.0f;

      glDisable(GL_COLOR_MATERIAL);
      glDisable(GL_LIGHTING);
      glDisable(GL_TEXTURE_2D);

        SetColour(1.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
          glVertex3f(position.x, position.y, position.z);
          glVertex3f(position.x + fWidth, position.y, position.z);
        glEnd();

        SetColour(0.0f, 1.0f, 0.0f);
        glBegin(GL_LINES);
          glVertex3f(position.x, position.y, position.z);
          glVertex3f(position.x, position.y + fWidth, position.z);
        glEnd();

        SetColour(0.0f, 0.0f, 1.0f);
        glBegin(GL_LINES);
          glVertex3f(position.x, position.y, position.z);
          glVertex3f(position.x, position.y, position.z + fWidth);
        glEnd();

      glEnable(GL_TEXTURE_2D);
      glEnable(GL_LIGHTING);
      glEnable(GL_COLOR_MATERIAL);
    }

    void cContext::RenderTriangle(const math::cVec3& v0, const math::cVec3& v1, const math::cVec3& v2)
    {
      math::cVec3 n;
      n.Cross(v0 - v2, v2 - v1);
      n.Normalise();

      glBegin(GL_TRIANGLES);

        glNormal3f(n.x, n.y, n.z);

        glVertex3f(v0.x, v0.y, v0.z);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
      glEnd();
    }

    void cContext::RenderBoxTextured(const math::cVec3& vMin, const math::cVec3& vMax)
    {
      glBegin(GL_QUADS);

        // Bottom Square
        glTexCoord2f(0.0f, 0.0f); glVertex3f(vMin.x, vMin.y, vMin.z);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(vMin.x, vMax.y, vMin.z);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(vMax.x, vMax.y, vMin.z);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(vMax.x, vMin.y, vMin.z);

        // Side Edges
        glTexCoord2f(0.0f, 0.0f); glVertex3f(vMin.x, vMax.y, vMin.z);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(vMin.x, vMax.y, vMax.z);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(vMax.x, vMax.y, vMax.z);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(vMax.x, vMax.y, vMin.z);

        glTexCoord2f(0.0f, 0.0f); glVertex3f(vMax.x, vMax.y, vMin.z);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(vMax.x, vMax.y, vMax.z);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(vMax.x, vMin.y, vMax.z);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(vMax.x, vMin.y, vMin.z);

        glTexCoord2f(0.0f, 0.0f); glVertex3f(vMax.x, vMin.y, vMin.z);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(vMax.x, vMin.y, vMax.z);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(vMin.x, vMin.y, vMax.z);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(vMin.x, vMin.y, vMin.z);

        glTexCoord2f(1.0f, 0.0f); glVertex3f(vMin.x, vMin.y, vMax.z);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(vMin.x, vMax.y, vMax.z);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(vMin.x, vMax.y, vMin.z);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(vMin.x, vMin.y, vMin.z);

        // Upper Square
        glTexCoord2f(0.0f, 0.0f); glVertex3f(vMin.x, vMin.y, vMax.z);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(vMax.x, vMin.y, vMax.z);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(vMax.x, vMax.y, vMax.z);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(vMin.x, vMax.y, vMax.z);

      glEnd();
    }

    void cContext::RenderBox(const math::cVec3& vMin, const math::cVec3& vMax)
    {
      glBegin(GL_QUADS);

        // Bottom Square
        glVertex3f(vMin.x, vMin.y, vMin.z);
        glVertex3f(vMax.x, vMin.y, vMin.z);
        glVertex3f(vMax.x, vMax.y, vMin.z);
        glVertex3f(vMin.x, vMax.y, vMin.z);

        // Side Edges
        /*glVertex3f(vMin.x, vMin.y, vMin.z);
        glVertex3f(vMin.x, vMin.y, vMax.z);

        glVertex3f(vMax.x, vMin.y, vMin.z);
        glVertex3f(vMax.x, vMin.y, vMax.z);

        glVertex3f(vMin.x, vMax.y, vMin.z);
        glVertex3f(vMin.x, vMax.y, vMax.z);

        glVertex3f(vMax.x, vMax.y, vMin.z);
        glVertex3f(vMax.x, vMax.y, vMax.z);*/

        // Upper Square
        glVertex3f(vMin.x, vMin.y, vMax.z);
        glVertex3f(vMin.x, vMax.y, vMax.z);
        glVertex3f(vMax.x, vMax.y, vMax.z);
        glVertex3f(vMax.x, vMin.y, vMax.z);

      glEnd();
    }

    void cContext::RenderWireframeBox(const math::cVec3& vMin, const math::cVec3& vMax)
    {
      glBegin(GL_LINES);

        // Bottom Square
        glVertex3f(vMin.x, vMin.y, vMin.z);
        glVertex3f(vMax.x, vMin.y, vMin.z);

        glVertex3f(vMin.x, vMin.y, vMin.z);
        glVertex3f(vMin.x, vMax.y, vMin.z);

        glVertex3f(vMax.x, vMax.y, vMin.z);
        glVertex3f(vMax.x, vMin.y, vMin.z);

        glVertex3f(vMax.x, vMax.y, vMin.z);
        glVertex3f(vMin.x, vMax.y, vMin.z);

        // Side Edges
        glVertex3f(vMin.x, vMin.y, vMin.z);
        glVertex3f(vMin.x, vMin.y, vMax.z);

        glVertex3f(vMax.x, vMin.y, vMin.z);
        glVertex3f(vMax.x, vMin.y, vMax.z);

        glVertex3f(vMin.x, vMax.y, vMin.z);
        glVertex3f(vMin.x, vMax.y, vMax.z);

        glVertex3f(vMax.x, vMax.y, vMin.z);
        glVertex3f(vMax.x, vMax.y, vMax.z);

        // Upper Square
        glVertex3f(vMin.x, vMin.y, vMax.z);
        glVertex3f(vMax.x, vMin.y, vMax.z);

        glVertex3f(vMin.x, vMin.y, vMax.z);
        glVertex3f(vMin.x, vMax.y, vMax.z);

        glVertex3f(vMax.x, vMax.y, vMax.z);
        glVertex3f(vMax.x, vMin.y, vMax.z);

        glVertex3f(vMax.x, vMax.y, vMax.z);
        glVertex3f(vMin.x, vMax.y, vMax.z);

      glEnd();
    }

#if 0
    float Angle(const math::cVec2 & a, const math::cVec2 & b)
    {
      if (a.x > b.x) {
        if (a.y > b.y) return (atan((a.y-b.y)/(a.x-b.x)) + math::cPI_DIV_180 * 90.0f) * math::c180_DIV_PI;

        return (-atan((a.y-b.y)/(b.x-a.x)) + math::cPI_DIV_180 * 90.0f) * math::c180_DIV_PI;
      }


      if (b.y > a.y) return (atan((b.y-a.y)/(b.x-a.x)) + math::cPI_DIV_180 * 270.0f) * math::c180_DIV_PI;

      return (atan((b.y-a.y)/(b.x-a.x)) + math::cPI_DIV_180 * 270.0f) * math::c180_DIV_PI;
    }
#endif

    void cContext::SelectTextureUnit0()
    {
      glActiveTexture(GL_TEXTURE0);
    }

    void cContext::SelectTextureUnit1()
    {
      glActiveTexture(GL_TEXTURE1);
    }

    void cContext::SelectTextureUnit2()
    {
      glActiveTexture(GL_TEXTURE2);
    }

    bool cContext::SetTexture0(cTextureRef pTexture)
    {
      ASSERT(pTexture != nullptr);

      //Activate the correct texture unit
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, pTexture->uiTexture);
      return true;
    }

    bool cContext::SetTexture1(cTextureRef pTexture)
    {
      ASSERT(pTexture != nullptr);

      //Activate the correct texture unit
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, pTexture->uiTexture);
      return true;
    }

    material::cMaterialRef cContext::GetCurrentMaterial() const
    {
      ASSERT(pCurrentMaterial != nullptr);
      return pCurrentMaterial;
    }

    cTextureRef cContext::GetCurrentTexture0() const
    {
      ASSERT(pCurrentMaterial != nullptr);
      ASSERT(pCurrentMaterial->vLayer.size() > 0);
      return pCurrentMaterial->vLayer[0]->pTexture;
    }

    cTextureRef cContext::GetCurrentTexture1() const
    {
      ASSERT(pCurrentMaterial != nullptr);
      ASSERT(pCurrentMaterial->vLayer.size() > 1);
      return pCurrentMaterial->vLayer[1]->pTexture;
    }

    cTextureRef cContext::GetCurrentTexture2() const
    {
      ASSERT(pCurrentMaterial != nullptr);
      ASSERT(pCurrentMaterial->vLayer.size() > 2);
      return pCurrentMaterial->vLayer[2]->pTexture;
    }

    bool cContext::ClearMaterial()
    {
      unsigned int i = 0;
      unsigned int n = 0;
      unsigned int unit = GL_TEXTURE0;

      material::cLayer* layerOld;

      for (i=n;i<material::nLayers;i++, unit++)
      {
        layerOld = &vLayer[i];

        //Activate the current texture unit
        glActiveTexture(unit);

        //Undo last mode
        if (TEXTURE_MODE::MASK == layerOld->uiTextureMode ||
            TEXTURE_MODE::BLEND == layerOld->uiTextureMode)
        {
          glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
          glBlendFunc(GL_ONE, GL_ZERO);
          glDisable(GL_BLEND);
        }
        else if (TEXTURE_MODE::DETAIL==layerOld->uiTextureMode)
        {
          // Reset the texture matrix
          glMatrixMode(GL_TEXTURE);
          glLoadIdentity();
          glMatrixMode(GL_MODELVIEW);

          //glEnable(GL_LIGHTING);
        }
        else if (TEXTURE_MODE::CUBE_MAP==layerOld->uiTextureMode)
        {
          glMatrixMode(GL_TEXTURE);
          glPopMatrix();

          glMatrixMode(GL_MODELVIEW);

          glDisable(GL_TEXTURE_CUBE_MAP);

          glDisable(GL_TEXTURE_GEN_R);
          glDisable(GL_TEXTURE_GEN_T);
          glDisable(GL_TEXTURE_GEN_S);

          glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 1);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        //Set the current mode and texture
        layerOld->uiTextureMode = TEXTURE_MODE::NONE;
        layerOld->pTexture.reset();
        layerOld->sTexture = TEXT("");
      }

      glActiveTexture(GL_TEXTURE0);
      glEnable(GL_TEXTURE_2D);
      glDisable(GL_LIGHTING);

      if (capabilities.bIsShadersTwoPointZeroOrLaterSupported && (pCurrentShader != nullptr)) UnBindShader();

      pCurrentMaterial.reset();

      ClearColour();

      return true;
    }

    bool cContext::SetShaderConstant(const std::string& sConstant, int value)
    {
      ASSERT(pCurrentShader != nullptr);

      GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
      if (loc == -1) {
        LOG.Error("cContext::SetShaderConstant", breathe::string::ToUTF8(pCurrentShader->sShaderVertex) + ", " + breathe::string::ToUTF8(pCurrentShader->sShaderFragment) + " Couldn't set \"" + sConstant + "\" perhaps the constant is not actually used within the shader");
        ASSERT(loc > 0);
        return false;
      }

      glUniform1i(loc, value);
      return true;
    }

    bool cContext::SetShaderConstant(const std::string& sConstant, float value)
    {
      ASSERT(pCurrentShader != nullptr);

      GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
      if (loc == -1) {
        LOG.Error("cContext::SetShaderConstant", breathe::string::ToUTF8(pCurrentShader->sShaderVertex) + ", " + breathe::string::ToUTF8(pCurrentShader->sShaderFragment) + " Couldn't set \"" + sConstant + "\" perhaps the constant is not actually used within the shader");
        ASSERT(loc > 0);
        return false;
      }

      glUniform1f(loc, value);
      return true;
    }

    bool cContext::SetShaderConstant(const std::string& sConstant, const math::cVec2& value)
    {
      ASSERT(pCurrentShader != nullptr);

      GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
      if (loc == -1) {
        LOG.Error("cContext::SetShaderConstant", breathe::string::ToUTF8(pCurrentShader->sShaderVertex) + ", " + breathe::string::ToUTF8(pCurrentShader->sShaderFragment) + " Couldn't set \"" + sConstant + "\" perhaps the constant is not actually used within the shader");
        ASSERT(loc > 0);
        return false;
      }

      glUniform2f(loc, value.x, value.y);
      return true;
    }

    bool cContext::SetShaderConstant(const std::string& sConstant, const math::cVec3& value)
    {
      ASSERT(pCurrentShader != nullptr);

      GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
      if (loc == -1) {
        LOG.Error("cContext::SetShaderConstant", breathe::string::ToUTF8(pCurrentShader->sShaderVertex) + ", " + breathe::string::ToUTF8(pCurrentShader->sShaderFragment) + " Couldn't set \"" + sConstant + "\" perhaps the constant is not actually used within the shader");
        ASSERT(loc > 0);
        return false;
      }

      glUniform3f(loc, value.x, value.y, value.z);
      return true;
    }

    bool cContext::SetShaderConstant(const std::string& sConstant, const math::cVec4& value)
    {
      ASSERT(pCurrentShader != nullptr);

      GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
      if (loc == -1) {
        LOG.Error("cContext::SetShaderConstant", breathe::string::ToUTF8(pCurrentShader->sShaderVertex) + ", " + breathe::string::ToUTF8(pCurrentShader->sShaderFragment) + " Couldn't set \"" + sConstant + "\" perhaps the constant is not actually used within the shader");
        ASSERT(loc > 0);
        return false;
      }

      glUniform4f(loc, value.x, value.y, value.z, value.w);
      return true;
    }


    void cContext::BindShader(cShaderRef pShader)
    {
      ASSERT(pShader != nullptr);

      glUseProgram(pShader->uiShaderProgram);

      pCurrentShader = pShader;

      // TODO: We also need some more variables within our post render shaders such as
      // brightness: HDR, Top Gear Shader, Night Vision
      // exposure: HDR, Top Gear Shader
      // sunPosition: Car Shader, shadows, this could be light[0] though

      if (pShader->bCameraPos) SetShaderConstant("cameraPos", frustum.eye);
      if (pShader->bAmbientColour) {
        SetShaderConstant("ambientColour", shaderConstants.GetValueVec4(TEXT("ambientColour")));
      }
      if (pShader->bLightPosition) {
        SetShaderConstant("lightPosition", shaderConstants.GetValueVec3(TEXT("lightPosition")));
      }
      if (pShader->bTexUnit0) SetShaderConstant("texUnit0", 0);
      if (pShader->bTexUnit1) SetShaderConstant("texUnit1", 1);
      if (pShader->bTexUnit2) SetShaderConstant("texUnit2", 2);
      if (pShader->bTexUnit3) SetShaderConstant("texUnit3", 3);

      // TODO: Is this needed?
      //glEnable(GL_LIGHTING);
    }

    void cContext::UnBindShader()
    {
      // TODO: Is this needed?
      //glDisable(GL_LIGHTING);

      glUseProgram(0);

      pCurrentShader.reset();
    }


    bool cContext::ApplyMaterial(material::cMaterialRef pMaterial)
    {
      ASSERT(pMaterial != nullptr);

      uiTextureModeChanges++;
      //uiTextureChanges;

      uint32_t unit = GL_TEXTURE0_ARB;

      const size_t n = pMaterial->vLayer.size();

      // We must have at least one layer
      ASSERT(n != 0);

      // Iterate through and apply each layer
      for (size_t i = 0; i < n; i++, unit++) {
        // Activate the current texture unit
        glActiveTexture(unit);

        material::cLayer* pLayer = pMaterial->vLayer[i];
        ASSERT(pLayer != nullptr);

        switch (pLayer->uiTextureMode) {
          case TEXTURE_MODE::NONE: {
            glDisable(GL_TEXTURE_2D);
            break;
          }

          case TEXTURE_MODE::NORMAL:
          case TEXTURE_MODE::MASK:
          case TEXTURE_MODE::BLEND: {
            if (pLayer->uiTextureMode == TEXTURE_MODE::BLEND) {
              glEnable(GL_BLEND);
            }

            // We now do masking and blending in shaders so this is greatly simplified
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, pLayer->pTexture->uiTexture);
            break;
          }

          case TEXTURE_MODE::DETAIL: {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, pLayer->pTexture->uiTexture);

            // Change the texture matrix so that we have more detail than normal texture
            glMatrixMode(GL_TEXTURE);
            glPushMatrix();
              glLoadIdentity();
              glMatrixMode(GL_MODELVIEW);

            break;
          }

          case TEXTURE_MODE::CUBE_MAP: {
            if (!bIsCubemappingEnabled) {
              // What to do?
              ASSERT(false);
            }


            glEnable(GL_TEXTURE_CUBE_MAP);
            glBindTexture(GL_TEXTURE_CUBE_MAP, pLayer->pTexture->uiTexture);

            glMatrixMode(GL_TEXTURE);
            glPushMatrix();
              glLoadIdentity();

#if 0
              float y = -Angle(spitfire::math::cVec2(frustum.eye.x, frustum.eye.y), spitfire::math::cVec2(frustum.target.x, frustum.target.y));
              float x = -Angle(spitfire::math::cVec2(frustum.eye.y, frustum.eye.z), spitfire::math::cVec2(frustum.target.y, frustum.target.z));
              //std::cout<<y<<"\t"<<x<<"\n";

              glRotatef(y, 0.0f, 1.0f, 0.0f);
              glRotatef(x, 1.0f, 0.0f, 0.0f);
#elif 0
              float mat[16];
              glGetFloatv(GL_MODELVIEW_MATRIX, mat);

              math::cQuaternion q(mat[8], mat[9], -mat[10]);

              glLoadMatrixf(static_cast<float* >(q.GetMatrix()));
#endif

              glMatrixMode(GL_MODELVIEW);


              glEnable(GL_TEXTURE_GEN_S);
              glEnable(GL_TEXTURE_GEN_T);
              glEnable(GL_TEXTURE_GEN_R);

              glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
              glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
              glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

            break;
          }


          case TEXTURE_MODE::POST_RENDER: {
            glEnable(GL_TEXTURE_2D);
            // TODO: Automatically fill this with a valid texture?
            //glBindTexture(GL_TEXTURE_2D, pLayer->pTexture->uiTexture);
            break;
          }

          default: {
            LOG<<"cContext::ApplyMaterial Unknown texture type"<<std::endl;
            ASSERT(false);
          }
        }
      }

      if (capabilities.bIsShadersTwoPointZeroOrLaterSupported && (pMaterial->pShader != nullptr)) {
        BindShader(pMaterial->pShader);
      }

      pCurrentMaterial = pMaterial;

      // Set the activate texture unit to the first one as that is what everyone is expecting
      glActiveTexture(GL_TEXTURE0_ARB);

      return true;
    }

    bool cContext::UnApplyMaterial(material::cMaterialRef pMaterial)
    {
      ASSERT(pMaterial != nullptr);

      uint32_t unit = GL_TEXTURE0_ARB;

      const size_t n = pMaterial->vLayer.size();

      // We must have at least one layer
      ASSERT(n != 0);

      // Iterate through and unapply each layer
      for (size_t i = 0; i < n; i++, unit++) {
        // Activate the current texture unit
        glActiveTexture(unit);

        material::cLayer* pLayer = pMaterial->vLayer[i];
        ASSERT(pLayer != nullptr);

        switch (pLayer->uiTextureMode) {
          case TEXTURE_MODE::NONE: {
            glDisable(GL_TEXTURE_2D);
            break;
          }

          case TEXTURE_MODE::NORMAL:
          case TEXTURE_MODE::MASK:
          case TEXTURE_MODE::BLEND: {
            // We now do masking and blending in shaders so this is greatly simplified
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);

            if (pLayer->uiTextureMode == TEXTURE_MODE::BLEND) {
              glDisable(GL_BLEND);
            }

            break;
          }

          case TEXTURE_MODE::DETAIL: {
              glDisable(GL_TEXTURE_2D);
              glBindTexture(GL_TEXTURE_2D, 0);

              // Change the texture matrix so that we have more detail than normal texture
              glMatrixMode(GL_TEXTURE);
              glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
          }

          case TEXTURE_MODE::CUBE_MAP: {
            if (!capabilities.bIsCubemappingSupported) {
              // What to do?
              ASSERT(false);
            }


              glDisable(GL_TEXTURE_GEN_R);
              glDisable(GL_TEXTURE_GEN_T);
              glDisable(GL_TEXTURE_GEN_S);


              glMatrixMode(GL_TEXTURE);
              glPopMatrix();

            glMatrixMode(GL_MODELVIEW);

            glDisable(GL_TEXTURE_CUBE_MAP);

            break;
          }


          case TEXTURE_MODE::POST_RENDER: {
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            break;
          }

          default: {
            LOG<<"cContext::UnApplyMaterial Unknown texture type"<<std::endl;
            ASSERT(false);
          }
        }
      }

      // UnApply shader
      if (capabilities.bIsShadersTwoPointZeroOrLaterSupported && (pMaterial->pShader != nullptr)) {
        UnBindShader();
      }


      // Set the activate texture unit to the first one as that is what everyone is expectingpMaterial
      glActiveTexture(GL_TEXTURE0_ARB);

      return false;
    }



    void cContext::ClearColour()
    {
      bIsActiveColour = false;

      colour.SetBlack();
      colour.a = 1.0f;

      glColor4f(colour.r, colour.g, colour.b, colour.a);
    }

    void cContext::SetColour(float r, float g, float b)
    {
      math::cColour colour(r, g, b, 1.0f);
      SetColour(colour);
    }

    void cContext::SetColour(const math::cColour& inColour)
    {
      bIsActiveColour = true;

      colour = inColour;

      glColor4f(colour.r, colour.g, colour.b, colour.a);
    }




    cResolution cContext::GetCurrentScreenResolution() const
    {
      const SDL_VideoInfo* pVideoInfo = SDL_GetVideoInfo();
      ASSERT(pVideoInfo != nullptr);
      ASSERT(pVideoInfo->vfmt != nullptr);
      const int iCurrentWidth = pVideoInfo->current_w;
      const int iCurrentHeight = pVideoInfo->current_h;
      const int iCurrentColourDepthInBits = pVideoInfo->vfmt->BitsPerPixel;

      cResolution r;

      ASSERT(iCurrentWidth >= 0);
      ASSERT(iCurrentHeight >= 0);
      ASSERT(iCurrentColourDepthInBits >= 0);
      r.SetWidth(size_t(iCurrentWidth));
      r.SetHeight(size_t(iCurrentHeight));
      r.SetColourDepth(size_t(iCurrentColourDepthInBits));
      r.SetFullScreen(true);

      return r;
    }

    std::vector<cResolution> cContext::GetAvailableScreenResolutions() const
    {
      std::vector<cResolution> resolutions;

      const SDL_VideoInfo* pVideoInfo = SDL_GetVideoInfo();
      ASSERT(pVideoInfo != nullptr);
      ASSERT(pVideoInfo->vfmt != nullptr);
      const int iCurrentWidth = pVideoInfo->current_w;
      const int iCurrentHeight = pVideoInfo->current_h;
      const int iCurrentColourDepthInBits = pVideoInfo->vfmt->BitsPerPixel;

      ASSERT(iCurrentWidth >= 0);
      ASSERT(iCurrentHeight >= 0);
      ASSERT(iCurrentColourDepthInBits >= 0);

      SDL_Rect** modes = SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_HWSURFACE);
      if (modes == (SDL_Rect**)0 || modes == (SDL_Rect**)-1) {
        CONSOLE<<"No modes available"<<std::endl;
        cResolution r;
        r.SetWidth(size_t(iCurrentWidth));
        r.SetHeight(size_t(iCurrentHeight));
        r.SetColourDepth(size_t(iCurrentColourDepthInBits));
        r.SetFullScreen(true);
        resolutions.push_back(r);
      } else {
        for (size_t i = 0; modes[i] != nullptr; i++) {
          cResolution r;
          r.SetWidth(size_t(modes[i]->w));
          r.SetHeight(size_t(modes[i]->h));
          r.SetColourDepth(size_t(iCurrentColourDepthInBits));
          r.SetFullScreen(true);
          resolutions.push_back(r);
        }
      }

      std::sort(resolutions.begin(), resolutions.end(), cResolution::ResolutionCompare);

      return resolutions;
    }


    void cContext::PushProjectionMatrix()
    {
      math::cMat4 matrix;

      // If we already have a matrix then get a copy of it
      if (!lMatProjection.empty()) matrix = lMatProjection.back();

      lMatProjection.push_back(matrix);
    }

    void cContext::PopProjectionMatrix()
    {
      ASSERT(!lMatProjection.empty());
      lMatProjection.pop_back();
    }

    void cContext::SetProjectionMatrix(const math::cMat4& matrix)
    {
      // If we already have a matrix then get a copy of it
      if (!lMatProjection.empty()) {
        math::cMat4& current = lMatProjection.back();
        current = matrix;
      }
    }

    void cContext::MultiplyProjectionMatrix(const math::cMat4& matrix)
    {
      ASSERT(!lMatProjection.empty());
      math::cMat4& current = lMatProjection.back();
      current *= matrix;
    }


    void cContext::PushModelViewMatrix()
    {
      math::cMat4 matrix;

      // If we already have a matrix then get a copy of it
      if (!lMatModelView.empty()) matrix = lMatModelView.back();

      lMatModelView.push_back(matrix);
    }

    void cContext::PopModelViewMatrix()
    {
      ASSERT(!lMatModelView.empty());
      lMatModelView.pop_back();
    }

    void cContext::SetModelViewMatrix(const math::cMat4& matrix)
    {
      // If we already have a matrix then get a copy of it
      if (!lMatModelView.empty()) {
        math::cMat4& current = lMatModelView.back();
        current = matrix;
      }
    }

    void cContext::MultiplyModelViewMatrix(const math::cMat4& matrix)
    {
      ASSERT(!lMatModelView.empty());
      math::cMat4& current = lMatModelView.back();
      current *= matrix;
    }



    void cContext::PushTextureMatrix()
    {
      math::cMat4 matrix;

      // If we already have a matrix then get a copy of it
      if (!lMatTexture.empty()) matrix = lMatTexture.back();

      lMatTexture.push_back(matrix);
    }

    void cContext::PopTextureMatrix()
    {
      ASSERT(!lMatTexture.empty());
      lMatTexture.pop_back();
    }

    void cContext::SetTextureMatrix(const math::cMat4& matrix)
    {
      // If we already have a matrix then get a copy of it
      if (!lMatTexture.empty()) {
        math::cMat4& current = lMatTexture.back();
        current = matrix;
      }
    }

    void cContext::MultiplyTextureMatrix(const math::cMat4& matrix)
    {
      ASSERT(!lMatTexture.empty());
      math::cMat4& current = lMatTexture.back();
      current *= matrix;
    }


    void cContext::ApplyMatrices()
    {
      math::cMat4 matProjection;
      if (!lMatProjection.empty()) matProjection = lMatProjection.back();

      math::cMat4 matModelView;
      if (!lMatModelView.empty()) matModelView = lMatModelView.back();

      math::cMat4 matTexture;
      if (!lMatTexture.empty()) matTexture = lMatTexture.back();

      glMatrixMode(GL_PROJECTION);
      glLoadMatrixf(matProjection.GetOpenGLMatrixPointer());

      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixf(matModelView.GetOpenGLMatrixPointer());

      glMatrixMode(GL_TEXTURE);
      glLoadMatrixf(matTexture.GetOpenGLMatrixPointer());

      // Under OpenGL 3.x we should use this method (We can probably do this under OpenGL 2.x too if we change the shaders)
      //glUniformMatrix4fv("projMat", 1, GL_FALSE, matProjection.GetOpenGLMatrixPointer());
      //glUniformMatrix4fv("???", 1, GL_FALSE, matModelView.GetOpenGLMatrixPointer());
      //glUniformMatrix4fv("???", 1, GL_FALSE, matTexture.GetOpenGLMatrixPointer());
    }







    ApplyTexture::ApplyTexture(cTextureRef pCurrent)
    {
      pLast = pContext->GetCurrentTexture0();
      pContext->SetTexture0(pCurrent);
    }

    ApplyTexture::~ApplyTexture()
    {
      pContext->SetTexture0(pLast);
    }


    ApplyMaterial::ApplyMaterial(material::cMaterialRef _pMaterial)
    {
      //pLast = pContext->GetCurrentMaterial();
      pMaterial = _pMaterial;
      pContext->ApplyMaterial(pMaterial);
    }

    ApplyMaterial::~ApplyMaterial()
    {
      pContext->UnApplyMaterial(pMaterial);
    }





    cRenderToScreen::cRenderToScreen()
    {
      pContext->BeginRenderToScreen();
    }

    cRenderToScreen::~cRenderToScreen()
    {
      pContext->EndRenderToScreen();
    }



    cRenderToTexture::cRenderToTexture(cTextureFrameBufferObjectRef _pTexture) :
      pTexture(_pTexture)
    {
      pContext->BeginRenderToTexture(pTexture);
    }

    cRenderToTexture::~cRenderToTexture()
    {
      pContext->EndRenderToTexture(pTexture);
    }



    cRenderToCubeMapTexture::cRenderToCubeMapTexture(cTextureFrameBufferObjectRef _pTexture, CUBE_MAP_FACE face) :
      pTexture(_pTexture)
    {
      pContext->BeginRenderToCubeMapTextureFace(pTexture, face);
    }

    cRenderToCubeMapTexture::~cRenderToCubeMapTexture()
    {
      pContext->EndRenderToCubeMapTextureFace(pTexture);
    }



    cRenderScreenSpace::cRenderScreenSpace()
    {
      pContext->BeginScreenSpaceRendering();
    }

    cRenderScreenSpace::~cRenderScreenSpace()
    {
      pContext->EndScreenSpaceRendering();
    }
  }
}
