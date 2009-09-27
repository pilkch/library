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

// Boost headers
#include <boost/shared_ptr.hpp>

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
#include <breathe/render/cRender.h>
#include <breathe/render/cVertexBufferObject.h>

#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

#include <breathe/game/cLevel.h>

breathe::cVar fDetailScale = 0.5f;
const float fMaximumViewDistance = 3000.0f;

breathe::render::cRender* pRender = nullptr;

namespace breathe
{
  namespace render
  {
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

    // *** cRender

    cRender::cRender() :

#ifdef BUILD_DEBUG
      bFullscreen(false),
#else
      bFullscreen(true),
#endif

      uiWidth(1024),
      uiHeight(768),
      uiDepth(32),

      uiTriangles(0),

      bIsCubemappingSupported(false),
      bIsShaderSupported(false),
      bIsRenderingToFrameBufferObjectSupported(false),
      bIsFSAASupported(false),

      bIsRenderWireframe(false),
      bIsRenderGui(true),
      bIsLightingEnabled(true),
      bIsCubemappingEnabled(false),
      bIsRenderWithShadersEnabled(false),
      bIsFSAAEnabled(false),

      uiFSAASampleLevel(8),


      clearColour(1.0f, 0.0f, 1.0f, 1.0f),
      bIsActiveColour(false),

      g_info(nullptr),
      videoInfo(nullptr)
    {
      pRender = this;

      uiFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;

      iMaxTextureSize=0;

      sunPosition.Set(10.0f, 10.0f, 5.0f, 0.0f);

      for (size_t i = 0; i < nAtlas; i++) {
        cTextureAtlasRef pNewTextureAtlas(new cTextureAtlas(i));
        vTextureAtlas.push_back(pNewTextureAtlas);
      }

      for (size_t i = 0; i < material::nLayers; i++) {
        material::cLayer layer;
        vLayer.push_back(layer);
      }
    }

    cRender::~cRender()
    {
      //TODO: Delete materials and shader objects and atlases etc.

      for (size_t i = 0; i < nAtlas; i++) vTextureAtlas[i].reset();
      vTextureAtlas.clear();

      vLayer.clear();
    }

    string_t cRender::GetErrorString(GLenum error) const
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

    string_t cRender::GetErrorString() const
    {
      return GetErrorString(glGetError());
    }

    bool cRender::FindExtension(const string_t& sExt) const
    {
      std::ostringstream t;
      t<<const_cast<const unsigned char*>(glGetString(GL_EXTENSIONS));

      return (breathe::string::ToString_t(t.str()).find(sExt) != string_t::npos);
    }

    float cRender::GetShaderVersion() const
    {
      float fGLVersion = 0.0f;
      {
        std::string temp((const char*)glGetString(GL_VERSION));

        fGLVersion = breathe::string::ToFloat(breathe::string::ToString_t(temp));
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

    bool cRender::PreInit()
    {
      LOG<<"cRender::PreInit"<<std::endl;

      // Fetch the video info
      videoInfo = SDL_GetVideoInfo();

      if (videoInfo == nullptr) {
        LOG.Error("SDL", std::string("Video query failed: ") + SDL_GetError());
        return breathe::BAD;
      }

      if (bFullscreen) {
        LOG.Success("App", "Going to fullscreen");
        uiFlags |= SDL_FULLSCREEN;
      } else {
        LOG.Success("App", "Going to windowed");
        uiFlags &= ~SDL_FULLSCREEN;
      }


      // This checks to see if surfaces can be stored in memory
      if (videoInfo->hw_available ) {
        uiFlags |= SDL_HWSURFACE;
        uiFlags &= ~SDL_SWSURFACE;
      } else {
        LOG.Error("SDL", "SOFTWARE SURFACE");
        uiFlags |= SDL_SWSURFACE;
        uiFlags &= ~SDL_HWSURFACE;
      }

      // This checks if hardware blits can be done
      if (videoInfo->blit_hw) uiFlags |= SDL_HWACCEL;
      else {
        LOG.Error("SDL", "SOFTWARE BLIT");
        uiFlags &= ~SDL_HWACCEL;
      }

      // Sets up OpenGL double buffering
      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

      // We definitely want the OpenGL flag for SDL_SetVideoMode
      ASSERT(uiFlags & SDL_OPENGL);



      bIsFSAASupported = (GL_ARB_multisample != 0);
      if (bIsFSAASupported) {
        // Only sample at 2, 4, 8 or 16
        if (uiFSAASampleLevel > 16) uiFSAASampleLevel = 16;
        else if (uiFSAASampleLevel > 8) uiFSAASampleLevel = 8;
        else if (uiFSAASampleLevel > 4) uiFSAASampleLevel = 4;
        else uiFSAASampleLevel = 2;

        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, uiFSAASampleLevel);
      }






      // Create an SDL surface
      LOG<<"cRender::PreInit Calling SDL_SetVideoMode"<<std::endl;
      pSurface = SDL_SetVideoMode(uiWidth, uiHeight, uiDepth, uiFlags);

      // Verify there is a surface
      if (pSurface == nullptr) {
        LOG.Error("SDL", std::string("Video mode set failed: ") + SDL_GetError());
        return breathe::BAD;
      }





      bIsFSAAEnabled = false;

      if (bIsFSAASupported) {
        if (!IsMultiSampling()) {
            bIsFSAAEnabled = false;
        } else {
            size_t uiActualSampleLevel = GetMultiSampleLevel();
            if (uiFSAASampleLevel != uiActualSampleLevel) {
              LOG<<"cRender::PreInit Requested sample level is "<<uiFSAASampleLevel<<", actual sample level is "<<uiActualSampleLevel<<std::endl;
            }

            if ((uiActualSampleLevel == 2) || (uiActualSampleLevel == 4) || (uiActualSampleLevel == 8) || (uiActualSampleLevel == 16)) bIsFSAAEnabled = true;
        }
      }

      return breathe::GOOD;
    }

















    bool cRender::IsMultiSampling() const
    {
      GLint iBuffers = 0;
      glGetIntegerv(GL_SAMPLE_BUFFERS_ARB, &iBuffers);

      return (iBuffers != 0);
    }

    size_t cRender::GetMultiSampleLevel() const
    {
      ASSERT(IsMultiSampling());

      GLint iSampleLevel = 0;
      glGetIntegerv(GL_SAMPLES_ARB, &iSampleLevel);

      if (iSampleLevel < 0) iSampleLevel = 0;

      return size_t(iSampleLevel);
    }

    bool cRender::Init()
    {
      LOG<<"cRender::Init"<<std::endl;

      std::ostringstream t;
      t << "Screen BPP: ";
      t << (unsigned int)(pSurface->format->BitsPerPixel);
      LOG.Success("Render", t.str());
      LOG.Success("Render", std::string("Vendor     : ") + (char*)(glGetString( GL_VENDOR )));
      LOG.Success("Render", std::string("Renderer   : ") + (char*)(glGetString( GL_RENDERER )));
      LOG.Success("Render", std::string("Version    : ") + (char*)(glGetString( GL_VERSION )));
      LOG.Success("Render", std::string("Extensions : ") + (char*)(glGetString( GL_EXTENSIONS )));

      GLint iValue = 0;
      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &iValue);
      ASSERT(iValue >= 0);
      iMaxTextureSize = iValue;

      t.str("");
      t << iMaxTextureSize;
      if (iMaxTextureSize >= MAX_TEXTURE_SIZE) {
        LOG.Success("Render", std::string("Max Texture Size : ") + t.str());
        iMaxTextureSize=MAX_TEXTURE_SIZE;
      } else LOG.Error("Render", std::string("Max Texture Size : ") + t.str());

      {
        CONSOLE<<"WIDESCREEN"<<std::endl;
        SETTINGS::resolution::iterator iter;
        iter.GetWidescreenResolutions();
        while(iter)
        {
          CONSOLE<<iter.GetWidth()<<"x"<<iter.GetHeight()<<std::endl;
          iter++;
        };

        CONSOLE<<"STANDARD"<<std::endl;
        iter.GetStandardResolutions();
        while(iter)
        {
          CONSOLE<<iter.GetWidth()<<"x"<<iter.GetHeight()<<std::endl;
          iter++;
        };
      }


      if (FindExtension(TEXT("GL_ARB_multitexture"))) LOG.Success("Render", "Found GL_ARB_multitexture");
      else {
        LOG.Error("Render", "Not Found GL_ARB_multitexture");
        return false;
      }


      // Cube Map Support
      if (FindExtension(TEXT("GL_ARB_texture_cube_map"))) {
        LOG.Success("Render", "Found GL_ARB_texture_cube_map");
        bIsCubemappingSupported = true;
      } else {
        LOG.Error("Render", "Not Found GL_ARB_texture_cube_map");
        return false;
      }


      // GLSL Version
      float fShaderVersion = GetShaderVersion();
      {
        std::ostringstream stm;
        stm<<fShaderVersion;

        if (fShaderVersion >= 1.0f) {
          LOG.Success("Render", "Found Shader" + stm.str());
          bIsShaderSupported = true;
        } else {
          LOG.Error("Render", "Not Found Shader1.1, version found is Shader" + stm.str());
          bIsShaderSupported = false;
        }
      }

      if (bIsShaderSupported) LOG.Success("Render", "Can use shaders, shaders turned on");
      else LOG.Success("Render", "Cannot use shaders, shaders turned off");


      // Frame Buffer Object Support
      if (FindExtension(TEXT("GL_EXT_framebuffer_object"))) {
        LOG.Success("Render", "Found GL_EXT_framebuffer_object");
        bIsRenderingToFrameBufferObjectSupported = true;
      }
      else LOG.Error("Render", "Not Found GL_EXT_framebuffer_object");



      glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a); // Clear the background to our requested colour
      glClearDepth(1.0);                  // Enables Clearing Of The Depth Buffer
      glEnable(GL_DEPTH_TEST);              // Enable Depth Testing
      glDepthFunc(GL_LEQUAL);

