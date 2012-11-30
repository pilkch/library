// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <list>
#include <vector>

// SDL headers
#include <SDL/SDL_image.h>
#include <SDL/SDL_syswm.h>

// Spitfire headers
#include <spitfire/util/log.h>

// libopenglmm headers
#include <libopenglmm/cContext.h>
#ifdef BUILD_OPENGLMM_FONT
#include <libopenglmm/cFont.h>
#endif
#include <libopenglmm/cShader.h>
#include <libopenglmm/cSystem.h>
#include <libopenglmm/cTexture.h>
#include <libopenglmm/cVertexBufferObject.h>
#include <libopenglmm/cWindow.h>
#include <libopenglmm/opengl.h>

#if (BUILD_LIBOPENGLMM_SDL_VERSION < 130) && (BUILD_LIBOPENGLMM_OPENGL_VERSION >= 300)
#ifdef __WIN__
typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);

#define WGL_CONTEXT_MAJOR_VERSION_ARB          0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB          0x2092
#define WGL_CONTEXT_FLAGS_ARB                  0x2094
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002
#define WGL_CONTEXT_PROFILE_MASK_ARB           0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB       0x00000001
#else
#include <GL/glx.h>

#define GLX_CONTEXT_DEBUG_BIT_ARB                0x00000001
#define GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB   0x00000002
#define GLX_CONTEXT_MAJOR_VERSION_ARB            0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB            0x2092
#define GLX_CONTEXT_FLAGS_ARB                    0x2094
#endif
#endif

#ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
// Use the old EXT versions
#ifndef GL_FRAMEBUFFER
#define GL_FRAMEBUFFER GL_FRAMEBUFFER_EXT
#endif
#ifndef glBindFramebuffer
#define glBindFramebuffer glBindFramebufferEXT
#endif
#endif

namespace opengl
{
  cContext::cContext(cSystem& _system, const cWindow& window) :
    system(_system),
    bIsRenderingToWindow(true),
    bIsValid(false),
    resolution(window.GetResolution()),
    pSurface(nullptr),
    targetWidth(0),
    targetHeight(0),
    clearColour(0.0f, 0.0f, 0.0f, 1.0f),
    ambientColour(1.0f, 1.0f, 1.0f, 1.0f),
    sunAmbientColour(1.0f, 1.0f, 1.0f, 1.0f),
    fSunIntensity(0.0f),
    pCurrentShader(nullptr)
  {
    LOG<<"cContext::cContext with window "<<std::endl;

    if (!_SetWindowVideoMode(window.IsFullScreen())) {
      LOG<<"cContext::cContext Error setting video mode"<<std::endl;
      assert(false);
    }

    _SetDefaultFlags();
    _SetPerspective(resolution.width, resolution.height);

    system.UpdateCapabilities();


    /*
    const cCapabilities& capabilities = system.GetCapabilities();

    bIsFSAAEnabled = false;

    if (capabilities.bIsFSAASupported) {
      // Only sample at 2, 4, 8 or 16
      if (capabilities.uiFSAASampleLevel > 16) capabilities.uiFSAASampleLevel = 16;
      else if (capabilities.uiFSAASampleLevel > 8) capabilities.uiFSAASampleLevel = 8;
      else if (capabilities.uiFSAASampleLevel > 4) capabilities.uiFSAASampleLevel = 4;
      else capabilities.uiFSAASampleLevel = 2;

      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, uiFSAASampleLevel);


      if (!IsMultiSampling()) {
          bIsFSAAEnabled = false;
      } else {
          size_t uiActualSampleLevel = GetMultiSampleLevel();
          if (uiFSAASampleLevel != uiActualSampleLevel) {
            LOG<<"cSystem::Create Requested sample level is "<<uiFSAASampleLevel<<", actual sample level is "<<uiActualSampleLevel<<std::endl;
          }

          if ((uiActualSampleLevel == 2) || (uiActualSampleLevel == 4) || (uiActualSampleLevel == 8) || (uiActualSampleLevel == 16)) bIsFSAAEnabled = true;
      }
    }*/

    bIsValid = true;

    LOG<<"cContext::cContext with window returning, "<<cSystem::GetErrorString()<<std::endl;
  }

  cContext::cContext(cSystem& _system, const cResolution& _resolution) :
    system(_system),
    bIsRenderingToWindow(false),
    bIsValid(false),
    resolution(_resolution),
    pSurface(nullptr),
    targetWidth(0),
    targetHeight(0),
    clearColour(0.0f, 0.0f, 0.0f, 1.0f),
    ambientColour(1.0f, 1.0f, 1.0f, 1.0f),
    pCurrentShader(nullptr)
  {
    LOG<<"cContext::cContext"<<std::endl;

    _SetDefaultFlags();
    _SetPerspective(resolution.width, resolution.height);
  }

