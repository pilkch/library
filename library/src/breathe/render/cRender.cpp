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

    void cBatchController::FinishAdding()
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
    }

    // *** cRender

    cRender::cRender() :
      bRenderWireframe(false),

      bLight(true),
      bCubemap(true),
      bShader(true),

      bCanCubemap(false),
      bCanShader(false),
      bCanFrameBufferObject(false),

#ifdef BUILD_DEBUG
      bFullscreen(false),
#else
      bFullscreen(true),
#endif

      uiWidth(1024),
      uiHeight(768),
      uiDepth(32),

      uiTriangles(0),

      pLevel(nullptr),

      bActiveColour(false),
      bActiveShader(false),

      g_info(nullptr),
      videoInfo(nullptr)
    {
      pRender = this;

      uiFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;

      iMaxTextureSize=0;

      uiActiveUnits=0;

      sunPosition.Set(10.0f, 10.0f, 5.0f, 0.0f);

      pFrustum = new math::cFrustum;

      unsigned int i = 0;
      for (i=0;i<nAtlas;i++) {
        cTextureAtlasRef pNewTextureAtlas(new cTextureAtlas(i));
        vTextureAtlas.push_back(pNewTextureAtlas);
      }

      for (i=0;i<material::nLayers;i++) {
        material::cLayer layer;
        vLayer.push_back(layer);
      }
    }

    cRender::~cRender()
    {
      //TODO: Delete materials and shader objects and atlases etc.

      LOG.Success("Delete", "Frustum");
      SAFE_DELETE(pFrustum);

      unsigned int i = 0;
      for (i=0;i<nAtlas;i++) vTextureAtlas[i].reset();
      vTextureAtlas.clear();

      vLayer.clear();


      LOG.Success("Delete", "Frame Buffer Objects");
      pFrameBuffer0.reset();
      pFrameBuffer1.reset();

      LOG.Success("Delete", "Static Mesh");
      std::map<string_t, render::model::cStaticRef>::iterator iter = mStatic.begin();
      const std::map<string_t, render::model::cStaticRef>::iterator iterEnd = mStatic.end();
      while(iter != iterEnd) {
        iter->second.reset();
        iter++;
      };
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
      t<<const_cast<const unsigned char*>(glGetString( GL_EXTENSIONS ));

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

      // Create an SDL surface
      LOG<<"cRender::PreInit Calling SDL_SetVideoMode"<<std::endl;
      pSurface = SDL_SetVideoMode(uiWidth, uiHeight, uiDepth, uiFlags);

      // Verify there is a surface
      if (pSurface == nullptr) {
        LOG.Error("SDL", std::string("Video mode set failed: ") + SDL_GetError());
        return breathe::BAD;
      }

      return breathe::GOOD;
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
        bCanCubemap = true;
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
          bShader = bCanShader = true;
        } else {
          LOG.Error("Render", "Not Found Shader1.1, version found is Shader" + stm.str());
          bShader = bCanShader = false;
        }
      }

      if (bCanShader) LOG.Success("Render", "Can use shaders, shaders turned on");
      else LOG.Success("Render", "Cannot use shaders, shaders turned off");


      // Frame Buffer Object Support
      if (FindExtension(TEXT("GL_EXT_framebuffer_object"))) {
        LOG.Success("Render", "Found GL_EXT_framebuffer_object");
        bCanFrameBufferObject = true;
      }
      else LOG.Error("Render", "Not Found GL_EXT_framebuffer_object");



      glClearColor(1.0f, 0.0f, 1.0f, 0.0f);        // Clear The Background Color To Black
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

      glLightfv(GL_LIGHT0, GL_POSITION, sunPosition);

      glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
      glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);

      glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);

      glEnable(GL_COLOR_MATERIAL);
      glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

      glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
      glMaterialfv(GL_FRONT, GL_EMISSION, MaterialEmission);


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
      GLfloat ratio = (GLfloat)uiWidth / (GLfloat)uiHeight;

      // Setup our viewport
      glViewport(0, 0, ( GLint )uiWidth, ( GLint )uiHeight);

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
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glMultMatrixf(pFrustum->m);

      if (bRenderWireframe) EnableWireframe();
      else DisableWireframe();

      if (bLight) {
        glLightfv(GL_LIGHT0, GL_POSITION, sunPosition);
        glEnable(GL_LIGHTING);
      } else glDisable(GL_LIGHTING);

      ClearMaterial();
    }

    void cRender::BeginRenderToTexture(cTextureFrameBufferObjectRef pTexture)
    {
      glEnable(GL_TEXTURE_2D);

      // First we bind the FBO so we can render to it
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, pTexture->uiFBO);

      // Save the view port and set it to the size of the texture
      glPushAttrib(GL_VIEWPORT_BIT);
      glViewport(0, 0, FBO_TEXTURE_WIDTH, FBO_TEXTURE_HEIGHT);

      _BeginRenderShared();
    }

    void cRender::EndRenderToTexture(cTextureFrameBufferObjectRef pTexture)
    {
      // Restore old view port and set rendering back to default frame buffer
      glPopAttrib();
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

#ifdef RENDER_GENERATEFBOMIPMAPS
      glBindTexture(GL_TEXTURE_2D, pTexture->uiTexture);

        glGenerateMipmapEXT(GL_TEXTURE_2D);

      glBindTexture(GL_TEXTURE_2D, 0);
#endif // RENDER_GENERATEFBOMIPMAPS

      glDisable(GL_TEXTURE_2D);
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
      SDL_GL_SwapBuffers();
    }

    void cRender::_RenderPostRenderPass(material::cMaterialRef pMaterial, cTextureFrameBufferObjectRef pFBO)
    {
      ASSERT(pMaterial != nullptr);

      BeginScreenSpaceRendering();
        SetMaterial(pMaterial);
        glBindTexture(GL_TEXTURE_2D, pFBO->uiTexture);
        RenderScreenSpaceRectangle(0.0f, 0.0f, 1.0f, 1.0f);
      EndScreenSpaceRendering();
    }

    void cRender::Begin()
    {
      if (lPostRenderEffects.empty()) _BeginRenderToScreen();
    }

    void cRender::End()
    {
      _EndRenderToScreen();
    }

    void cRender::BeginRenderScene()
    {
      // If we are just rendering to the screen, no post rendering effects
      if (lPostRenderEffects.empty()) return;

      BeginRenderToTexture(pFrameBuffer0);
    }

    void cRender::EndRenderScene()
    {
      // Basically we render like this, if there are no post rendering effects,
      // render straight to the screen.
      // If there is one rendering effect render to pFrameBuffer0 then to the screen.
      // If there is more than one rendering effect, render to pFrameBuffer0,
      // then render pFrameBuffer0 to pFrameBuffer1, pFrameBuffer1 to pFrameBuffer0 ...
      // until n-1, for the last effect we render whichever FBO we last rendered to,
      // to the screen.
      size_t n = lPostRenderEffects.size();

      // If we are just rendering to the screen, no post rendering effects
      if (n == 0) return;

      // Ok, we actually want to do some exciting post render effects
      ASSERT(pFrameBuffer0 != nullptr);
      EndRenderToTexture(pFrameBuffer0);

      // We have just rendered to a texture, loop through the post render chain alternating
      // rendering to pFrameBuffer0 and pFrameBuffer1
      std::list<material::cMaterialRef>::iterator iter = lPostRenderEffects.begin();
      size_t i = 0;
      for (; i < n - 1; i++, iter++) {
        BeginRenderToTexture((i % 2) ? pFrameBuffer0 : pFrameBuffer1);
          _RenderPostRenderPass(*iter, ((i+1) % 2) ? pFrameBuffer0 : pFrameBuffer1);
        EndRenderToTexture((i % 2) ? pFrameBuffer0 : pFrameBuffer1);
      }

      // Finally draw our texture to the screen, we don't end rendering to the screen in this function,
      // from now on in our rendering process we use exactly the same method as non-FBO rendering
      _BeginRenderToScreen();
        _RenderPostRenderPass(*iter, (n==1 || ((i+1) % 2)) ? pFrameBuffer0 :pFrameBuffer1);
    }

    void cRender::SaveScreenShot()
    {
      int value = 0;
      GetApplicationUserSetting(TEXT("ScreenShot"), TEXT("Count"), value);
      SetApplicationUserSetting(TEXT("ScreenShot"), TEXT("Count"), value + 1);

      stringstream_t o;
      o<<filesystem::GetHomeImagesDirectory();
      o<<TEXT("screenshot");
      o<<value;
      o<<TEXT(".bmp");
      string_t sFilename(o.str());

      SDL_Surface* screen = pSurface;
      ASSERT(screen != nullptr);

      if (!(screen->flags & SDL_OPENGL)) {
        SDL_SaveBMP(screen, breathe::string::ToUTF8(sFilename).c_str());
        return;
      }

      SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE, screen->w, screen->h, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);
      if (temp == nullptr) return;

      unsigned char* pixels = (unsigned char*)malloc(3 * screen->w * screen->h);
      if (pixels == nullptr) {
        SDL_FreeSurface(temp);
        return;
      }

      glReadPixels(0, 0, screen->w, screen->h, GL_RGB, GL_UNSIGNED_BYTE, pixels);

      ASSERT(screen->h >= 0);
      const size_t n = screen->h;
      for (size_t i = 0; i < n; i++) memcpy(((char*)temp->pixels) + temp->pitch * i, pixels + 3 * screen->w * (screen->h-i - 1), screen->w * 3);

      free(pixels);

      SDL_SaveBMP(temp, breathe::string::ToUTF8(sFilename).c_str());
      SDL_FreeSurface(temp);
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
        glTexCoord2f(0.0f, 0.0f); glVertex2f(fX, fY);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(fX + fWidth, fY);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(fX + fWidth, fY + fHeight);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(fX, fY + fHeight);
      glEnd();
    }

    void cRender::RenderScreenSpaceRectangleTopLeftIsAt(
      float fX, float fY, float fWidth, float fHeight,
      float fU, float fV, float fU2, float fV2)
    {
      glBegin(GL_QUADS);
        glTexCoord2f(fU, fV);       glVertex2f(fX, fY);
        glTexCoord2f(fU + fU2, fV); glVertex2f(fX + fWidth, fY);
        glTexCoord2f(fU + fU2, fV + fV2);       glVertex2f(fX + fWidth, fY + fHeight);
        glTexCoord2f(fU, fV + fV2);             glVertex2f(fX, fY + fHeight);
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


      // Our screen coordinates look like this
      // 0.0f, 1.0f            1.0f, 1.0f
      //
      //
      // 0.0f, 0.0f            1.0f, 0.0f

      glPushAttrib(GL_TRANSFORM_BIT);

        // Setup projection matrix
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
          glLoadIdentity();
          glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

          BeginScreenSpaceGuiRendering();
    }

    void cRender::EndScreenSpaceRendering()
    {
          EndScreenSpaceGuiRendering();

        glMatrixMode( GL_PROJECTION );  // Select Projection
        glPopMatrix();                  // Pop The Matrix
      glPopAttrib();
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
        glOrtho( 0, fScale, 0, fScale, -1, 1 );

        // Setup modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
    }

    void cRender::EndScreenSpaceWorldRendering()
    {
          glMatrixMode( GL_MODELVIEW );
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
      ASSERT(pMesh != nullptr);
      ASSERT(pMesh->pMeshData);

      float* fVertices=&pMesh->pMeshData->vVertex[0];
      float* fTextureCoords=&pMesh->pMeshData->vTextureCoord[0];

      unsigned int triangle=0;
      unsigned int texcoord=0;
      unsigned int vert=0;
      //unsigned int mesh=0;
      unsigned int nTriangles = pMesh->pMeshData->uiTriangles;

      math::cVec3 v0;
      math::cVec3 v1;
      math::cVec3 v2;
      math::cVec3 n;

      glBegin(GL_TRIANGLES);

        if (1==uiActiveUnits)
          for (triangle=0;triangle<nTriangles;triangle++, vert+=9, texcoord+=6)
          {
            v0.Set(&fVertices[vert]);
            v1.Set(&fVertices[vert+3]);
            v2.Set(&fVertices[vert+6]);
            n.Cross(v0-v2, v2-v1);
                n.Normalise();

            glNormal3f(n.x, n.y, n.z);

            glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord], fTextureCoords[texcoord+1]);
            glVertex3f(fVertices[vert], fVertices[vert+1], fVertices[vert+2]);

            glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord+2], fTextureCoords[texcoord+3]);
            glVertex3f(fVertices[vert+3], fVertices[vert+4], fVertices[vert+5]);

            glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord+4], fTextureCoords[texcoord+5]);
            glVertex3f(fVertices[vert+6], fVertices[vert+7], fVertices[vert+8]);
          }
        else if (2==uiActiveUnits)
          for (triangle=0;triangle<nTriangles;triangle++, vert+=9, texcoord+=6)
          {
            v0.Set(&fVertices[vert]);
            v1.Set(&fVertices[vert+3]);
            v2.Set(&fVertices[vert+6]);
            n.Cross(v0-v2, v2-v1);
                                                                                n.Normalise();

            glNormal3f(n.x, n.y, n.z);
            glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord], fTextureCoords[texcoord+1]);
            glMultiTexCoord2f( GL_TEXTURE1, fTextureCoords[texcoord], fTextureCoords[texcoord+1]);
            glVertex3f(fVertices[vert], fVertices[vert+1], fVertices[vert+2]);

            //n.Cross(v1-v0, v0-v2);

            //glNormal3f(n.x, n.y, n.z);
            glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord+2], fTextureCoords[texcoord+3]);
            glMultiTexCoord2f( GL_TEXTURE1, fTextureCoords[texcoord+2], fTextureCoords[texcoord+3]);
            glVertex3f(fVertices[vert+3], fVertices[vert+4], fVertices[vert+5]);

            //n.Cross(v2-v1, v1-v0);

            //glNormal3f(n.x, n.y, n.z);
            glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord+4], fTextureCoords[texcoord+5]);
            glMultiTexCoord2f( GL_TEXTURE1, fTextureCoords[texcoord+4], fTextureCoords[texcoord+5]);
            glVertex3f(fVertices[vert+6], fVertices[vert+7], fVertices[vert+8]);
          }
        else if (3==uiActiveUnits)
          for (triangle=0;triangle<nTriangles;triangle++, vert+=9, texcoord+=6)
          {
            glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord], fTextureCoords[texcoord+1]);
            glMultiTexCoord2f( GL_TEXTURE1, fTextureCoords[texcoord], fTextureCoords[texcoord+1]);
            glMultiTexCoord2f( GL_TEXTURE2, fTextureCoords[texcoord], fTextureCoords[texcoord+1]);
            glVertex3f(fVertices[vert], fVertices[vert+1], fVertices[vert+2]);

            glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord+2], fTextureCoords[texcoord+3]);
            glMultiTexCoord2f( GL_TEXTURE1, fTextureCoords[texcoord+2], fTextureCoords[texcoord+3]);
            glMultiTexCoord2f( GL_TEXTURE2, fTextureCoords[texcoord+2], fTextureCoords[texcoord+3]);
            glVertex3f(fVertices[vert+3], fVertices[vert+4], fVertices[vert+5]);

            glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord+4], fTextureCoords[texcoord+5]);
            glMultiTexCoord2f( GL_TEXTURE1, fTextureCoords[texcoord+4], fTextureCoords[texcoord+5]);
            glMultiTexCoord2f( GL_TEXTURE2, fTextureCoords[texcoord+4], fTextureCoords[texcoord+5]);
            glVertex3f(fVertices[vert+6], fVertices[vert+7], fVertices[vert+8]);
          }
        else if (bCubemap)
        {
          //std::ostringstream t;
          //t << uiActiveUnits;
          //LOG.Error("RenderStaticModel", "Invalid texture unit count " + t.str());
        }

      glEnd();
    }

    unsigned int cRender::RenderStaticModel(model::cStaticRef p)
    {
      if (nullptr == p) return 0;

      unsigned int uiTriangles = 0;
      unsigned int nMeshes = 0;

      std::vector<model::cMeshRef> vMesh=p->vMesh;

      nMeshes = static_cast<unsigned int>(vMesh.size());

      for (size_t mesh=0;mesh<nMeshes;mesh++) {
        ASSERT(vMesh[mesh]->pMeshData);

        if (nullptr == vMesh[mesh]->pMaterial) vMesh[mesh]->pMaterial = GetMaterial(vMesh[mesh]->sMaterial);
        SetMaterial(vMesh[mesh]->pMaterial);

        RenderMesh(vMesh[mesh]);
        uiTriangles += vMesh[mesh]->pMeshData->uiTriangles;
      }

      return uiTriangles;
    }

    unsigned int cRender::RenderStaticModel(model::cStaticRef p, const math::cColour& colour)
    {
      SetColour(colour);

      unsigned int uiTriangles = RenderStaticModel(p);

      ClearColour();

      return uiTriangles;
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
      pMaterialNotFoundMaterial->vLayer[0]->uiTextureMode = TEXTURE_NORMAL;

      return pMaterialNotFoundMaterial;
    }

    material::cMaterialRef cRender::AddMaterial(const string_t& sNewfilename)
    {
      if (TEXT("") == sNewfilename) return material::cMaterialRef();

      material::cMaterialRef pMaterial = GetMaterial(sNewfilename);

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

    float Angle(const math::cVec2 & a, const math::cVec2 & b)
    {
      if (a.x>b.x)
      {
        if (a.y>b.y)
          return (atan((a.y-b.y)/(a.x-b.x)) + math::cPI_DIV_180 * 90.0f) * math::c180_DIV_PI;

        return (-atan((a.y-b.y)/(b.x-a.x)) + math::cPI_DIV_180 * 90.0f) * math::c180_DIV_PI;
      }


      if (b.y>a.y)
        return (atan((b.y-a.y)/(b.x-a.x)) + math::cPI_DIV_180 * 270.0f) * math::c180_DIV_PI;

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
        if (TEXTURE_MASK == layerOld->uiTextureMode ||
            TEXTURE_BLEND == layerOld->uiTextureMode)
        {
          glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
          glBlendFunc(GL_ONE, GL_ZERO);
          glDisable(GL_BLEND);
        }
        else if (  TEXTURE_DETAIL==layerOld->uiTextureMode)
        {
          // Reset the texture matrix
          glMatrixMode(GL_TEXTURE);
          glLoadIdentity();
          glMatrixMode(GL_MODELVIEW);

          //glEnable(GL_LIGHTING);
        }
        else if (TEXTURE_CUBEMAP==layerOld->uiTextureMode)
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
        layerOld->uiTextureMode = TEXTURE_NONE;
        layerOld->pTexture.reset();
        layerOld->sTexture = TEXT("");
      }

      glActiveTexture(GL_TEXTURE0);
      glEnable(GL_TEXTURE_2D);
      glDisable(GL_LIGHTING);

      bActiveShader = false;

      if (bCanShader) glUseProgram(0);

      pCurrentMaterial.reset();

      ClearColour();

      return true;
    }

    bool cRender::SetShaderConstant(material::cMaterialRef pMaterial, const std::string& sConstant, int value)
    {
      ASSERT(pMaterial != nullptr);
      ASSERT(pMaterial->pShader != nullptr);

      GLint loc = glGetUniformLocation(pMaterial->pShader->uiShaderProgram, sConstant.c_str());
      if (loc == -1) {
        LOG.Error("cRender::SetShaderConstant", breathe::string::ToUTF8(pMaterial->sName) + " Couldn't set \"" + sConstant + "\" perhaps the constant is not actually used within the shader");
        ASSERT(loc > 0);
        return false;
      }

      glUniform1i(loc, value);
      return true;
    }

    bool cRender::SetShaderConstant(material::cMaterialRef pMaterial, const std::string& sConstant, float value)
    {
      ASSERT(pMaterial != nullptr);
      ASSERT(pMaterial->pShader != nullptr);

      GLint loc = glGetUniformLocation(pMaterial->pShader->uiShaderProgram, sConstant.c_str());
      if (loc == -1) {
        LOG.Error("cRender::SetShaderConstant", breathe::string::ToUTF8(pMaterial->sName) + " Couldn't set \"" + sConstant + "\" perhaps the constant is not actually used within the shader");
        ASSERT(loc > 0);
        return false;
      }

      glUniform1f(loc, value);
      return true;
    }

    bool cRender::SetShaderConstant(material::cMaterialRef pMaterial, const std::string& sConstant, const math::cVec3& value)
    {
      assert(pMaterial != nullptr);
      assert(pMaterial->pShader != nullptr);

      GLint loc = glGetUniformLocation(pMaterial->pShader->uiShaderProgram, sConstant.c_str());
      if (loc == -1) {
        LOG.Error("cRender::SetShaderConstant", breathe::string::ToUTF8(pMaterial->sName) + " Couldn't set \"" + sConstant + "\" perhaps the constant is not actually used within the shader");
        ASSERT(loc > 0);
        return false;
      }

      glUniform3f(loc, value.x, value.y, value.z);
      return true;
    }

    bool cRender::SetMaterial(material::cMaterialRef pMaterial, const math::cVec3& pos)
    {
      ASSERT(pMaterial != nullptr);

      if (pCurrentMaterial == pMaterial)
      {
        // Update camera possibly?
        return true;
      }

      uiTextureModeChanges++;
      //uiTextureChanges

      unsigned int i=0;
      unsigned int n=material::nLayers;
      unsigned int unit=0;

      material::cLayer* layerOld;
      material::cLayer* layerNew;

      for (i=0;i<n;i++)
      {
        layerNew = pMaterial->vLayer[i];

        if (  TEXTURE_NONE==layerNew->uiTextureMode || (TEXTURE_CUBEMAP==layerNew->uiTextureMode && !bCubemap))
          n = i;

        //if ((TEXTURE_NONE!=layerNew->uiTextureMode && TEXTURE_CUBEMAP!=layerNew->uiTextureMode) || (TEXTURE_CUBEMAP==layerNew->uiTextureMode && bCubemap))
        //  n = i;
      }

      //if (0 == n)
      //  LOG.Error("Render", "No layers to render");

      uiActiveUnits=n;

      unit=GL_TEXTURE0_ARB;

      for (i=0;i<n;i++, unit++)
      {
        layerNew = pMaterial->vLayer[i];
        layerOld = &vLayer[i];

#ifdef BUILD_LEVEL
        //If this is a cubemap, set the material texture to the cubemap before we get there
        if (TEXTURE_CUBEMAP==layerNew->uiTextureMode)
          layerNew->pTexture=pLevel->FindClosestCubeMap(pos);
#endif

        //Activate the current texture unit
        glActiveTexture(unit);

        //Different mode, probably means different texture, change mode and bind it anyway
        if (layerOld->uiTextureMode!=layerNew->uiTextureMode)
        {
          //Undo last mode
          if (TEXTURE_MASK==layerOld->uiTextureMode ||
              TEXTURE_BLEND==layerOld->uiTextureMode ||
              TEXTURE_DETAIL==layerOld->uiTextureMode)
          {
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glBlendFunc(GL_ONE, GL_ZERO);
            glDisable(GL_BLEND);
          }
          else if (TEXTURE_CUBEMAP==layerOld->uiTextureMode)
          {
            glMatrixMode(GL_TEXTURE);
            glPopMatrix();

            glMatrixMode(GL_MODELVIEW);

            glDisable(GL_TEXTURE_CUBE_MAP);

            glDisable(GL_TEXTURE_GEN_R);
            glDisable(GL_TEXTURE_GEN_T);
            glDisable(GL_TEXTURE_GEN_S);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 1);

            glEnable(GL_TEXTURE_2D);
          }

          //Set the current mode and texture
          layerOld->uiTextureMode=layerNew->uiTextureMode;
          layerOld->pTexture=layerNew->pTexture;

          if (layerOld->pTexture == nullptr) continue;

          if (TEXTURE_NONE==layerOld->uiTextureMode)
            glDisable(GL_TEXTURE_2D);
          else if (TEXTURE_NORMAL==layerOld->uiTextureMode)
          {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);
          }
          else if (TEXTURE_MASK==layerOld->uiTextureMode)
          {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glEnable(GL_BLEND);
          }
          else if (TEXTURE_BLEND==layerOld->uiTextureMode)
          {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
            glEnable(GL_BLEND);
          }
          else if (TEXTURE_DETAIL==layerOld->uiTextureMode)
          {
            glActiveTexture(GL_TEXTURE0_ARB);
            //glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
            //glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);

            glActiveTexture(GL_TEXTURE1_ARB);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);
            //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
            //glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);

            // Change the texture matrix so that we have more detail than normal texture
            glMatrixMode(GL_TEXTURE);
              glLoadIdentity();
              glScalef(fDetailScale.GetFloat(), fDetailScale.GetFloat(), 1);
              glMatrixMode(GL_MODELVIEW);

              // General Switches
              glDisable(GL_BLEND);
              glEnable(GL_LIGHTING);
          }
          else if (TEXTURE_CUBEMAP==layerOld->uiTextureMode)
          {
            //Assume we got one we shouldn't be here if we didn't
            //It is possible if there are NO cubemaps in the whole level,
            //so make sure we load one already
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_TEXTURE_CUBE_MAP);
            glBindTexture(GL_TEXTURE_CUBE_MAP, layerOld->pTexture->uiTexture);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);



            glMatrixMode(GL_TEXTURE);
            glPushMatrix();
            glLoadIdentity();

            float y = -Angle(math::cVec2(pFrustum->eye.x, pFrustum->eye.y), math::cVec2(pFrustum->target.x, pFrustum->target.y));
            float x = -Angle(math::cVec2(pFrustum->eye.y, pFrustum->eye.z), math::cVec2(pFrustum->target.y, pFrustum->target.z));
            //std::cout<<y<<"\t"<<x<<"\n";

            glRotatef(y, 0.0f, 1.0f, 0.0f);
            glRotatef(x, 1.0f, 0.0f, 0.0f);


            //float mat[16];
            //glGetFloatv(GL_MODELVIEW_MATRIX, mat);

            //math::cQuaternion q(mat[8], mat[9], -mat[10]);

            //glLoadMatrixf(static_cast<float* >(q.GetMatrix()));


            glMatrixMode(GL_MODELVIEW);


            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glEnable(GL_TEXTURE_GEN_R);

            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
            glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
          }
          else if (TEXTURE_POST_RENDER!=layerOld->uiTextureMode)
          {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);
          }

          if (0==unit)
          {
            if (n>1)
            {
              glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            }
            else
            {
              glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            }
          }
        }


        // **************************************************************************************
        // **************************************************************************************
        // **************************************************************************************
        // **************************************************************************************
        // **************************************************************************************
        // **************************************************************************************
        // **************************************************************************************
        // **************************************************************************************
        //Same Mode, just change texture
        else if (layerOld->pTexture->uiTexture!=layerNew->pTexture->uiTexture)
        {
          uiTextureChanges++;
          if (  TEXTURE_MASK==layerOld->uiTextureMode ||
              TEXTURE_DETAIL==layerOld->uiTextureMode)
          {
            layerOld->pTexture->uiTexture=layerNew->pTexture->uiTexture;
            glBindTexture(GL_TEXTURE_2D, layerNew->pTexture->uiTexture);
          }
          else if (TEXTURE_BLEND==layerOld->uiTextureMode)
          {
            layerOld->pTexture->uiTexture=layerNew->pTexture->uiTexture;
            glBindTexture(GL_TEXTURE_2D, layerNew->pTexture->uiTexture);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
            glEnable(GL_BLEND);
          }
#ifdef BUILD_LEVEL
          else if (TEXTURE_CUBEMAP==layerOld->uiTextureMode)
          {
            layerOld->pTexture->uiTexture=layerNew->pTexture->uiTexture;

            cTextureRef t=pLevel->FindClosestCubeMap(pos);

            if (t)
            {
              glDisable(GL_TEXTURE_2D);
              glEnable(GL_TEXTURE_CUBE_MAP);
              glBindTexture(GL_TEXTURE_CUBE_MAP, t->uiTexture);

              //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
              glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MULT);

              glEnable(GL_TEXTURE_GEN_S);
              glEnable(GL_TEXTURE_GEN_T);
              glEnable(GL_TEXTURE_GEN_R);

              glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_REFLECTION_MAP);
              glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_REFLECTION_MAP);
              glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_REFLECTION_MAP);
            }
          }