      glCullFace( GL_BACK );
      glFrontFace( GL_CCW );
      glEnable( GL_CULL_FACE );

      glEnable( GL_TEXTURE_2D );
      glShadeModel( GL_SMOOTH );
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


      if (!GLeeInit()) {
        LOG.Error("cRender::Init", "GLeeInit Failed");
        LOG<<GLeeGetErrorString()<<std::endl;
      } else {
        LOG.Success("cRender::Init", "GLeeInit Succeeded");
        LOG<<GLeeGetErrorString()<<std::endl;
      }

      return breathe::GOOD;
    }

    void cRender::Destroy()
    {
      ASSERT(pSurface != nullptr);

      SDL_FreeSurface(pSurface);
      pSurface = nullptr;
    }

    void cRender::ToggleFullscreen()
    {
      bFullscreen = !bFullscreen;

      if (bFullscreen) {
#ifdef BUILD_DEBUG
        uiWidth = 1024;
        uiHeight = 768;
        return;
#endif

        if (uiWidth<1280) {
          if (uiWidth<1024) {
            if (uiWidth<800) {
              uiWidth = 640;
              uiHeight = 480;
            } else {
              uiWidth = 800;
              uiHeight = 600;
            }
          }
          else {
            uiWidth = 1024;
            uiHeight = 768;
          }
        } else {
          uiWidth = 1600;
          uiHeight = 1280;
        }
      }
    }

    void cRender::SetPerspective()
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

    void cRender::EnableWireframe()
    {
      glDisable(GL_CULL_FACE);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void cRender::DisableWireframe()
    {
      glEnable(GL_CULL_FACE);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void cRender::_BeginRenderShared()
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

    void cRender::_EndRenderShared()
    {
      if (bIsFSAAEnabled) glDisable(GL_MULTISAMPLE_ARB);
    }

    void cRender::BeginRenderToTexture(cTextureFrameBufferObjectRef pTexture)
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

    void cRender::EndRenderToTexture(cTextureFrameBufferObjectRef pTexture)
    {
      _EndRenderShared();

      // Restore old view port settings and set rendering back to default frame buffer
      glPopAttrib();
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

      pTexture->GenerateMipMapsIfRequired();

      glDisable(GL_TEXTURE_2D);
    }

    void cRender::BeginRenderToCubeMapTextureFace(cTextureFrameBufferObjectRef pTexture, CUBE_MAP_FACE face)
    {
      ASSERT(pTexture->IsValid());
      ASSERT(pTexture->IsModeCubeMap());

      glEnable(GL_TEXTURE_2D);

      // First we bind the FBO so we can render to it
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, pTexture->uiFBO);


      GLenum openGLFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
      if (face == CUBE_MAP_FACE::CUBE_MAP_FACE_NEGATIVE_X) openGLFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
      else if (face == CUBE_MAP_FACE::CUBE_MAP_FACE_POSITIVE_Y) openGLFace = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
      else if (face == CUBE_MAP_FACE::CUBE_MAP_FACE_NEGATIVE_Y) openGLFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
      else if (face == CUBE_MAP_FACE::CUBE_MAP_FACE_POSITIVE_Z) openGLFace = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
      else if (face == CUBE_MAP_FACE::CUBE_MAP_FACE_NEGATIVE_Z) openGLFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;

      // Bind the actual face we want to render to
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, openGLFace, pTexture->uiTexture, 0);


      // Save the view port settings and set it to the size of the texture
      glPushAttrib(GL_VIEWPORT_BIT);
      glViewport(0, 0, pTexture->GetWidth(), pTexture->GetHeight());

      _BeginRenderShared();
    }