  cContext::~cContext()
  {
    LOG<<"cContext::~cContext"<<std::endl;

    assert(pCurrentShader == nullptr); // Shaders must be unbound before this point

#ifdef BUILD_OPENGLMM_FONT
    assert(fonts.empty());
#endif
    assert(textures.empty());
    assert(cubeMapTextures.empty());
    assert(shaders.empty());
    assert(staticVertexBufferObjects.empty());

    if (pSurface != nullptr) {
      SDL_FreeSurface(pSurface);
      pSurface = nullptr;
    }
  }

  bool cContext::IsValid() const
  {
    return bIsValid;
  }


  cTexture* cContext::CreateTexture(const opengl::string_t& sFileName)
  {
    voodoo::cImage image;
    if (!image.LoadFromFile(sFileName)) return nullptr;

    cTexture* pTexture = new cTexture;
    if (!pTexture->CreateFromImage(image)) {
      delete pTexture;
      return nullptr;
    }

    //textures.push_back(pTexture);

    return pTexture;
  }

  cTexture* cContext::CreateTextureNoMipMaps(const opengl::string_t& sFileName)
  {
    voodoo::cImage image;
    if (!image.LoadFromFile(sFileName)) return nullptr;

    cTexture* pTexture = new cTexture;
    pTexture->SetDoNotUseMipMaps();
    if (!pTexture->CreateFromImage(image)) {
      delete pTexture;
      return nullptr;
    }

    //textures.push_back(pTexture);

    return pTexture;
  }

  cTexture* cContext::CreateTextureFromImage(const voodoo::cImage& image)
  {
    assert(image.IsValid());

    cTexture* pTexture = new cTexture;
    if (!pTexture->CreateFromImage(image)) {
      delete pTexture;
      return nullptr;
    }

    //textures.push_back(pTexture);

    return pTexture;
  }

  cTexture* cContext::CreateTextureFromBuffer(const uint8_t* pBuffer, size_t width, size_t height, PIXELFORMAT pixelFormat)
  {
    assert(pBuffer != nullptr);

    voodoo::cImage image;
    if (!image.CreateFromBuffer(pBuffer, width, height, pixelFormat)) return nullptr;

    cTexture* pTexture = new cTexture;
    if (!pTexture->CreateFromImage(image)) {
      delete pTexture;
      return nullptr;
    }

    //textures.push_back(pTexture);

    return pTexture;
  }

  cTexture* cContext::CreateTextureFromBufferNoMipMaps(const uint8_t* pBuffer, size_t width, size_t height, PIXELFORMAT pixelFormat)
  {
    assert(pBuffer != nullptr);

    voodoo::cImage image;
    if (!image.CreateFromBuffer(pBuffer, width, height, pixelFormat)) return nullptr;

    cTexture* pTexture = new cTexture;
    pTexture->SetDoNotUseMipMaps();
    if (!pTexture->CreateFromImage(image)) {
      delete pTexture;
      return nullptr;
    }

    //textures.push_back(pTexture);

    return pTexture;
  }

  void cContext::DestroyTexture(cTexture* pTexture)
  {
    assert(pTexture != nullptr);

    //textures.remove(pTexture);

    pTexture->Destroy();
    delete pTexture;
  }

  cTextureCubeMap* cContext::CreateTextureCubeMap(
    const opengl::string_t& filePathPositiveX,
    const opengl::string_t& filePathNegativeX,
    const opengl::string_t& filePathPositiveY,
    const opengl::string_t& filePathNegativeY,
    const opengl::string_t& filePathPositiveZ,
    const opengl::string_t& filePathNegativeZ
  )
  {
    cTextureCubeMap* pTexture = new cTextureCubeMap;
    if (!pTexture->CreateFromFilePaths(
        filePathPositiveX,
        filePathNegativeX,
        filePathPositiveY,
        filePathNegativeY,
        filePathPositiveZ,
        filePathNegativeZ
      )
    ) {
      delete pTexture;
      return nullptr;
    }

    //cubeMapTextures.push_back(pTexture);

    return pTexture;
  }

  void cContext::DestroyTextureCubeMap(cTextureCubeMap* pTexture)
  {
    assert(pTexture != nullptr);

    //cubeMapTextures.remove(pTexture);

    pTexture->Destroy();
    delete pTexture;
  }


  cTextureFrameBufferObject* cContext::CreateTextureFrameBufferObject(size_t width, size_t height, PIXELFORMAT pixelFormat)
  {
    (void)pixelFormat;

    cTextureFrameBufferObject* pTexture = new cTextureFrameBufferObject;
    if (!pTexture->CreateFrameBufferObject(width, height)) {
      delete pTexture;
      return nullptr;
    }

    //textures.push_back(pTexture);

    return pTexture;
  }

  cTextureFrameBufferObject* cContext::CreateTextureFrameBufferObjectNoMipMaps(size_t width, size_t height, PIXELFORMAT pixelFormat)
  {
    (void)pixelFormat;

    cTextureFrameBufferObject* pTexture = new cTextureFrameBufferObject;
    pTexture->SetDoNotUseMipMaps();
    if (!pTexture->CreateFrameBufferObject(width, height)) {
      delete pTexture;
      return nullptr;
    }

    //textures.push_back(pTexture);

    return pTexture;
  }