#endif
          else
          {
            glBlendFunc(GL_ONE, GL_ZERO);
            glDisable(GL_BLEND);
          }


          //Set the current mode and texture
          layerOld->uiTextureMode=layerNew->uiTextureMode;
          layerOld->pTexture->uiTexture=layerNew->pTexture->uiTexture;





          if (TEXTURE_NONE==layerOld->uiTextureMode)
            glDisable(GL_TEXTURE_2D);
          else if (TEXTURE_NORMAL==layerOld->uiTextureMode)
          {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);
          }
          else if (TEXTURE_MASK==layerOld->uiTextureMode)
          {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glEnable(GL_BLEND);
          }
          else if (TEXTURE_BLEND==layerOld->uiTextureMode ||
                  TEXTURE_DETAIL==layerOld->uiTextureMode)
          {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
            glEnable(GL_BLEND);
          }
          else if (TEXTURE_CUBEMAP==layerOld->uiTextureMode)
          {
            //Assume we got one we shouldn't be here if we didn't
            //It is possible if there are NO cubemaps in the whole level,
            //so make sure we load one already
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_TEXTURE_CUBE_MAP);
            glBindTexture(GL_TEXTURE_CUBE_MAP, layerOld->pTexture->uiTexture);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);



            glMatrixMode(GL_TEXTURE);
            glPushMatrix();
            glLoadIdentity();

            float y=-Angle(math::cVec2(pFrustum->eye.x, pFrustum->eye.y),
              math::cVec2(pFrustum->target.x, pFrustum->target.y));
            float x=-Angle(math::cVec2(pFrustum->eye.y, pFrustum->eye.z),
              math::cVec2(pFrustum->target.y, pFrustum->target.z));
            //std::cout<<y<<"\t"<<x<<"\n";

            glRotatef(y, 0.0f, 1.0f, 0.0f);
            glRotatef(x, 1.0f, 0.0f, 0.0f);


            //float mat[16];
            //glGetFloatv(GL_MODELVIEW_MATRIX, mat);

            //math::cQuaternion q(mat[8], mat[9], -mat[10]);

            //glLoadMatrixf(static_cast<float*>(q.GetMatrix()));


            glMatrixMode(GL_MODELVIEW);


            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glEnable(GL_TEXTURE_GEN_R);

            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
            glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
          }
          else
          {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);
          }

          if (0==unit)
          {
            if (n>1)
            {
              glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            }
            else
            {
              glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            }
          }
        }





        // **************************************************************************************
        // **************************************************************************************
        // **************************************************************************************
        // **************************************************************************************
        // **************************************************************************************
        // **************************************************************************************
        // **************************************************************************************
        // **************************************************************************************
      }

      unit = GL_TEXTURE0_ARB + n;

      for (i=n;i<material::nLayers;i++, unit++)
      {
        layerNew = pMaterial->vLayer[i];
        layerOld = &vLayer[i];

        //Activate the current texture unit
        glActiveTexture(unit);

        //Undo last mode
        if (TEXTURE_MASK==layerOld->uiTextureMode ||
            TEXTURE_BLEND==layerOld->uiTextureMode)
        {
          glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
          glBlendFunc(GL_ONE, GL_ZERO);
          glDisable(GL_BLEND);
        }
        else if (TEXTURE_DETAIL==layerOld->uiTextureMode)
        {
          // Change the texture matrix so that we have more detail than normal texture
          glMatrixMode(GL_TEXTURE);
            glLoadIdentity();
          glMatrixMode(GL_MODELVIEW);

          // General Switches
          glDisable(GL_BLEND);
          //glEnable(GL_LIGHTING);
        }
        else if (TEXTURE_CUBEMAP==layerOld->uiTextureMode)
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

        glDisable(GL_TEXTURE_2D);

        //Set the current mode and texture
        layerOld->uiTextureMode=TEXTURE_NONE;
      }



      if (1==uiActiveUnits)
      {
        glActiveTexture(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);

        if (TEXTURE_NORMAL==vLayer[0].uiTextureMode && bActiveColour)
        {
          glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, colour);

          glColor4f(colour.r, colour.g, colour.b, colour.a);
        }
        else
        {
          float a0[4] = {1.0f, 1.0f, 1.0f, 1.0f};
          glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, a0);
        }
      }
      else if (2==uiActiveUnits)
      {
        if (TEXTURE_DETAIL==pMaterial->vLayer[1]->uiTextureMode)
        {
          // TEXTURE-UNIT #0
          glActiveTexture(GL_TEXTURE0_ARB);
          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D, pMaterial->vLayer[0]->pTexture->uiTexture);
          //glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
          //glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);

          // TEXTURE-UNIT #1
          glActiveTexture(GL_TEXTURE1_ARB);
          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D, pMaterial->vLayer[1]->pTexture->uiTexture);
          //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
          //glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);

          // Now we want to enter the texture matrix.  This will allow us
          // to change the tiling of the detail texture.
          glMatrixMode(GL_TEXTURE);

            // Reset the current matrix and apply our chosen scale value
            glLoadIdentity();
            glScalef(fDetailScale.GetFloat(), fDetailScale.GetFloat(), 1);

            // Leave the texture matrix and set us back in the model view matrix
            glMatrixMode(GL_MODELVIEW);

            // General Switches
            glDisable(GL_BLEND);
            //glDisable(GL_LIGHTING);
        }
        else
        {
          {
            glActiveTexture(GL_TEXTURE1_ARB);

            float a1[4] = {0.6f, 0.6f, 0.6f, 1.0f};
            glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, a1);
          }

          glActiveTexture(GL_TEXTURE0_ARB);
          if (  TEXTURE_NORMAL==vLayer[0].uiTextureMode && bActiveColour &&
              !TEXTURE_DETAIL==vLayer[1].uiTextureMode)
          {
            glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, colour);
            glColor4f(colour.r, colour.g, colour.b, colour.a);
          }
          else
          {
            float a0[4] = {1.0f, 1.0f, 1.0f, 1.0f};
            glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, a0);
          }
        }
      }
      else if (3==uiActiveUnits)
      {
        glActiveTexture(GL_TEXTURE2_ARB);
        float a2[4] = {0.5f, 0.5f, 0.5f, 1.0f};
        glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, a2);

        glActiveTexture(GL_TEXTURE1_ARB);
        float a1[4] = {0.5f, 0.5f, 0.5f, 1.0f};
        glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, a1);

        glActiveTexture(GL_TEXTURE0_ARB);
        if (TEXTURE_NORMAL==vLayer[0].uiTextureMode && bActiveColour)
        {
          glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, colour);

          glColor4f(colour.r, colour.g, colour.b, colour.a);
        }
        else
        {
          float a0[4] = {1.0f, 1.0f, 1.0f, 1.0f};
          glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, a0);
        }
      }
      //else disable none of them

      if (pMaterial->pShader && bShader)
      {
        bActiveShader = true;

        glUseProgram(pMaterial->pShader->uiShaderProgram);

        // TODO: We also need some more variables within our post render shaders such as
        // brightness: HDR, Top Gear Shader, Night Vision
        // exposure: HDR, Top Gear Shader
        // sunPosition: Car Shader, shadows, grass

        if (pMaterial->pShader->bCameraPos)
          SetShaderConstant(pMaterial, "cameraPos", pFrustum->eye);

        if (uiActiveUnits>0 && pMaterial->pShader->bTexUnit0)
          SetShaderConstant(pMaterial, "texUnit0", 0);

        if (uiActiveUnits>1 && pMaterial->pShader->bTexUnit1)
          SetShaderConstant(pMaterial, "texUnit1", 1);

        if (uiActiveUnits>2 && pMaterial->pShader->bTexUnit2)
          SetShaderConstant(pMaterial, "texUnit2", 2);

        if (uiActiveUnits>3 && pMaterial->pShader->bTexUnit3)
          SetShaderConstant(pMaterial, "texUnit3", 3);

        glEnable(GL_LIGHTING);
      } else if (bActiveShader) {
        bActiveShader = false;

        if (bCanShader) glUseProgram(0);
      }

      pCurrentMaterial=pMaterial;
      return true;
    }

    material::cMaterialRef cRender::GetMaterial(const string_t& sFilename)
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

    material::cMaterialRef cRender::AddPostRenderEffect(const string_t& sFilename)
    {
      material::cMaterialRef pMaterial = AddMaterial(sFilename);
      ASSERT(pMaterial != nullptr);
      lPostRenderEffects.push_back(pMaterial);

      if  (!pFrameBuffer0) {
        pFrameBuffer0.reset(new cTextureFrameBufferObject);
        pFrameBuffer0->Create();
      }

      if (lPostRenderEffects.size() > 1 && !pFrameBuffer1) {
        pFrameBuffer1.reset(new cTextureFrameBufferObject);
        pFrameBuffer1->Create();
      }

      return pMaterial;
    }

    void cRender::RemovePostRenderEffect()
    {
      if (!lPostRenderEffects.empty()) lPostRenderEffects.pop_back();
    }


    model::cStaticRef cRender::CreateNewModel(const string_t& sName)
    {
      model::cStaticRef pModel = mStatic[sName];
      if (pModel != nullptr) return pModel;

      pModel.reset(new model::cStatic);

      mStatic[sName] = pModel;

      return pModel;
    }

    model::cStaticRef cRender::AddModel(const string_t& sShortFilename)
    {
      model::cStaticRef pModel = GetModel(sShortFilename);
      if (pModel != nullptr) return pModel;

      pModel.reset(new model::cStatic);

      string_t sNewfilename;
      breathe::filesystem::FindResourceFile(sShortFilename, sNewfilename);
      if (pModel->Load(sNewfilename)) {
        mStatic[sNewfilename] = pModel;

        const size_t n = pModel->vMesh.size();
        for (size_t i = 0; i < n; i++) {
          pModel->vMesh[i]->pMaterial = AddMaterial(pModel->vMesh[i]->sMaterial);
        }

        return pModel;
      }

      return model::cStaticRef();
    }

    model::cStaticRef cRender::GetModel(const string_t& sFilename)
    {
      LOG<<"cRender::GetModel Looking for "<<sFilename<<std::endl;
#ifndef NDEBUG
      {
        // Print out the current list of models
        std::map<string_t, model::cStaticRef>::iterator iter = mStatic.begin();
        std::map<string_t, model::cStaticRef>::iterator iterEnd = mStatic.end();
        while (iter != iterEnd) {
          LOG<<"cRender::GetModel static["<<iter->first<<"]"<<std::endl;
          iter++;
        }
      }
#endif

      std::map<string_t, model::cStaticRef>::iterator iter(mStatic.find(sFilename));
      if (iter != mStatic.end()) {
        model::cStaticRef pModel = iter->second;
        if (pModel != nullptr) return pModel;
      }


      iter = mStatic.find(sFilename);
      if (iter != mStatic.end()) {
        model::cStaticRef pModel = iter->second;
        if (pModel != nullptr) return pModel;
      }

      LOG<<"cRender::GetModel Couldn't find "<<breathe::string::ToUTF8(sFilename)<<std::endl;

      return model::cStaticRef();
    }




    void cRender::ClearColour()
    {
      bActiveColour = false;

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
      bActiveColour = true;

      colour = inColour;

      glColor4f(colour.r, colour.g, colour.b, colour.a);
    }


    void cRender::TransformModels()
    {
      cTextureRef t;
      material::cMaterialRef mat;

      model::cStaticRef s;
      model::cMeshRef pMesh;
      float* fTextureCoords=NULL;
      size_t nMeshes=0;
      //unsigned int uiTriangles=0;
      size_t nTexcoords=0;
      unsigned int mesh=0;
      unsigned int texcoord=0;
      //unsigned int triangle=0;

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

            /*Init all vertex normals to zero

            for all faces:
              compute face normal

            for every vertex in every face:
              add face normal to vertex normal
              for all adjacent faces:
                  if the dotproduct of the face normal and the adjacentface normal is > 0.71:
                      add adjacentface normal to vertex normal

            for all vertex normals:
                                                                                normalise vertex normal*/
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

      LOG.Success("Render", "TransformModels returning");
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
          vVertexBufferObject[i]->Init();
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


    ApplyMaterial::ApplyMaterial(material::cMaterialRef pCurrent)
    {
      pLast = pRender->GetCurrentMaterial();
      pRender->SetMaterial(pCurrent);
    }

    ApplyMaterial::~ApplyMaterial()
    {
      pRender->SetMaterial(pLast);
    }
  }
}