    void cRender::EndRenderToCubeMapTextureFace(cTextureFrameBufferObjectRef pTexture)
    {
      EndRenderToTexture(pTexture);
    }

    void cRender::_BeginRenderToScreen()
    {
      // Set viewport
      glViewport(0, 0, uiWidth, uiHeight);

      _BeginRenderShared();

      uiTextureModeChanges = uiTextureChanges = uiTriangles = 0;
    }

    void cRender::_EndRenderToScreen()
    {
      _EndRenderShared();

      SDL_GL_SwapBuffers();
    }

    void cRender::BeginRenderToScreen()
    {
      _BeginRenderToScreen();
    }

    void cRender::EndRenderToScreen()
    {
      _EndRenderToScreen();
    }

    void cRender::_RenderPostRenderPass(material::cMaterialRef pMaterial, cTextureFrameBufferObjectRef pTexture)
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

    void cRender::SaveScreenShot()
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

    // Our screen coordinates look like this
    // 0.0f, 0.0f            1.0f, 0.0f
    //
    //
    // 0.0f, 1.0f            1.0f, 1.0f

    void cRender::RenderScreenSpacePolygon(float fX, float fY,
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
    void cRender::RenderScreenSpaceSolidRectangleWithBorderTopLeftIsAt(float fX, float fY, float fWidth, float fHeight, const math::cColour& boxColour, const math::cColour& upperBorderColour, const math::cColour& lowerBorderColour)
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

    void cRender::RenderScreenSpaceGradientFilledRectangleTopLeftIsAt(float fX, float fY, float fWidth, float fHeight, const math::cColour& colour0, const math::cColour& colour1, const math::cColour& colour2, const math::cColour& colour3)
    {
      glBegin(GL_QUADS);
        glColor3f(colour0.r, colour0.g, colour0.b); glVertex2f(fX, fY);
        glColor3f(colour1.r, colour1.g, colour1.b); glVertex2f(fX + fWidth, fY);
        glColor3f(colour2.r, colour2.g, colour2.b); glVertex2f(fX + fWidth, fY + fHeight);
        glColor3f(colour3.r, colour3.g, colour3.b); glVertex2f(fX, fY + fHeight);
      glEnd();
    }

    void cRender::RenderScreenSpaceRectangle(float fX, float fY, float fWidth, float fHeight)
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

    void cRender::RenderScreenSpaceRectangle(
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

    void cRender::RenderScreenSpaceRectangleTopLeftIsAt(float fX, float fY, float fWidth, float fHeight)
    {
      glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(fX, fY);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(fX + fWidth, fY);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(fX + fWidth, fY + fHeight);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(fX, fY + fHeight);
      glEnd();
    }

    void cRender::RenderScreenSpaceRectangleTopLeftIsAt(
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

    void cRender::RenderScreenSpaceRectangleRotated(float fX, float fY, float fWidth, float fHeight, float fRotation)
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

    void cRender::BeginScreenSpaceRendering()
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

    void cRender::EndScreenSpaceRendering()
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

    void cRender::BeginScreenSpaceGuiRendering()
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

    void cRender::EndScreenSpaceGuiRendering()
    {
          glMatrixMode( GL_MODELVIEW );
        glPopMatrix();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();

      glFrontFace(GL_CCW);
    }

    // In this mode y is 1..0
    void cRender::BeginScreenSpaceWorldRendering(float fScale)
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

    void cRender::EndScreenSpaceWorldRendering()
    {
          glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();

      glFrontFace(GL_CW);
    }

    void cRender::PushScreenSpacePosition(float x, float y)
    {
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
        //glLoadIdentity();
        glTranslatef(x, -y, 0.0f);
    }

    void cRender::PopScreenSpacePosition()
    {
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
    }


    void cRender::RenderMesh(model::cMeshRef pMesh)
    {
      ASSERT(false);
    }

    void cRender::RenderArrow(const math::cVec3& from, const math::cVec3& to, const math::cColour& colour)
    {
      SetColour(colour);
      glBegin(GL_LINES);
        glVertex3f(from.x, from.y, from.z);
        glVertex3f(to.x, to.y, to.z);
      glEnd();
    }

    void cRender::RenderAxisReference(float x, float y, float z)
    {
      math::cVec3 position(x, y, z);
      RenderAxisReference(position);
    }

    void cRender::RenderAxisReference(const math::cVec3& position)
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

    void cRender::RenderTriangle(const math::cVec3& v0, const math::cVec3& v1, const math::cVec3& v2)
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

    void cRender::RenderBoxTextured(const math::cVec3& vMin, const math::cVec3& vMax)
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

    void cRender::RenderBox(const math::cVec3& vMin, const math::cVec3& vMax)
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

    void cRender::RenderWireframeBox(const math::cVec3& vMin, const math::cVec3& vMax)
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

    void cRender::SetAtlasWidth(unsigned int uiNewSegmentWidthPX, unsigned int uiNewSegmentSmallPX, unsigned int uiNewAtlasWidthPX)
    {
      uiSegmentWidthPX=uiNewSegmentWidthPX;
      uiAtlasWidthPX=uiNewAtlasWidthPX;
      uiSegmentSmallPX=uiNewSegmentSmallPX;
    }

    void cRender::BeginLoadingTextures()
    {
      for (size_t i = 0; i < nAtlas; i++) vTextureAtlas[i]->Begin(uiSegmentWidthPX, uiSegmentSmallPX, uiAtlasWidthPX);

      if (pTextureNotFoundTexture == nullptr) AddTextureNotFoundTexture(TEXT("textures/texturenotfound.png"));
      if (pMaterialNotFoundMaterial == nullptr) AddMaterialNotFoundMaterial(TEXT("textures/materialnotfound.png"));
    }

    void cRender::EndLoadingTextures()
    {
      for (size_t i = 0; i < nAtlas; i++) vTextureAtlas[i]->End();
    }



    cTextureRef cRender::AddTextureToAtlas(const string_t& sNewFilename, unsigned int uiAtlas)
    {
      ASSERT(sNewFilename != TEXT(""));
      ASSERT(ATLAS_NONE != uiAtlas);

      string_t sFilename;
      breathe::filesystem::FindFile(breathe::string::ToString_t(sNewFilename), sFilename);

      cTextureRef p = vTextureAtlas[uiAtlas]->AddTexture(sFilename);
      if ((p == nullptr) || (p == pTextureNotFoundTexture)) {
        LOG.Error("Texture", breathe::string::ToUTF8(sFilename) + " pTextureNotFound");
        return pTextureNotFoundTexture;
      }

      std::ostringstream t;
      t << p->uiTexture;
      LOG.Success("Texture", breathe::string::ToUTF8(sFilename) + " " + t.str());

      return p;
    }

    cTextureRef cRender::AddTexture(const string_t& sNewFilename)
    {
      ASSERT(sNewFilename != TEXT(""));

      string_t sFilename;
      breathe::filesystem::FindFile(breathe::string::ToString_t(sNewFilename), sFilename);

      string_t s = breathe::filesystem::GetFile(sFilename);

      cTextureRef p = GetTexture(s);
      if (p == nullptr) {
        ASSERT(p != nullptr);
        return pTextureNotFoundTexture;
      }

      p = cTextureRef(new cTexture);
      if (p->Load(sFilename) != breathe::GOOD) {
        LOG.Error("Render", "Failed to load " + breathe::string::ToUTF8(sFilename));
        p.reset();
        return pTextureNotFoundTexture;
      }

      p->Create();
      p->CopyFromSurfaceToTexture();

      mTexture[s]=p;

      std::ostringstream t;
      t << p->uiTexture;
      LOG.Success("Texture", "Texture " + breathe::string::ToUTF8(s) + " uiTexture=" + breathe::string::ToUTF8(t.str()));
      return p;
    }

    bool cRender::AddTextureNotFoundTexture(const string_t& sNewFilename)
    {
      string_t sFilename;
      breathe::filesystem::FindResourceFile(breathe::string::ToString_t(sNewFilename), sFilename);

      cTextureRef p(new cTexture);
      if (p->Load(sFilename) != breathe::GOOD) {
        // Just assert, don't even try to come back from this situation
        LOG.Error("Render", "Failed to load texture not found texture");
        CONSOLE<<"cRender::AddTextureNotFoundTexture failed to load "<<sNewFilename<<" "<<sFilename<<std::endl;
        ASSERT(false);
        p.reset();
        return breathe::BAD;
      }

      p->Create();
      p->CopyFromSurfaceToTexture();

      mTexture[sNewFilename]=p;
      pTextureNotFoundTexture = p;

      std::ostringstream t;
      t << pTextureNotFoundTexture->uiTexture;
      LOG.Success("Texture", "TextureNotFoundTexture " + t.str());

      return breathe::GOOD;
    }

    bool cRender::AddMaterialNotFoundTexture(const string_t& sNewFilename)
    {
      string_t sFilename;
      breathe::filesystem::FindResourceFile(breathe::string::ToString_t(sNewFilename), sFilename);

      LOG.Success("Texture", "Loading " + breathe::string::ToUTF8(sFilename));

      cTextureRef p(new cTexture);
      if (p->Load(sFilename) != breathe::GOOD) {
        // Just assert, don't even try to come back from this situation
        LOG.Error("Render", "Failed to load material not found texture");
        CONSOLE<<"cRender::AddMaterialNotFoundTexture failed to load "<<sNewFilename<<" "<<sFilename<<std::endl;
        ASSERT(false);
        p.reset();
        return breathe::BAD;
      }

      p->Create();
      p->CopyFromSurfaceToTexture();

      mTexture[sNewFilename]=p;
      pMaterialNotFoundTexture = p;

      std::ostringstream t;
      t << pMaterialNotFoundTexture->uiTexture;
      LOG.Success("Texture", "MaterialNotFoundTexture " + t.str());

      return breathe::GOOD;
    }


    cTextureRef cRender::GetTextureAtlas(ATLAS atlas)
    {
      ASSERT(atlas < nAtlas);

      return vTextureAtlas[atlas];
    }

    cTextureRef cRender::GetTexture(const string_t& sNewFilename)
    {
      std::map<string_t, cTextureRef>::iterator iter = mTexture.find(sNewFilename);
      if (iter != mTexture.end()) return iter->second;

      return pTextureNotFoundTexture;
    }

    cVertexBufferObjectRef cRender::AddVertexBufferObject()
    {
      cVertexBufferObjectRef pVertexBufferObject(new cVertexBufferObject);
      vVertexBufferObject.push_back(pVertexBufferObject);

      return pVertexBufferObject;
    }

    cTextureRef cRender::GetCubeMap(const string_t& sNewFilename)
    {
      if (TEXT("") == sNewFilename) return cTextureRef();

      std::map<string_t, cTextureRef>::iterator iter=mCubeMap.find(sNewFilename);

      if (mCubeMap.end() != iter) return iter->second;

      return cTextureRef();
    }

    cTextureRef cRender::AddCubeMap(const string_t& sFilename)
    {
      /*TODO: Surface of 1x6 that holds the cubemap faces,
      not actually used for rendering, just collecting each surface

      class cCubeMap : protected cTexture
      {
      public:
        CopyFromSurface(uiWidth, 6 * uiHeight);
      };
      */

      if (TEXT("")==sFilename)
        return pTextureNotFoundTexture;

      cTextureRef p = mCubeMap[sFilename];
      if (p) return p;

      p.reset(new cTexture);

      mCubeMap[sFilename] = p;

      p->sFilename = sFilename;

      GLuint cube_map_directions[6] =
      {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
      };

      LOG.Success("Texture", "Loading CubeMap " + breathe::string::ToUTF8(sFilename));

      unsigned int i=0;
      unsigned int uiTempTexture=0;

      glEnable(GL_TEXTURE_CUBE_MAP);

      glGenTextures(1, &p->uiTexture);

      glBindTexture(GL_TEXTURE_CUBE_MAP, p->uiTexture);

      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


      std::stringstream s;

      string_t sFile = breathe::filesystem::GetFileNoExtension(breathe::string::ToString_t(sFilename));
      string_t sExt = breathe::filesystem::GetExtension(breathe::string::ToString_t(sFilename));

      for (i=0;i<6;i++)
      {
        s.str("");

        s<<breathe::string::ToUTF8(sFile)<<"/"<<breathe::string::ToUTF8(sFile)<<i<<"."<<breathe::string::ToUTF8(sExt);
        string_t sFilename;
        breathe::filesystem::FindResourceFile(breathe::string::ToString_t(s.str()), sFilename);

        unsigned int mode=0;

        SDL_Surface* surface = IMG_Load(breathe::string::ToUTF8(sFilename).c_str());

        // could not load filename
        if (!surface)
        {
          LOG.Error("Texture", "Couldn't Load Texture " + breathe::string::ToUTF8(sFilename));
          return cTextureRef();
        }

        if (surface->format->BytesPerPixel == 3) // RGB 24bit
        {
          mode = GL_RGB;
          LOG.Success("Texture", "RGB Image");
        }
        else if (surface->format->BytesPerPixel == 4)// RGBA 32bit
        {
          mode = GL_RGBA;
          LOG.Success("Texture", "RGBA Image");
        }
        else
        {
          SDL_FreeSurface(surface);
          std::ostringstream t;
          t << surface->format->BytesPerPixel;
          LOG.Error("Texture", "Error Unknown Image Format (" + t.str() + ")");

          return cTextureRef();
        }

        {
          int nHH = surface->h / 2;
          int nPitch = surface->pitch;

          unsigned char* pBuf = new unsigned char[nPitch];
          unsigned char* pSrc = (unsigned char*) surface->pixels;
          unsigned char* pDst = (unsigned char*) surface->pixels + nPitch*(surface->h - 1);

          while (nHH--)
          {
            std::memcpy(pBuf, pSrc, nPitch);
            std::memcpy(pSrc, pDst, nPitch);
            std::memcpy(pDst, pBuf, nPitch);

            pSrc += nPitch;
            pDst -= nPitch;
          };

          SAFE_DELETE_ARRAY(pBuf);
        }




        // create one texture name
        glGenTextures(1, &uiTempTexture);

        // This sets the alignment requirements for the start of each pixel row in memory.
        glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

        // Bind the texture to the texture arrays index and init the texture
        glBindTexture(GL_TEXTURE_2D, uiTempTexture);

        // this reads from the sdl surface and puts it into an opengl texture
        glTexImage2D(cube_map_directions[i], 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

        // these affect how this texture is drawn later on...
        /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //Trilinear mipmapping.
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //build mipmaps
        //gluBuild2DMipmaps(cube_map_directions[i], mode, surface->w, surface->h, mode, GL_UNSIGNED_BYTE, surface->pixels);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);*/

        // clean up
        SDL_FreeSurface(surface);
      }


      glDisable(GL_TEXTURE_CUBE_MAP);
      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_GEN_T);
      glDisable(GL_TEXTURE_GEN_R);
      glEnable(GL_TEXTURE_2D);


      std::ostringstream t;
      t << p->uiTexture;
      LOG.Success("Texture", t.str());

      return p;
    }

    material::cMaterialRef cRender::AddMaterialNotFoundMaterial(const string_t& sNewFilename)
    {
      AddMaterialNotFoundTexture(sNewFilename);

      pMaterialNotFoundMaterial.reset(new material::cMaterial(TEXT("MaterialNotFound")));

      string_t sFilename;
      breathe::filesystem::FindResourceFile(breathe::string::ToString_t(sNewFilename), sFilename);
      pMaterialNotFoundMaterial->vLayer[0]->sTexture = sFilename;
      pMaterialNotFoundMaterial->vLayer[0]->pTexture = pMaterialNotFoundTexture;
      pMaterialNotFoundMaterial->vLayer[0]->uiTextureMode = TEXTURE_MODE::TEXTURE_NORMAL;

      return pMaterialNotFoundMaterial;
    }

    material::cMaterialRef cRender::AddMaterial(const string_t& sNewfilename)
    {
      if (sNewfilename.empty()) return material::cMaterialRef();

      material::cMaterialRef pMaterial = _GetMaterial(sNewfilename);

      if (pMaterial != pMaterialNotFoundMaterial) return pMaterial;

      string_t sFilename;
      filesystem::FindResourceFile(breathe::string::ToString_t(sNewfilename), sFilename);
      pMaterial.reset(new material::cMaterial(sFilename));

      if (breathe::BAD == pMaterial->Load(sFilename)) {
        pMaterial = pMaterialNotFoundMaterial;
      }

      mMaterial[filesystem::GetFile(sFilename)] = pMaterial;

      return pMaterial;
    }

    material::cMaterialRef cRender::AddMaterialAsAlias(const string_t& sNewfilename, const string_t& sAlias)
    {
      LOG<<"cRender::AddMaterialAsAlias sNewfilename=\""<<sNewfilename<<"\""<<std::endl;
      if (sNewfilename.empty()) {
        LOG<<"cRender::AddMaterialAsAlias sNewfilename is empty, returning"<<std::endl;
        return material::cMaterialRef();
      }

      string_t sFilename;
      filesystem::FindResourceFile(breathe::string::ToString_t(sNewfilename), sFilename);

      LOG<<"cRender::AddMaterialAsAlias sFilename=\""<<sFilename<<"\""<<std::endl;

      material::cMaterialRef pMaterial(new material::cMaterial(sFilename));

      if (breathe::BAD == pMaterial->Load(sFilename)) {
        LOG<<"cRender::AddMaterialAsAlias FAILED to load material"<<std::endl;
        pMaterial = pMaterialNotFoundMaterial;
      }

      mMaterial[sAlias] = pMaterial;

      LOG<<"cRender::AddMaterialAsAlias returning"<<std::endl;

      return pMaterial;
    }

    material::cMaterialRef cRender::GetMaterial(const string_t& sFilename)
    {
      AddMaterial(sFilename);
      return _GetMaterial(sFilename);
    }

    material::cMaterialRef cRender::_GetMaterial(const string_t& sFilename)
    {
      std::map<string_t, material::cMaterialRef>::iterator iter = mMaterial.begin();
      std::map<string_t, material::cMaterialRef>::iterator iterEnd = mMaterial.end();

      string_t temp = filesystem::GetFile(sFilename);
      while(iter != iterEnd) {
        if (temp == iter->first) return iter->second;
        iter++;
      }

      return pMaterialNotFoundMaterial;
    }

    float Angle(const math::cVec2 & a, const math::cVec2 & b)
    {
      if (a.x > b.x) {
        if (a.y > b.y) return (atan((a.y-b.y)/(a.x-b.x)) + math::cPI_DIV_180 * 90.0f) * math::c180_DIV_PI;

        return (-atan((a.y-b.y)/(b.x-a.x)) + math::cPI_DIV_180 * 90.0f) * math::c180_DIV_PI;
      }


      if (b.y > a.y) return (atan((b.y-a.y)/(b.x-a.x)) + math::cPI_DIV_180 * 270.0f) * math::c180_DIV_PI;

      return (atan((b.y-a.y)/(b.x-a.x)) + math::cPI_DIV_180 * 270.0f) * math::c180_DIV_PI;
    }


    void cRender::SelectTextureUnit0()
    {
      glActiveTexture(GL_TEXTURE0);
    }

    void cRender::SelectTextureUnit1()
    {
      glActiveTexture(GL_TEXTURE1);
    }

    void cRender::SelectTextureUnit2()
    {
      glActiveTexture(GL_TEXTURE2);
    }

    bool cRender::SetTexture0(ATLAS atlas)
    {
      return SetTexture0(GetTextureAtlas(atlas));
    }

    bool cRender::SetTexture1(ATLAS atlas)
    {
      return SetTexture1(GetTextureAtlas(atlas));
    }

    bool cRender::SetTexture0(cTextureRef pTexture)
    {
      ASSERT(pTexture != nullptr);

      //Activate the correct texture unit
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, pTexture->uiTexture);
      return true;
    }

    bool cRender::SetTexture1(cTextureRef pTexture)
    {
      ASSERT(pTexture != nullptr);

      //Activate the correct texture unit
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, pTexture->uiTexture);
      return true;
    }

    material::cMaterialRef cRender::GetCurrentMaterial() const
    {
      ASSERT(pCurrentMaterial != nullptr);
      return pCurrentMaterial;
    }

    cTextureRef cRender::GetCurrentTexture0() const
    {
      ASSERT(pCurrentMaterial != nullptr);
      ASSERT(pCurrentMaterial->vLayer.size() > 0);
      return pCurrentMaterial->vLayer[0]->pTexture;
    }

    cTextureRef cRender::GetCurrentTexture1() const
    {
      ASSERT(pCurrentMaterial != nullptr);
      ASSERT(pCurrentMaterial->vLayer.size() > 1);
      return pCurrentMaterial->vLayer[1]->pTexture;
    }

    cTextureRef cRender::GetCurrentTexture2() const
    {
      ASSERT(pCurrentMaterial != nullptr);
      ASSERT(pCurrentMaterial->vLayer.size() > 2);
      return pCurrentMaterial->vLayer[2]->pTexture;
    }

    bool cRender::ClearMaterial()
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
        if (TEXTURE_MODE::TEXTURE_MASK == layerOld->uiTextureMode ||
            TEXTURE_MODE::TEXTURE_BLEND == layerOld->uiTextureMode)
        {
          glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
          glBlendFunc(GL_ONE, GL_ZERO);
          glDisable(GL_BLEND);
        }
        else if (  TEXTURE_MODE::TEXTURE_DETAIL==layerOld->uiTextureMode)
        {
          // Reset the texture matrix
          glMatrixMode(GL_TEXTURE);
          glLoadIdentity();
          glMatrixMode(GL_MODELVIEW);

          //glEnable(GL_LIGHTING);
        }
        else if (TEXTURE_MODE::TEXTURE_CUBE_MAP==layerOld->uiTextureMode)
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
        layerOld->uiTextureMode = TEXTURE_MODE::TEXTURE_NONE;
        layerOld->pTexture.reset();
        layerOld->sTexture = TEXT("");
      }