  void cContext::DestroyTextureFrameBufferObject(cTextureFrameBufferObject* pTexture)
  {
    assert(pTexture != nullptr);

    //textures.remove(pTexture);

    pTexture->Destroy();
    delete pTexture;
  }


  cShader* cContext::CreateShader(const opengl::string_t& sVertexShaderFileName, const opengl::string_t& sFragmentShaderFileName)
  {
    cShader* pShader = new cShader;
    if (!pShader->LoadVertexShaderAndFragmentShader(sVertexShaderFileName, sFragmentShaderFileName)) {
      delete pShader;
      return nullptr;
    }

    shaders.push_back(pShader);

    return pShader;
  }

  void cContext::DestroyShader(cShader* pShader)
  {
    assert(pShader != nullptr);

    shaders.remove(pShader);

    pShader->Destroy();
    delete pShader;
  }

  cStaticVertexBufferObject* cContext::CreateStaticVertexBufferObject()
  {
    cStaticVertexBufferObject* pStaticVertexBufferObject = new cStaticVertexBufferObject;
    staticVertexBufferObjects.push_back(pStaticVertexBufferObject);
    return pStaticVertexBufferObject;
  }

  void cContext::DestroyStaticVertexBufferObject(cStaticVertexBufferObject* pStaticVertexBufferObject)
  {
    assert(pStaticVertexBufferObject != nullptr);

    staticVertexBufferObjects.remove(pStaticVertexBufferObject);

    pStaticVertexBufferObject->Destroy();
    delete pStaticVertexBufferObject;
  }


#ifdef BUILD_OPENGLMM_FONT
  cFont* cContext::CreateFont(const opengl::string_t& sFileName, size_t fontSize, const opengl::string_t& sVertexShader, const opengl::string_t& sFragmentShader)
  {
    cFont* pFont = new cFont;
    if (!pFont->Load(*this, sFileName, fontSize, sVertexShader, sFragmentShader)) {
      delete pFont;
      return nullptr;
    }

    fonts.push_back(pFont);

    return pFont;
  }

  void cContext::DestroyFont(cFont* pFont)
  {
    assert(pFont != nullptr);

    fonts.remove(pFont);

    pFont->Destroy(*this);
    delete pFont;
  }
#endif


