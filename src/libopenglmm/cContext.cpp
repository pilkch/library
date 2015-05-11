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
#include <SDL2/SDL_image.h>

// Spitfire headers
#include <spitfire/util/log.h>
#include <spitfire/util/string.h>

// libopenglmm headers
#include <libopenglmm/cContext.h>
#ifdef BUILD_LIBOPENGLMM_FONT
#include <libopenglmm/cFont.h>
#endif
#include <libopenglmm/cShader.h>
#include <libopenglmm/cSystem.h>
#include <libopenglmm/cTexture.h>
#include <libopenglmm/cVertexBufferObject.h>
#include <libopenglmm/cWindow.h>
#include <libopenglmm/opengl.h>

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
  #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
  cContext::cContext(cSystem& _system, cWindow& window) :
    system(_system),
    bIsRenderingToWindow(true),
    bIsValid(false),
    resolution(window.GetResolution()),
    context(nullptr),
    pSurface(nullptr),
    targetWidth(0),
    targetHeight(0),
    clearColour(0.0f, 0.0f, 0.0f, 1.0f),
    ambientColour(1.0f, 1.0f, 1.0f, 1.0f),
    sunAmbientColour(1.0f, 1.0f, 1.0f, 1.0f),
    fSunIntensity(0.0f),
    pCurrentShader(nullptr)
  {
    LOG("With window ");

    if (!_SetWindowVideoMode(window, window.IsFullScreen())) {
      LOG("Error setting video mode");
      assert(false);
    }

    _SetDefaultFlags();
    _SetPerspective(resolution.width, resolution.height);
    targetWidth = resolution.width;
    targetHeight = resolution.height;


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
            LOG("Requested sample level is ", uiFSAASampleLevel, ", actual sample level is ", uiActualSampleLevel);
          }

          if ((uiActualSampleLevel == 2) || (uiActualSampleLevel == 4) || (uiActualSampleLevel == 8) || (uiActualSampleLevel == 16)) bIsFSAAEnabled = true;
      }
    }*/

    bIsValid = true;

    LOG("With window returning, ", cSystem::GetErrorString());
  }
  #endif

  cContext::cContext(cSystem& _system, const cResolution& _resolution, bool _bIsRenderingToWindow) :
    system(_system),
    bIsRenderingToWindow(_bIsRenderingToWindow),
    bIsValid(false),
    resolution(_resolution),
    context(nullptr),
    #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
    pSurface(nullptr),
    #endif
    targetWidth(0),
    targetHeight(0),
    clearColour(0.0f, 0.0f, 0.0f, 1.0f),
    ambientColour(1.0f, 1.0f, 1.0f, 1.0f),
    pCurrentShader(nullptr)
  {
    LOG("");

    // Get the current context
    context = SDL_GL_GetCurrentContext();

    _SetDefaultFlags();
    _SetPerspective(resolution.width, resolution.height);
    targetWidth = resolution.width;
    targetHeight = resolution.height;

    bIsValid = true;
  }

  cContext::~cContext()
  {
    LOG("");

    assert(pCurrentShader == nullptr); // Shaders must be unbound before this point

#ifdef BUILD_LIBOPENGLMM_FONT
    assert(fonts.empty());
#endif
    assert(textures.empty());
    assert(cubeMapTextures.empty());
    assert(shaders.empty());
    assert(staticVertexBufferObjects.empty());

    #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
    // Destroy the surface
    if (pSurface != nullptr) {
      SDL_FreeSurface(pSurface);
      pSurface = nullptr;
    }
    #endif

    // Destroy the context
    if (context != nullptr) {
      SDL_GL_DeleteContext(context);
      context = nullptr;
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

  void cContext::CreateStaticVertexBufferObject(cStaticVertexBufferObject& vbo)
  {
    staticVertexBufferObjects.push_back(&vbo);
  }

  void cContext::DestroyStaticVertexBufferObject(cStaticVertexBufferObject& vbo)
  {
    staticVertexBufferObjects.remove(&vbo);

    vbo.Destroy();
  }


#ifdef BUILD_LIBOPENGLMM_FONT
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


  #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
  bool cContext::_SetWindowVideoMode(cWindow& window, bool bIsFullScreen)
  {
    LOG("");
    assert(bIsRenderingToWindow);

    // Avoid a divide by zero
    if (resolution.height == 0) resolution.height = 1;


    // Destroy the old surface
    if (pSurface != nullptr) {
      SDL_FreeSurface(pSurface);
      pSurface = nullptr;
    }

    // Destroy the old context
    if (context != nullptr) {
      SDL_GL_DeleteContext(context);
      context = nullptr;
    }

    unsigned int uiFlags = SDL_WINDOW_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
    //uiFlags |= SDL_HWPALETTE;

    // Create a window that is fullscreen with a custom resolution
    if (bIsFullScreen) {
      LOG("fullscreen");
      uiFlags |= SDL_WINDOW_FULLSCREEN;
    } else {
      LOG("window");
      uiFlags &= ~SDL_WINDOW_FULLSCREEN;
    }

    // Set up OpenGL double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    const int iMajor = BUILD_LIBOPENGLMM_OPENGL_VERSION / 100;
    const int iMinor = (BUILD_LIBOPENGLMM_OPENGL_VERSION % 100) / 10;

    // For SDL 1.3 and above we can just request the OpenGL version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, iMajor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, iMinor);
    #ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    #endif

    #ifdef BUILD_DEBUG
    // For debugging
    //const int iDisplay = SDL_GetNumVideoDisplays(); // Assume that the last monitor is the desired one (Works for most second monitor setups where a TV or larger monitor is the second or third device)
    const int iDisplay = 1;
    #else
    // TODO: Allow selection of the monitor or use the default or same monitor that the mouse is currently on?
    const int iDisplay = 1;
    #endif

    // Create our window if it has not already been created
    LOG("Calling SDL_CreateWindow");
    const bool bCreated = (window.pWindow != nullptr);
    if (!bCreated) {
      window.pWindow = SDL_CreateWindow("My Game Window",
        SDL_WINDOWPOS_CENTERED_DISPLAY(iDisplay), SDL_WINDOWPOS_CENTERED_DISPLAY(iDisplay),
        int(resolution.width), int(resolution.height),
        uiFlags);
      // Create a fullscreen window with the current resolution
      //window.pWindow = SDL_CreateWindow(title,
      //                        SDL_WINDOWPOS_CENTERED_DISPLAY(iDisplay), SDL_WINDOWPOS_CENTERED_DISPLAY(iDisplay),
      //                        0, 0,
      //                        SDL_WINDOW_FULLSCREEN_DESKTOP);

      if (window.pWindow == nullptr) {
        LOGERROR("cContext::_SetWindowVideoMode SDL_CreateWindow FAILED error=", SDL_GetError());
        return false;
      }
    }

    context = SDL_GL_CreateContext(window.pWindow);
    if (context == nullptr) {
      LOGERROR("cContext::_SetWindowVideoMode SDL_GL_CreateContext FAILED error=", SDL_GetError());
      return false;
    }

    #ifdef __WIN__
    if (gl3wInit()) {
      LOGERROR("cContext::_SetWindowVideoMode Failed to initialize OpenGL");
      return false;
    }
    if (!gl3wIsSupported(iMajor, iMinor)) {
      LOGERROR(TEXT("cContext::_SetWindowVideoMode OpenGL "), spitfire::string::ToString(iMajor), TEXT("."), spitfire::string::ToString(iMinor), TEXT(" not supported"));
      return false;
    }
    #endif


    #ifdef BUILD_DEBUG
    LOG("OpenGL Version: ", (const char*)(glGetString(GL_VERSION)));
    LOG("OpenGL Vendor: ", (const char*)(glGetString(GL_VENDOR)));
    LOG("OpenGL Renderer: ", (const char*)(glGetString(GL_RENDERER)));
    LOG("GLSL Version: ", (const char*)(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    #endif

    LOG("returning true, ", SDL_GetError(), ", ", cSystem::GetErrorString());

    return true;
  }
  #endif

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
    //LOG(cSystem::GetErrorString());
    // Setup our viewport
    glViewport(0, 0, (GLint)width, (GLint)height);

    // Set up projection matrix
    matProjection = CalculateProjectionMatrix(width, height, 45.0f);

    // Load identity matrix for the modelview matrix
    matModelView.LoadIdentity();
    //LOG("returning, ", cSystem::GetErrorString());
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
    } else if (type == MODE2D_TYPE::Y_INCREASES_UP_SCREEN_KEEP_ASPECT_RATIO) {
      // Keep the aspect ratio of the screen, for example 16:9
      const float fRight = float(targetWidth) / float(targetHeight);
      matrix.SetOrtho(0.0f, fRight, 0.0f, 1.0f, -1.0f, 1.0f); // Y axis increases up the screen
    } else {
      // Use an aspect ratio of 1:1
      matrix.SetOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f); // Y axis increases up the screen
    }

    return matrix;
  }

  #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
  void cContext::ResizeWindow(cWindow& window, const cResolution& _resolution)
  #else
  void cContext::ResizeWindow(const cResolution& _resolution)
  #endif
  {
    assert(bIsRenderingToWindow);

    resolution = _resolution;

    #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
    if (!_SetWindowVideoMode(window, false)) {
      LOG("Error setting video mode");
      assert(false);
    }
    #endif

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
    //LOG(cSystem::GetErrorString());
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
    //LOG("returning, ", cSystem::GetErrorString());
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

  #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
  void cContext::EndRenderToScreen(cWindow& window)
  #else
  void cContext::EndRenderToScreen()
  #endif
  {
    _EndRenderShared();

    #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
    if (bIsRenderingToWindow) window.SwapWindowFromContext();
    #endif
  }

  void cContext::BeginRenderToTexture(cTextureFrameBufferObject& texture)
  {
    //LOG(cSystem::GetErrorString());
    assert(texture.IsValid());
    assert(!texture.IsModeCubeMap()); // Cubemaps have to be rendered into each face separately

    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glEnable(GL_TEXTURE_2D);
    #endif

    // First we bind the FBO so we can render to it
    glBindFramebuffer(GL_FRAMEBUFFER, texture.uiFBO);

    //LOG(cSystem::GetErrorString());

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) LOG("Incomplete FBO ", status);

    //LOG(cSystem::GetErrorString());

    _BeginRenderShared(texture.GetWidth(), texture.GetHeight());

    //LOG(cSystem::GetErrorString());
  }

  void cContext::EndRenderToTexture(cTextureFrameBufferObject& texture)
  {
    //LOG(cSystem::GetErrorString());

    _EndRenderShared();

    //LOG(cSystem::GetErrorString());

    // Set rendering back to default frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //LOG(cSystem::GetErrorString());

    texture.GenerateMipMapsIfRequired();

    //LOG(cSystem::GetErrorString());

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


#ifdef BUILD_LIBOPENGLMM_FONT
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
    //LOG(cSystem::GetErrorString());

    // Activate the current texture unit
    glActiveTexture(GL_TEXTURE0 + int(uTextureUnit));
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    glClientActiveTexture(GL_TEXTURE0 + uTextureUnit);
    #endif

    GLenum type = GL_TEXTURE_2D;
    if (texture.GetWidth() != texture.GetHeight()) type = GL_TEXTURE_RECTANGLE;

    glBindTexture(type, texture.GetTexture());

    //LOG(cSystem::GetErrorString()));
  }

  void cContext::UnBindTexture(size_t uTextureUnit, const cTexture& texture)
  {
    // Activate the current texture unit
    glActiveTexture(GL_TEXTURE0 + int(uTextureUnit));

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
    glActiveTexture(GL_TEXTURE0 + int(uTextureUnit));
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
    glActiveTexture(GL_TEXTURE0 + int(uTextureUnit));

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
      LOG(TEXT("cContext::SetShaderConstant \""), pCurrentShader->sShaderVertex, TEXT("\":\""), pCurrentShader->IsCompiledFragment(), TEXT("\" Couldn't set \""), spitfire::string::ToString_t(sConstant), TEXT("\" perhaps the constant is not actually used within the shader"));
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
      LOG(TEXT("cContext::SetShaderConstant \""), pCurrentShader->sShaderVertex, TEXT("\":\""), pCurrentShader->IsCompiledFragment(), TEXT("\" Couldn't set \""), spitfire::string::ToString_t(sConstant), TEXT("\" perhaps the constant is not actually used within the shader"));
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
      LOG("\"", pCurrentShader->sShaderVertex, "\":\"", pCurrentShader->IsCompiledFragment(), "\" Couldn't set \"", spitfire::string::ToString_t(sConstant), "\" perhaps the constant is not actually used within the shader");
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
      LOG(TEXT("cContext::SetShaderConstant \""), pCurrentShader->sShaderVertex, TEXT("\":\""), pCurrentShader->IsCompiledFragment(), TEXT("\" Couldn't set \""), spitfire::string::ToString_t(sConstant), TEXT("\" perhaps the constant is not actually used within the shader"));
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
      LOG(TEXT("cContext::SetShaderConstant \""), pCurrentShader->sShaderVertex, TEXT("\":\""), pCurrentShader->IsCompiledFragment(), TEXT("\" Couldn't set \""), spitfire::string::ToString_t(sConstant), TEXT("\" perhaps the constant is not actually used within the shader"));
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
      LOG(TEXT("cContext::SetShaderConstant \""), pCurrentShader->sShaderVertex, TEXT("\":\""), pCurrentShader->IsCompiledFragment(), TEXT("\" Couldn't set \""), spitfire::string::ToString_t(sConstant), TEXT("\" perhaps the constant is not actually used within the shader"));
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
      LOG(TEXT("cContext::SetShaderConstant \""), pCurrentShader->sShaderVertex, TEXT("\":\""), pCurrentShader->IsCompiledFragment(), TEXT("\" Couldn't set \""), spitfire::string::ToString_t(sConstant), TEXT("\" perhaps the constant is not actually used within the shader"));
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