      glActiveTexture(GL_TEXTURE0);
      glEnable(GL_TEXTURE_2D);
      glDisable(GL_LIGHTING);

      if (bIsShaderSupported && (pCurrentShader != nullptr)) UnBindShader();

      pCurrentMaterial.reset();

      ClearColour();

      return true;
    }

    bool cRender::SetShaderConstant(const std::string& sConstant, int value)
    {
      ASSERT(pCurrentShader != nullptr);

      GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
      if (loc == -1) {
        LOG.Error("cRender::SetShaderConstant", breathe::string::ToUTF8(pCurrentShader->sShaderVertex) + ", " + breathe::string::ToUTF8(pCurrentShader->sShaderFragment) + " Couldn't set \"" + sConstant + "\" perhaps the constant is not actually used within the shader");
        ASSERT(loc > 0);
        return false;
      }

      glUniform1i(loc, value);
      return true;
    }

    bool cRender::SetShaderConstant(const std::string& sConstant, float value)
    {
      ASSERT(pCurrentShader != nullptr);

      GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
      if (loc == -1) {
        LOG.Error("cRender::SetShaderConstant", breathe::string::ToUTF8(pCurrentShader->sShaderVertex) + ", " + breathe::string::ToUTF8(pCurrentShader->sShaderFragment) + " Couldn't set \"" + sConstant + "\" perhaps the constant is not actually used within the shader");
        ASSERT(loc > 0);
        return false;
      }

      glUniform1f(loc, value);
      return true;
    }

    bool cRender::SetShaderConstant(const std::string& sConstant, const math::cVec2& value)
    {
      ASSERT(pCurrentShader != nullptr);

      GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
      if (loc == -1) {
        LOG.Error("cRender::SetShaderConstant", breathe::string::ToUTF8(pCurrentShader->sShaderVertex) + ", " + breathe::string::ToUTF8(pCurrentShader->sShaderFragment) + " Couldn't set \"" + sConstant + "\" perhaps the constant is not actually used within the shader");
        ASSERT(loc > 0);
        return false;
      }

      glUniform2f(loc, value.x, value.y);
      return true;
    }

    bool cRender::SetShaderConstant(const std::string& sConstant, const math::cVec3& value)
    {
      ASSERT(pCurrentShader != nullptr);

      GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
      if (loc == -1) {
        LOG.Error("cRender::SetShaderConstant", breathe::string::ToUTF8(pCurrentShader->sShaderVertex) + ", " + breathe::string::ToUTF8(pCurrentShader->sShaderFragment) + " Couldn't set \"" + sConstant + "\" perhaps the constant is not actually used within the shader");
        ASSERT(loc > 0);
        return false;
      }

      glUniform3f(loc, value.x, value.y, value.z);
      return true;
    }

    bool cRender::SetShaderConstant(const std::string& sConstant, const math::cVec4& value)
    {
      ASSERT(pCurrentShader != nullptr);

      GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
      if (loc == -1) {
        LOG.Error("cRender::SetShaderConstant", breathe::string::ToUTF8(pCurrentShader->sShaderVertex) + ", " + breathe::string::ToUTF8(pCurrentShader->sShaderFragment) + " Couldn't set \"" + sConstant + "\" perhaps the constant is not actually used within the shader");
        ASSERT(loc > 0);
        return false;
      }

      glUniform4f(loc, value.x, value.y, value.z, value.w);
      return true;
    }


    void cRender::BindShader(cShaderRef pShader)
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

    void cRender::UnBindShader()
    {
      // TODO: Is this needed?
      //glDisable(GL_LIGHTING);

      glUseProgram(0);

      pCurrentShader.reset();
    }


    bool cRender::ApplyMaterial(material::cMaterialRef pMaterial)
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
          case TEXTURE_MODE::TEXTURE_NONE: {
            glDisable(GL_TEXTURE_2D);
            break;
          }

          case TEXTURE_MODE::TEXTURE_NORMAL:
          case TEXTURE_MODE::TEXTURE_MASK:
          case TEXTURE_MODE::TEXTURE_BLEND: {
            // We now do masking and blending in shaders so this is greatly simplified
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, pLayer->pTexture->uiTexture);
            break;
          }

          case TEXTURE_MODE::TEXTURE_DETAIL: {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, pLayer->pTexture->uiTexture);

            // Change the texture matrix so that we have more detail than normal texture
            glMatrixMode(GL_TEXTURE);
            glPushMatrix();
              glLoadIdentity();
              glMatrixMode(GL_MODELVIEW);

            break;
          }

          case TEXTURE_MODE::TEXTURE_CUBE_MAP: {
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


          case TEXTURE_MODE::TEXTURE_POST_RENDER: {
            glEnable(GL_TEXTURE_2D);
            // TODO: Automatically fill this with a valid texture?
            //glBindTexture(GL_TEXTURE_2D, pLayer->pTexture->uiTexture);
            break;
          }

          default: {
            LOG<<"cRender::ApplyMaterial Unknown texture type"<<std::endl;
            ASSERT(false);
          }
        }
      }

      if (bIsShaderSupported && (pMaterial->pShader != nullptr)) {
        BindShader(pMaterial->pShader);
      }

      pCurrentMaterial = pMaterial;

      // Set the activate texture unit to the first one as that is what everyone is expecting
      glActiveTexture(GL_TEXTURE0_ARB);

      return true;
    }

    bool cRender::UnApplyMaterial(material::cMaterialRef pMaterial)
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
          case TEXTURE_MODE::TEXTURE_NONE: {
            glDisable(GL_TEXTURE_2D);
            break;
          }

          case TEXTURE_MODE::TEXTURE_NORMAL:
          case TEXTURE_MODE::TEXTURE_MASK:
          case TEXTURE_MODE::TEXTURE_BLEND: {
            // We now do masking and blending in shaders so this is greatly simplified
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            break;
          }

          case TEXTURE_MODE::TEXTURE_DETAIL: {
              glDisable(GL_TEXTURE_2D);
              glBindTexture(GL_TEXTURE_2D, 0);

              // Change the texture matrix so that we have more detail than normal texture
              glMatrixMode(GL_TEXTURE);
              glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
          }

          case TEXTURE_MODE::TEXTURE_CUBE_MAP: {
            if (!bIsCubemappingSupported) {
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


          case TEXTURE_MODE::TEXTURE_POST_RENDER: {
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            break;
          }

          default: {
            LOG<<"cRender::UnApplyMaterial Unknown texture type"<<std::endl;
            ASSERT(false);
          }
        }
      }

      // UnApply shader
      if ((pMaterial->pShader != nullptr) && bIsShaderSupported) {
        UnBindShader();
      }


      // Set the activate texture unit to the first one as that is what everyone is expectingpMaterial
      glActiveTexture(GL_TEXTURE0_ARB);

      return false;
    }



    void cRender::ClearColour()
    {
      bIsActiveColour = false;

      colour.SetBlack();
      colour.a = 1.0f;

      glColor4f(colour.r, colour.g, colour.b, colour.a);
    }

    void cRender::SetColour(float r, float g, float b)
    {
      math::cColour colour(r, g, b, 1.0f);
      SetColour(colour);
    }

    void cRender::SetColour(const math::cColour& inColour)
    {
      bIsActiveColour = true;

      colour = inColour;

      glColor4f(colour.r, colour.g, colour.b, colour.a);
    }


    void cRender::TransformModels()
    {
      /*cTextureRef t;
      material::cMaterialRef mat;

      model::cStaticRef s;
      model::cMeshRef pMesh;
      float* fTextureCoords=NULL;
      size_t nMeshes=0;
      size_t nTexcoords=0;
      unsigned int mesh=0;
      unsigned int texcoord=0;

      //Transform uv texture coordinates
      std::map<string_t, model::cStaticRef>::iterator iter = mStatic.begin();
      std::map<string_t, model::cStaticRef>::iterator iterEnd = mStatic.end();
      while (iter != iterEnd) {
        string_t sFilename = iter->first;
        s = iter->second;

        LOG.Success("cRender::TransformModels", "UV model=" + breathe::string::ToUTF8(sFilename));
        ASSERT(s != nullptr);

        nMeshes = s->vMesh.size();

        std::ostringstream sOut;
        sOut<<static_cast<unsigned int>(nMeshes);
        LOG.Success("cRender::TransformModels", "UV model=" + breathe::string::ToUTF8(sFilename) + " meshes=" + sOut.str());

        for (mesh = 0; mesh < nMeshes; mesh++) {
          pMesh = s->vMesh[mesh];
          fTextureCoords = &pMesh->pMeshData->vTextureCoord[0];
          nTexcoords = pMesh->pMeshData->vTextureCoord.size();

          mat = GetMaterial(pMesh->sMaterial);

          if (mat != nullptr) {
            if (!mat->vLayer.empty()) {
              t = mat->vLayer[0]->pTexture;

              if (t == nullptr) t = GetTexture(mat->vLayer[0]->sTexture);

              if (t != nullptr) {
                for (texcoord=0;texcoord<nTexcoords;texcoord+=2) t->Transform(fTextureCoords[texcoord], fTextureCoords[texcoord+1]);
              } else LOG.Error("Transform", "Texture not found " + breathe::string::ToUTF8(mat->vLayer[0]->sTexture));
            } else LOG.Error("Transform", "Material doesn't have any layers");
          } else LOG.Error("Transform", "Material not found " + breathe::string::ToUTF8(pMesh->sMaterial));
        }

        iter++;
      }


      float* fNormals=NULL;

      //Calculate normals
      for (iter=mStatic.begin();iter!=mStatic.end();iter++)
      {
        LOG.Success("Transform", "Normals " + breathe::string::ToUTF8(iter->first));

        s=iter->second;

        if (s)
        {
          nMeshes=s->vMesh.size();

          for (mesh=0;mesh<nMeshes;mesh++)
          {
            pMesh = s->vMesh[mesh];
            fNormals = &pMesh->pMeshData->vNormal[0];

            //Init all vertex normals to zero

            //for all faces:
            //  compute face normal

            //for every vertex in every face:
            //  add face normal to vertex normal
            //  for all adjacent faces:
            //      if the dotproduct of the face normal and the adjacentface normal is > 0.71:
            //          add adjacentface normal to vertex normal

            //for all vertex normals:
            //                                                                    normalise vertex normal
          }
        }
      }


      // TODO: Optimise order for rendering
      unsigned int uiPass=0;
      unsigned int i=0;
      unsigned int uiMode0=0;
      //unsigned int uiMode1=0;

      for (iter = mStatic.begin(); iter != mStatic.end(); iter++) {
        LOG.Success("Transform", "Optimising " + breathe::string::ToUTF8(iter->first));

        s = iter->second;

        if (s != nullptr) {
          nMeshes = s->vMesh.size();

          for (uiPass = 1; uiPass < nMeshes; uiPass++) {
            for (i = 0; i < nMeshes - uiPass; i++) {
              uiMode0=GetMaterial(s->vMesh[i]->sMaterial)->vLayer[0]->uiTextureMode;

              //x[i] > x[i+1]
              if ((TEXTURE_MASK == uiMode0) || (TEXTURE_BLEND == uiMode0))
                std::swap(s->vMesh[i], s->vMesh[i+1]);
            }
          }
        }
      }

      LOG.Success("Render", "TransformModels returning");*/
    }

    void cRender::ReloadTextures()
    {
      LOG.Success("Render", "ReloadTextures");

      {
        LOG.Success("Render", "ReloadTextures Atlases");
        cTextureAtlasRef pAtlas;
        size_t n = vTextureAtlas.size();
        for (size_t i = 0;i<n;i++)
          vTextureAtlas[i]->Reload();
      }

      {
        LOG.Success("Render", "ReloadTextures Misc Textures");
        cTextureRef pTexture;
        std::map<string_t, cTextureRef>::iterator iter=mTexture.begin();
        std::map<string_t, cTextureRef>::iterator iterEnd=mTexture.end();
        while(iter != iterEnd)
        {
          pTexture = iter->second;

          pTexture->Reload();

          iter++;
        }
      }

      {
        LOG.Success("Render", "ReloadTextures Materials");
        material::cMaterialRef pMaterial;
        std::map<string_t, material::cMaterialRef>::iterator iter=mMaterial.begin();
        const std::map<string_t, material::cMaterialRef>::iterator iterEnd=mMaterial.end();
        while (iter != iterEnd) {
          pMaterial = iter->second;

          if (pMaterial->pShader != nullptr) {
            pMaterial->pShader->Destroy();
            pMaterial->pShader->Init();
          }

          iter++;
        }
      }

      {
        LOG.Success("Render", "ReloadTextures Vertex Buffer Objects");
        const size_t n = vVertexBufferObject.size();
        for (size_t i = 0; i < n; i++) {
          vVertexBufferObject[i]->Destroy();
          vVertexBufferObject[i]->Compile();
        }
      }
    }

    namespace SETTINGS
    {
      bool ResolutionCompare(const resolution& lhs, const resolution& rhs)
      {
        return (lhs.GetHeight() > rhs.GetHeight() && lhs.GetWidth() > rhs.GetWidth());
      }

      resolution::iterator::iterator()
      {
        iter = resolutions.end();
      }


      const float fWideScreenRatio = 1.33333333333333333333333f;

      void resolution::iterator::GetResolutions(bool onlyWidescreen)
      {
        resolutions.clear();
        iter = resolutions.end();

        SDL_Rect** modes = SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_HWSURFACE);
        if (modes == (SDL_Rect**)0 || modes == (SDL_Rect**)-1)
        {
          CONSOLE<<"No modes available"<<std::endl;
          resolutions.push_back(resolution(640, 480));
        }
        else if (onlyWidescreen)
        {
          for (int i=0;modes[i];++i)
          {
            float ratio = static_cast<float>(modes[i]->w)/static_cast<float>(modes[i]->h);
            if (ratio > fWideScreenRatio)
              resolutions.push_back(resolution(modes[i]->w, modes[i]->h));
          }
        }
        else
        {
          for (int i=0;modes[i];++i)
          {
            float ratio = static_cast<float>(modes[i]->w)/static_cast<float>(modes[i]->h);
            if (ratio < fWideScreenRatio + math::cEPSILON)
              resolutions.push_back(resolution(modes[i]->w, modes[i]->h));
          }
        }

        std::sort(resolutions.begin(), resolutions.end(), ResolutionCompare);
        iter = resolutions.begin();
      }
    }


    ApplyTexture::ApplyTexture(cTextureRef pCurrent)
    {
      pLast = pRender->GetCurrentTexture0();
      pRender->SetTexture0(pCurrent);
    }

    ApplyTexture::~ApplyTexture()
    {
      pRender->SetTexture0(pLast);
    }


    ApplyMaterial::ApplyMaterial(material::cMaterialRef _pMaterial)
    {
      //pLast = pRender->GetCurrentMaterial();
      pMaterial = _pMaterial;
      pRender->ApplyMaterial(pMaterial);
    }

    ApplyMaterial::~ApplyMaterial()
    {
      pRender->UnApplyMaterial(pMaterial);
    }





    cRenderToScreen::cRenderToScreen()
    {
      pRender->BeginRenderToScreen();
    }

    cRenderToScreen::~cRenderToScreen()
    {
      pRender->EndRenderToScreen();
    }



    cRenderToTexture::cRenderToTexture(cTextureFrameBufferObjectRef _pTexture) :
      pTexture(_pTexture)
    {
      pRender->BeginRenderToTexture(pTexture);
    }

    cRenderToTexture::~cRenderToTexture()
    {
      pRender->EndRenderToTexture(pTexture);
    }



    cRenderToCubeMapTexture::cRenderToCubeMapTexture(cTextureFrameBufferObjectRef _pTexture, CUBE_MAP_FACE face) :
      pTexture(_pTexture)
    {
      pRender->BeginRenderToCubeMapTextureFace(pTexture, face);
    }

    cRenderToCubeMapTexture::~cRenderToCubeMapTexture()
    {
      pRender->EndRenderToCubeMapTextureFace(pTexture);
    }



    cRenderScreenSpace::cRenderScreenSpace()
    {
      pRender->BeginScreenSpaceRendering();
    }

    cRenderScreenSpace::~cRenderScreenSpace()
    {
      pRender->EndScreenSpaceRendering();
    }
  }
}