  bool cContext::_SetWindowVideoMode(bool bIsFullScreen)
  {
    LOG<<"cContext::_SetWindowVideoMode "<<std::endl;
    assert(bIsRenderingToWindow);

    // Avoid a divide by zero
    if (resolution.height == 0) resolution.height = 1;


    // Destroy the old surface
    if (pSurface != nullptr) {
      SDL_FreeSurface(pSurface);
      pSurface = nullptr;
    }


    unsigned int uiFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;


    if (bIsFullScreen) {
      LOG<<"cContext::_SetWindowVideoMode fullscreen"<<std::endl;
      uiFlags |= SDL_FULLSCREEN;
    } else {
      LOG<<"cContext::_SetWindowVideoMode window"<<std::endl;
      uiFlags &= ~SDL_FULLSCREEN;
    }



    const SDL_VideoInfo* pVideoInfo = SDL_GetVideoInfo();
    if (pVideoInfo == nullptr) {
      LOGERROR<<"cContext::_SetWindowVideoMode SDL_GetVideoInfo FAILED error="<<SDL_GetError()<<std::endl;
      return false;
    }


    // This checks to see if surfaces can be stored in memory
    if (pVideoInfo->hw_available) {
      LOG<<"cContext::_SetWindowVideoMode Hardware surface"<<std::endl;
      uiFlags |= SDL_HWSURFACE;
      uiFlags &= ~SDL_SWSURFACE;
    } else {
      LOG<<"cContext::_SetWindowVideoMode Software surface"<<std::endl;
      uiFlags |= SDL_SWSURFACE;
      uiFlags &= ~SDL_HWSURFACE;
    }

    // This checks if hardware blits can be done
    if (pVideoInfo->blit_hw) {
      LOG<<"cContext::_SetWindowVideoMode Hardware blit"<<std::endl;
      uiFlags |= SDL_HWACCEL;
    } else {
      LOG<<"cContext::_SetWindowVideoMode Software blit"<<std::endl;
      uiFlags &= ~SDL_HWACCEL;
    }

    // Sets up OpenGL double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // We definitely want the OpenGL flag for SDL_SetVideoMode
    assert((uiFlags & SDL_OPENGL) != 0);

    #if (BUILD_LIBOPENGLMM_SDL_VERSION >= 130) || (BUILD_LIBOPENGLMM_OPENGL_VERSION >= 300)
    const int iMajor = BUILD_LIBOPENGLMM_OPENGL_VERSION / 100;
    const int iMinor = (BUILD_LIBOPENGLMM_OPENGL_VERSION % 100) / 10;
    #endif

    #if BUILD_LIBOPENGLMM_SDL_VERSION >= 130
    // For SDL 1.3 and above we can just request the OpenGL version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, iMajor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, iMinor);
    #elif BUILD_LIBOPENGLMM_OPENGL_VERSION >= 300
    // SDL 1.2 and lower doesn't support OpenGL 3.0 or later so we have to initialize it manually
    #ifdef __WIN__
    // Get a handle to the window
    SDL_SysWMinfo wmInfo = { 0 };
    SDL_GetWMInfo(&wmInfo);
    if (wmInfo.window == NULL) {
      LOGERROR<<"cContext::_SetWindowVideoMode SDL_GetWMInfo FAILED, returning false"<<std::endl;
      return false;
    }

    HDC hdc = GetDC(wmInfo.window);
    if (hdc == NULL) {
      LOGERROR<<"cContext::_SetWindowVideoMode GetDC FAILED, returning false"<<std::endl;
      return false;
    }
    #endif
    #endif

    // Create an SDL surface
    LOG<<"cContext::_SetWindowVideoMode Calling SDL_SetVideoMode"<<std::endl;
    pSurface = SDL_SetVideoMode(resolution.width, resolution.height, GetBitsForPixelFormat(resolution.pixelFormat), uiFlags);
    if (pSurface == nullptr) {
      LOGERROR<<"cContext::_SetWindowVideoMode SDL_SetVideoMode FAILED error="<<SDL_GetError()<<std::endl;
      return false;
    }

    #ifdef __WIN__
    if (gl3wInit()) {
      LOGERROR<<"cContext::_SetWindowVideoMode Failed to initialize OpenGL"<<std::endl;
      return false;
    }
    if (!gl3wIsSupported(iMajor, iMinor)) {
      LOGERROR<<"cContext::_SetWindowVideoMode OpenGL "<<spitfire::string::ToString(iMajor)<<"."<<spitfire::string::ToString(iMinor)<<" not supported"<<std::endl;
      return false;
    }
    #endif

    LOG<<"cContext::_SetWindowVideoMode glGetError="<<cSystem::GetErrorString()<<std::endl;

    #if BUILD_LIBOPENGLMM_SDL_VERSION < 130
    // SDL 1.2 and lower doesn't support OpenGL 3.0 or later so we have to initialize it manually here
    #if BUILD_LIBOPENGLMM_OPENGL_VERSION >= 300

    #ifdef __WIN__
    LOG<<"cContext::_SetWindowVideoMode Error before wglGetProcAddress "<<SDL_GetError()<<", "<<cSystem::GetErrorString()<<std::endl;
    // Load the wglCreateContextAttribsARB extension
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    LOG<<"cContext::_SetWindowVideoMode Error after wglGetProcAddress "<<SDL_GetError()<<", "<<cSystem::GetErrorString()<<std::endl;
    if (wglCreateContextAttribsARB == nullptr) {
      LOG<<"cContext::_SetWindowVideoMode wglCreateContextAttribsARB NOT FOUND, returning false"<<std::endl;
      return false;
    }

    if (wglCreateContextAttribsARB != nullptr) {
      // Create a new context, make it current and destroy the old one
      LOG<<"cContext::_SetWindowVideoMode Initializing OpenGL "<<iMajor<<"."<<iMinor<<std::endl;
      // Create the new context
      int attribList[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, iMajor,
        WGL_CONTEXT_MINOR_VERSION_ARB, iMinor,
        #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
        // Use the stricter OpenGL 3 core compatibility flag
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        #endif
        0
      };
      HGLRC ctx3 = wglCreateContextAttribsARB(hdc, NULL, attribList);
      LOG<<"cContext::_SetWindowVideoMode After wglCreateContextAttribsARB "<<SDL_GetError()<<", "<<cSystem::GetErrorString()<<std::endl;
      if (ctx3 == NULL) {
        LOGERROR<<"cContext::_SetWindowVideoMode wglCreateContextAttribsARB FAILED, returning false"<<std::endl;
        return false;
      }

      // Make the new context current
      wglMakeCurrent(hdc, ctx3);
      LOG<<"cContext::_SetWindowVideoMode After wglMakeCurrent "<<SDL_GetError()<<", "<<cSystem::GetErrorString()<<std::endl;

      // Destroy the old context
      //wglDeleteContext(wmInfo.hglrc);
      //LOG<<"cContext::_SetWindowVideoMode After wglDeleteContext "<<SDL_GetError()<<", "<<cSystem::GetErrorString()<<std::endl;
    }
    #else
    // SDL 1.2 and lower doesn't support OpenGL 3.0 or later so we have to initialize it manually here
    // http://encelo.netsons.org/2009/01/16/habemus-opengl-30/
    // Get a pointer to glXCreateContextAttribsARB
    typedef GLXContext (*PFNGLXCREATECONTEXTATTRIBSARBPROC)(Display* dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int* attrib_list);
    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((GLubyte*)"glXCreateContextAttribsARB");
    if (glXCreateContextAttribsARB == nullptr) {
      LOG<<"cContext::_SetWindowVideoMode glXCreateContextAttribsARB NOT FOUND, returning false"<<std::endl;
      return false;
    }

    if (glXCreateContextAttribsARB != nullptr) {
      // Create a new context, make it current and destroy the old one
      LOG<<"cContext::_SetWindowVideoMode Initializing OpenGL "<<iMajor<<"."<<iMinor<<std::endl;
      // Tell GLX which version of OpenGL we want
      //GLXContext ctx = glXGetCurrentContext();
      Display* dpy = glXGetCurrentDisplay();
      GLXDrawable draw = glXGetCurrentDrawable();
      GLXDrawable read = glXGetCurrentReadDrawable();
      int nelements = 0;
      GLXFBConfig* cfg = glXGetFBConfigs(dpy, 0, &nelements);
      int attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, iMajor,
        GLX_CONTEXT_MINOR_VERSION_ARB, iMinor,
        #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
        // Use the stricter OpenGL 3 core compatibility flag
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        #endif
        0
      };
      GLXContext ctx3 = glXCreateContextAttribsARB(dpy, *cfg, 0, 1, attribs);
      glXMakeContextCurrent(dpy, draw, read, ctx3);

      // Destroy the old context
      //glXDestroyContext(dpy, ctx);

      LOG<<"cContext::_SetWindowVideoMode glGetError="<<cSystem::GetErrorString()<<std::endl;
    }
    #endif

    #endif
    #endif

    LOG<<"cContext::_SetWindowVideoMode returning true, "<<SDL_GetError()<<", "<<cSystem::GetErrorString()<<std::endl;
    return true;
  }

  void cContext::_SetDefaultFlags()
  {
    // Setup rendering options
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glShadeModel(GL_SMOOTH);
    #endif

    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glEnable(GL_NORMALIZE);
    #endif

    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    #endif
  }

  void cContext::_SetPerspective(size_t width, size_t height)
  {
    //LOG<<"cContext::_SetPerspective "<<cSystem::GetErrorString()<<std::endl;
    // Setup our viewport
    glViewport(0, 0, (GLint)width, (GLint)height);

    // Set up projection matrix
    matProjection = CalculateProjectionMatrix(width, height, 45.0f);

    // Load identity matrix for the modelview matrix
    matModelView.LoadIdentity();
    //LOG<<"cContext::_SetPerspective returning, "<<cSystem::GetErrorString()<<std::endl;
  }

  spitfire::math::cMat4 cContext::CalculateProjectionMatrix() const
  {
    return CalculateProjectionMatrix(targetWidth, targetHeight, 45.0f);
  }

  spitfire::math::cMat4 cContext::CalculateProjectionMatrix(size_t width, size_t height, float fFOV) const
  {
    // Set our perspective
    assert(height != 0); // Protect against a divide by zero
    const GLfloat fRatio = (GLfloat)width / (GLfloat)height;
    const float fMaximumViewDistance = 1000.0f;
    spitfire::math::cMat4 matrix;
    matrix.SetPerspective(fFOV, fRatio, 0.1f, fMaximumViewDistance);
    return matrix;
  }

  spitfire::math::cMat4 cContext::CalculateProjectionMatrixRenderMode2D(MODE2D_TYPE type) const
  {
    spitfire::math::cMat4 matrix;

    // Our screen coordinates look like this
    // 0.0f, 0.0f            1.0f, 0.0f
    //
    //
    // 0.0f, 1.0f            1.0f, 1.0f

    if (type == MODE2D_TYPE::Y_INCREASES_DOWN_SCREEN) {
      // Use an aspect ratio of 1:1
      matrix.SetOrtho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f); // Y axis increases down the screen
    } else if (type == MODE2D_TYPE::Y_INCREASES_DOWN_SCREEN_KEEP_ASPECT_RATIO) {
      // Keep the aspect ratio of the screen, for example 16:9
      const float fRight = float(targetWidth) / float(targetHeight);
      matrix.SetOrtho(0.0f, fRight, 1.0f, 0.0f, -1.0f, 1.0f); // Y axis increases down the screen
    } else if (type == MODE2D_TYPE::Y_INCREASES_DOWN_SCREEN_KEEP_DIMENSIONS_AND_ASPECT_RATIO) {
      // Keep the width and height and aspect ratio of the screen, for example 1920x1080, 16:9
      matrix.SetOrtho(0.0f, float(targetWidth), float(targetHeight), 0.0f, -1.0f, 1.0f); // Y axis increases down the screen
    } else {
      // Use an aspect ratio of 1:1
      matrix.SetOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f); // Y axis increases up the screen
    }

    return matrix;
  }


  void cContext::ResizeWindow(const cResolution& _resolution)
  {
    assert(bIsRenderingToWindow);

    resolution = _resolution;

    if (!_SetWindowVideoMode(false)) {
      LOG<<"cContext::ResizeWindow Error setting video mode"<<std::endl;
      assert(false);
    }

    _SetPerspective(resolution.width, resolution.height);
  }

  void cContext::SetClearColour(const spitfire::math::cColour& _clearColour)
  {
    clearColour = _clearColour;
  }

  void cContext::SetAmbientColour(const spitfire::math::cColour& _ambientColour)
  {
    ambientColour = _ambientColour;
  }

  void cContext::SetSunPosition(const spitfire::math::cVec3& _sunPosition)
  {
    sunPosition = _sunPosition;
  }

  void cContext::SetSunAmbientColour(const spitfire::math::cColour& _sunAmbientColour)
  {
    sunAmbientColour = _sunAmbientColour;
  }

  void cContext::SetSunIntensity(float _fSunIntensity)
  {
    fSunIntensity = _fSunIntensity;
  }

  void cContext::_BeginRenderShared(size_t width, size_t height)
  {
    //LOG<<"cContext::_BeginRenderShared "<<cSystem::GetErrorString()<<std::endl;
    matTexture.LoadIdentity();

    targetWidth = width;
    targetHeight = height;

    _SetPerspective(width, height);

    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glClearDepth(1.0);
    #endif
    glEnable(GL_DEPTH_TEST);
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glDepthFunc(GL_LEQUAL);
    #endif

    glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    // Set our default colour
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    //const GLfloat global_ambient[] = { ambientColour.r, ambientColour.g, ambientColour.b, 1.0f };
    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    #endif

    //if (bIsFSAAEnabled) glEnable(GL_MULTISAMPLE_ARB);

    //if (bIsRenderWireframe) EnableWireframe();
    //else DisableWireframe();

    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_FILL);
    #endif
    DisableWireframe();
    //LOG<<"cContext::_BeginRenderShared returning, "<<cSystem::GetErrorString()<<std::endl;
  }

  void cContext::_EndRenderShared()
  {
    targetWidth = resolution.width;
    targetHeight = resolution.height;

    //if (bIsFSAAEnabled) glDisable(GL_MULTISAMPLE_ARB);
  }


  void cContext::BeginRenderToScreen()
  {
    _BeginRenderShared(resolution.width, resolution.height);
  }

  void cContext::EndRenderToScreen()
  {
    _EndRenderShared();

    if (bIsRenderingToWindow) SDL_GL_SwapBuffers();
  }

  void cContext::BeginRenderToTexture(cTextureFrameBufferObject& texture)
  {
    assert(texture.IsValid());
    assert(!texture.IsModeCubeMap()); // Cubemaps have to be rendered into each face separately

    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glEnable(GL_TEXTURE_2D);
    #endif

    // First we bind the FBO so we can render to it
    glBindFramebuffer(GL_FRAMEBUFFER, texture.uiFBO);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) LOG<<"cContext::BeginRenderToTexture Incomplete FBO "<<status<<std::endl;

    _BeginRenderShared(texture.GetWidth(), texture.GetHeight());
  }

  void cContext::EndRenderToTexture(cTextureFrameBufferObject& texture)
  {
    _EndRenderShared();

    // Set rendering back to default frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    texture.GenerateMipMapsIfRequired();

    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glDisable(GL_TEXTURE_2D);
    #endif
  }

  void cContext::BeginRenderMode2D(MODE2D_TYPE type)
  {
    (void)type;

    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glDisable(GL_LIGHTING);
    #endif
    glDisable(GL_DEPTH_TEST);
  }

  void cContext::EndRenderMode2D()
  {
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glEnable(GL_LIGHTING);
    #endif
    glEnable(GL_DEPTH_TEST);
  }

  void cContext::SetShaderProjectionAndModelViewMatricesRenderMode2D(MODE2D_TYPE type, const spitfire::math::cMat4& _matModelView)
  {
    assert(pCurrentShader != nullptr);

    spitfire::math::cMat4 _matProjection = CalculateProjectionMatrixRenderMode2D(type);
    SetShaderProjectionAndModelViewMatrices(_matProjection, _matModelView);
  }

  void cContext::SetShaderProjectionAndModelViewMatrices(const spitfire::math::cMat4& _matProjection, const spitfire::math::cMat4& _matModelView)
  {
    assert(pCurrentShader != nullptr);

    matProjection = _matProjection;
    matModelView = _matModelView;

    // Update the projection matrix on the shader if required
    if (pCurrentShader->bProjectionMatrix) SetShaderConstant("matProjection", matProjection);

    if (pCurrentShader->bModelViewMatrix) SetShaderConstant("matModelView", matModelView);

    if (pCurrentShader->bModelViewProjectionMatrix) {
      // Modelview projection matrix
      const spitfire::math::cMat4 matModelViewProjection(matProjection * matModelView);
      SetShaderConstant("matModelViewProjection", matModelViewProjection);
    }

    if (pCurrentShader->bNormalMatrix) {
      // Normal matrix
      // http://blog.tojicode.com/2012/07/the-webgl-guide-to-reading-opengl.html
      // http://www.gamedev.net/topic/503063-inverse-transpose-matrix-what-is-that/page__p__4281691#entry4281691
      const spitfire::math::cMat3 matNormal = spitfire::math::cMat3(matModelView).GetInverseTranspose();
      SetShaderConstant("matNormal", matNormal);
    }
  }

  //void cContext::SetTextureMatrix(const spitfire::math::cMat3& matrix)
  //{
  //  SetShaderConstant("matTexture", matTexture);
  //}

  void cContext::SetShaderLightEnabled(size_t light, bool bEnable)
  {
    (void)light;
    (void)bEnable;
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    if (bEnable) glEnable(GL_LIGHT0 + light);
    else glDisable(GL_LIGHT0 + light);
    #endif
  }

  void cContext::SetShaderLightType(size_t light, LIGHT_TYPE type)
  {
    (void)light;
    (void)type;
  }

  void cContext::SetShaderLightPosition(size_t light, const spitfire::math::cVec3& _position)
  {
    (void)light;
    (void)_position;
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    const GLfloat position[] = { _position.x, _position.y, _position.z, 0.0f };
    glLightfv(GL_LIGHT0 + light, GL_POSITION, position);
    #endif
  }

  void cContext::SetShaderLightRotation(size_t light, const spitfire::math::cQuaternion& rotation)
  {
    (void)light;
    (void)rotation;
  }

  void cContext::SetShaderLightAmbientColour(size_t light, const spitfire::math::cColour& colour)
  {
    (void)light;
    (void)colour;
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    const GLfloat ambient[] = { colour.r, colour.g, colour.b, 1.0f };
    glLightfv(GL_LIGHT0 + light, GL_AMBIENT, ambient);
    #endif
  }

  void cContext::SetShaderLightDiffuseColour(size_t light, const spitfire::math::cColour& colour)
  {
    (void)light;
    (void)colour;
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    const GLfloat diffuse[] = { colour.r, colour.g, colour.b, 1.0f };
    glLightfv(GL_LIGHT0 + light, GL_DIFFUSE, diffuse);
    #endif
  }

  void cContext::SetShaderLightSpecularColour(size_t light, const spitfire::math::cColour& colour)
  {
    (void)light;
    (void)colour;
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    const GLfloat specular[] = { colour.r, colour.g, colour.b, 1.0f };
    glLightfv(GL_LIGHT0 + light, GL_SPECULAR, specular);
    #endif
  }


  #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
  void cContext::EnableLighting()
  {
    glEnable(GL_LIGHTING);
  }

  void cContext::DisableLighting()
  {
    glDisable(GL_LIGHTING);
  }
  #endif


  void cContext::EnableDepthTesting()
  {
    glEnable(GL_DEPTH_TEST);
  }

  void cContext::DisableDepthTesting()
  {
    glDisable(GL_DEPTH_TEST);
  }

  void cContext::EnableDepthMasking()
  {
    glDepthMask(GL_TRUE);
  }

  void cContext::DisableDepthMasking()
  {
    glDepthMask(GL_FALSE);
  }

  void cContext::EnableAlphaTesting()
  {
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glEnable(GL_ALPHA_TEST);
    #endif
  }

  void cContext::DisableAlphaTesting()
  {
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glDisable(GL_ALPHA_TEST);
    #endif
  }

  void cContext::EnableBlending()
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  void cContext::DisableBlending()
  {
    glDisable(GL_BLEND);
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


#ifdef BUILD_OPENGLMM_FONT
  void cContext::BindFont(const cFont& font)
  {
    assert(font.IsValid());

    BindTexture(0, *(font.pTexture));
    BindShader(*(font.pShader));
  }

  void cContext::UnBindFont(const cFont& font)
  {
    assert(font.IsValid());

    UnBindShader(*(font.pShader));
    UnBindTexture(0, *(font.pTexture));
  }
#endif

  void cContext::BindTexture(size_t uTextureUnit, const cTexture& texture)
  {
    // Activate the current texture unit
    glActiveTexture(GL_TEXTURE0 + uTextureUnit);
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glClientActiveTexture(GL_TEXTURE0 + uTextureUnit);
    #endif

    GLenum type = GL_TEXTURE_2D;
    if (texture.GetWidth() != texture.GetHeight()) type = GL_TEXTURE_RECTANGLE;

    glBindTexture(type, texture.GetTexture());
  }

  void cContext::UnBindTexture(size_t uTextureUnit, const cTexture& texture)
  {
    // Activate the current texture unit
    glActiveTexture(GL_TEXTURE0 + uTextureUnit);

    GLenum type = GL_TEXTURE_2D;
    if (texture.GetWidth() != texture.GetHeight()) type = GL_TEXTURE_RECTANGLE;

    glBindTexture(type, 0);
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glDisable(type);
    #endif
  }


  void cContext::BindTextureCubeMap(size_t uTextureUnit, const cTextureCubeMap& texture)
  {
    // Activate the current texture unit
    glActiveTexture(GL_TEXTURE0 + uTextureUnit);
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glClientActiveTexture(GL_TEXTURE0 + uTextureUnit);
    #endif

    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glEnable(GL_TEXTURE_CUBE_MAP);
    #endif
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.GetTexture());
  }

  void cContext::UnBindTextureCubeMap(size_t uTextureUnit, const cTextureCubeMap& texture)
  {
    (void)texture;

    // Activate the current texture unit
    glActiveTexture(GL_TEXTURE0 + uTextureUnit);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glDisable(GL_TEXTURE_CUBE_MAP);
    #endif
  }


  void cContext::BindShader(cShader& shader)
  {
    assert(shader.IsCompiledProgram());

    assert(pCurrentShader == nullptr); // UnBindShader must be called first

    shader.Bind();

    pCurrentShader = &shader;

    // TODO: We also need some more variables within our post render shaders such as:
    //  - brightness: HDR, Top Gear Shader, Night Vision
    //  - exposure: HDR, Top Gear Shader

    if (shader.bTexUnit0) SetShaderConstant("texUnit0", 0);
    if (shader.bTexUnit1) SetShaderConstant("texUnit1", 1);
    if (shader.bTexUnit2) SetShaderConstant("texUnit2", 2);
    if (shader.bTexUnit3) SetShaderConstant("texUnit3", 3);

    if (shader.bAmbientColour) SetShaderConstant("ambientColour", ambientColour);
    if (shader.bSunAmbientColour) SetShaderConstant("sunAmbientColour", sunAmbientColour);
    if (shader.bSunPosition) SetShaderConstant("sunPosition", sunPosition);
    if (shader.bSunIntensity) SetShaderConstant("fSunIntensity", fSunIntensity);
  }

  void cContext::UnBindShader(cShader& shader)
  {
    assert(pCurrentShader == &shader);

    shader.UnBind();

    pCurrentShader = nullptr;
  }

  bool cContext::SetShaderConstant(const std::string& sConstant, int value)
  {
    GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
    if (loc == -1) {
      LOG<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
      assert(loc > 0);
      return false;
    }

    glUniform1i(loc, value);
    return true;
  }

  bool cContext::SetShaderConstant(const std::string& sConstant, float value)
  {
    GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
    if (loc == -1) {
      LOG<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
      assert(loc > 0);
      return false;
    }

    glUniform1f(loc, value);
    return true;
  }

  bool cContext::SetShaderConstant(const std::string& sConstant, const spitfire::math::cVec2& value)
  {
    GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
    if (loc == -1) {
      LOG<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
      assert(loc > 0);
      return false;
    }

    glUniform2f(loc, value.x, value.y);
    return true;
  }

  bool cContext::SetShaderConstant(const std::string& sConstant, const spitfire::math::cVec3& value)
  {
    GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
    if (loc == -1) {
      LOG<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
      assert(loc > 0);
      return false;
    }

    glUniform3f(loc, value.x, value.y, value.z);
    return true;
  }

  bool cContext::SetShaderConstant(const std::string& sConstant, const spitfire::math::cVec4& value)
  {
    GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
    if (loc == -1) {
      LOG<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
      assert(loc > 0);
      return false;
    }

    glUniform4f(loc, value.x, value.y, value.z, value.w);
    return true;
  }

  bool cContext::SetShaderConstant(const std::string& sConstant, const spitfire::math::cColour& value)
  {
    return SetShaderConstant(sConstant, spitfire::math::cVec4(value.r, value.g, value.b, value.a));
  }

  bool cContext::SetShaderConstant(const std::string& sConstant, const spitfire::math::cMat3& matrix)
  {
    GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
    if (loc == -1) {
      LOG<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
      assert(loc > 0);
      return false;
    }

    glUniformMatrix3fv(loc, 1, GL_FALSE, matrix.GetOpenGLMatrixPointer());
    return true;
  }

  bool cContext::SetShaderConstant(const std::string& sConstant, const spitfire::math::cMat4& matrix)
  {
    GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
    if (loc == -1) {
      LOG<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
      assert(loc > 0);
      return false;
    }

    glUniformMatrix4fv(loc, 1, GL_FALSE, matrix.GetOpenGLMatrixPointer());
    return true;
  }


  // ** cStaticVertexBufferObject

  void cContext::BindStaticVertexBufferObject(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.Bind();
  }

  void cContext::UnBindStaticVertexBufferObject(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.Unbind();
  }

  void cContext::BindStaticVertexBufferObject2D(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.Bind2D();
  }

  void cContext::UnBindStaticVertexBufferObject2D(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.Unbind2D();
  }


  void cContext::DrawStaticVertexBufferObjectLines(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderLines();
  }

  void cContext::DrawStaticVertexBufferObjectTriangles(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderTriangles();
  }

  void cContext::DrawStaticVertexBufferObjectTriangleStrip(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderTriangleStrip();
  }

  #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
  void cContext::DrawStaticVertexBufferObjectQuads(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderQuads();
  }

  void cContext::DrawStaticVertexBufferObjectQuadStrip(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderQuadStrip();
  }
  #endif


  void cContext::DrawStaticVertexBufferObjectLines2D(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderLines2D();
  }

  void cContext::DrawStaticVertexBufferObjectTriangles2D(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderTriangles2D();
  }

  #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
  void cContext::DrawStaticVertexBufferObjectQuads2D(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderQuads2D();
  }
  #endif
}
