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
#include <GL/glx.h>

#define GLX_CONTEXT_DEBUG_BIT_ARB                0x00000001
#define GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB   0x00000002
#define GLX_CONTEXT_MAJOR_VERSION_ARB            0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB            0x2092
#define GLX_CONTEXT_FLAGS_ARB                    0x2094
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
    clearColour(0.0f, 0.0f, 0.0f, 1.0f),
    ambientColour(1.0f, 1.0f, 1.0f, 1.0f),
    sunAmbientColour(1.0f, 1.0f, 1.0f, 1.0f),
    fSunIntensity(0.0f),
    pCurrentShader(nullptr)
  {
    std::cout<<"cContext::cContext with window"<<std::endl;

    if (!_SetWindowVideoMode(window.IsFullScreen())) {
      std::cout<<"cContext::cContext Error setting video mode"<<std::endl;
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
            std::cout<<"cSystem::Create Requested sample level is "<<uiFSAASampleLevel<<", actual sample level is "<<uiActualSampleLevel<<std::endl;
          }

          if ((uiActualSampleLevel == 2) || (uiActualSampleLevel == 4) || (uiActualSampleLevel == 8) || (uiActualSampleLevel == 16)) bIsFSAAEnabled = true;
      }
    }*/

    bIsValid = true;
  }

  cContext::cContext(cSystem& _system, const cResolution& _resolution) :
    system(_system),
    bIsRenderingToWindow(false),
    bIsValid(false),
    resolution(_resolution),
    pSurface(nullptr),
    clearColour(0.0f, 0.0f, 0.0f, 1.0f),
    ambientColour(1.0f, 1.0f, 1.0f, 1.0f),
    pCurrentShader(nullptr)
  {
    std::cout<<"cContext::cContext"<<std::endl;

    _SetDefaultFlags();
    _SetPerspective(resolution.width, resolution.height);
  }

  cContext::~cContext()
  {
    std::cout<<"cContext::~cContext"<<std::endl;

    assert(pCurrentShader == nullptr); // Shaders must be unbound before this point

#ifdef BUILD_OPENGLMM_FONT
    assert(fonts.empty());
#endif
    assert(textures.empty());
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

  void cContext::DestroyTexture(cTexture* pTexture)
  {
    assert(pTexture != nullptr);

    //textures.remove(pTexture);

    pTexture->Destroy();
    delete pTexture;
  }


  cTextureFrameBufferObject* cContext::CreateTextureFrameBufferObject(size_t width, size_t height, PIXELFORMAT pixelFormat)
  {
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
      std::cout<<"cContext::_SetWindowVideoMode fullscreen"<<std::endl;
      uiFlags |= SDL_FULLSCREEN;
    } else {
      std::cout<<"cContext::_SetWindowVideoMode window"<<std::endl;
      uiFlags &= ~SDL_FULLSCREEN;
    }



    const SDL_VideoInfo* pVideoInfo = SDL_GetVideoInfo();
    if (pVideoInfo == nullptr) {
      std::cout<<"cContext::_SetWindowVideoMode SDL_GetVideoInfo FAILED error="<<SDL_GetError()<<std::endl;
      return false;
    }


    // This checks to see if surfaces can be stored in memory
    if (pVideoInfo->hw_available) {
      std::cout<<"cContext::_SetWindowVideoMode Hardware surface"<<std::endl;
      uiFlags |= SDL_HWSURFACE;
      uiFlags &= ~SDL_SWSURFACE;
    } else {
      std::cout<<"cContext::_SetWindowVideoMode Software surface"<<std::endl;
      uiFlags |= SDL_SWSURFACE;
      uiFlags &= ~SDL_HWSURFACE;
    }

    // This checks if hardware blits can be done
    if (pVideoInfo->blit_hw) {
      std::cout<<"cContext::_SetWindowVideoMode Hardware blit"<<std::endl;
      uiFlags |= SDL_HWACCEL;
    } else {
      std::cout<<"cContext::_SetWindowVideoMode Software blit"<<std::endl;
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
    // SDL 1.2 and lower don't support OpenGL 3.0 or later so we have to initialize it manually here
    // http://encelo.netsons.org/2009/01/16/habemus-opengl-30/
    // Get a pointer to glXCreateContextAttribsARB
    typedef GLXContext (*PFNGLXCREATECONTEXTATTRIBSARBPROC)(Display* dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int* attrib_list);
    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((GLubyte*)"glXCreateContextAttribsARB");
    if (glXCreateContextAttribsARB == nullptr) {
      std::cerr<<"cContext::_SetWindowVideoMode glXCreateContextAttribsARB NOT FOUND, returning false"<<std::endl;
      return false;
    }
    #endif

    // Create an SDL surface
    std::cout<<"cContext::_SetWindowVideoMode Calling SDL_SetVideoMode"<<std::endl;
    pSurface = SDL_SetVideoMode(resolution.width, resolution.height, GetBitsForPixelFormat(resolution.pixelFormat), uiFlags);
    if (pSurface == nullptr) {
      std::cout<<"cContext::_SetWindowVideoMode SDL_SetVideoMode FAILED error="<<SDL_GetError()<<std::endl;
      return false;
    }

    std::cout<<"cContext::_SetWindowVideoMode glGetError="<<cSystem::GetErrorString()<<std::endl;

    #if BUILD_LIBOPENGLMM_SDL_VERSION < 130
    // SDL 1.2 and lower don't support OpenGL 3.0 or later so we have to initialize it manually here
    #if BUILD_LIBOPENGLMM_OPENGL_VERSION >= 300
    if (glXCreateContextAttribsARB != nullptr) {
      // Create a new context, make it current and destroy the old one
      std::cout<<"cContext::_SetWindowVideoMode Initializing OpenGL "<<iMajor<<"."<<iMinor<<std::endl;
      // Tell GLX which version of OpenGL we want
      GLXContext ctx = glXGetCurrentContext();
      Display* dpy = glXGetCurrentDisplay();
      GLXDrawable draw = glXGetCurrentDrawable();
      GLXDrawable read = glXGetCurrentReadDrawable();
      int nelements = 0;
      GLXFBConfig* cfg = glXGetFBConfigs(dpy, 0, &nelements);
      int attribs[]= {
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
      glXDestroyContext(dpy, ctx);

      std::cout<<"cContext::_SetWindowVideoMode glGetError="<<cSystem::GetErrorString()<<std::endl;
    }
    #endif
    #endif

    return true;
  }

  void cContext::_SetDefaultFlags()
  {
    // Setup rendering options
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    glShadeModel(GL_SMOOTH);
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
    // Setup our viewport
    glViewport(0, 0, (GLint)width, (GLint)height);

    // Set up projection matrix
    matProjection = CalculateProjectionMatrix(width, height, 45.0f);

    // Load identity matrix for the modelview matrix
    matModelView.LoadIdentity();
  }

  spitfire::math::cMat4 cContext::CalculateProjectionMatrix() const
  {
    return CalculateProjectionMatrix(resolution.width, resolution.height, 45.0f);
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
      matrix.SetOrtho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f); // Y axis increases down the screen
    } else {
      matrix.SetOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f); // Y axis increases up the screen
    }

    return matrix;
  }


  void cContext::ResizeWindow(const cResolution& _resolution)
  {
    assert(bIsRenderingToWindow);

    resolution = _resolution;

    if (!_SetWindowVideoMode(false)) {
      std::cout<<"cContext::ResizeWindow Error setting video mode"<<std::endl;
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
    matTexture.LoadIdentity();

    _SetPerspective(width, height);


    //glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LEQUAL);

    glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set our default colour
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    //const GLfloat global_ambient[] = { ambientColour.r, ambientColour.g, ambientColour.b, 1.0f };
    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    //if (bIsFSAAEnabled) glEnable(GL_MULTISAMPLE_ARB);

    //if (bIsRenderWireframe) EnableWireframe();
    //else DisableWireframe();

    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_FILL);
  }

  void cContext::_EndRenderShared()
  {
    //if (bIsFSAAEnabled) glDisable(GL_MULTISAMPLE_ARB);
  }


  void cContext::BeginRendering()
  {
    _BeginRenderShared(resolution.width, resolution.height);
  }

  void cContext::EndRendering()
  {
    _EndRenderShared();

    if (bIsRenderingToWindow) SDL_GL_SwapBuffers();
  }

  void cContext::BeginRenderToTexture(cTextureFrameBufferObject& texture)
  {
    assert(texture.IsValid());
    assert(!texture.IsModeCubeMap()); // Cubemaps have to be rendered into each face separately

    glEnable(GL_TEXTURE_2D);

    // First we bind the FBO so we can render to it
    glBindFramebuffer(GL_FRAMEBUFFER, texture.uiFBO);

    // Save the view port settings (It is about to be change in _SetPerspective which is called by _BeginRenderShared)
    glPushAttrib(GL_VIEWPORT_BIT);

    _BeginRenderShared(texture.GetWidth(), texture.GetHeight());
  }

  void cContext::EndRenderToTexture(cTextureFrameBufferObject& texture)
  {
    _EndRenderShared();

    // Restore old view port settings and set rendering back to default frame buffer
    glPopAttrib();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    texture.GenerateMipMapsIfRequired();

    glDisable(GL_TEXTURE_2D);
  }

  void cContext::BeginRenderMode2D(MODE2D_TYPE type)
  {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
  }

  void cContext::EndRenderMode2D()
  {
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
  }

  void cContext::SetShaderProjectionAndModelViewMatricesRenderMode2D(MODE2D_TYPE type, const spitfire::math::cMat4& _matModelView)
  {
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
    if (bEnable) glEnable(GL_LIGHT0 + light);
    else glDisable(GL_LIGHT0 + light);
  }

  void cContext::SetShaderLightType(size_t light, LIGHT_TYPE type)
  {
  }

  void cContext::SetShaderLightPosition(size_t light, const spitfire::math::cVec3& _position)
  {
    const GLfloat position[] = { _position.x, _position.y, _position.z, 0.0f };
    glLightfv(GL_LIGHT0 + light, GL_POSITION, position);
  }

  void cContext::SetShaderLightRotation(size_t light, const spitfire::math::cQuaternion& rotation)
  {
  }

  void cContext::SetShaderLightAmbientColour(size_t light, const spitfire::math::cColour& colour)
  {
    const GLfloat ambient[] = { colour.r, colour.g, colour.b, 1.0f };
    glLightfv(GL_LIGHT0 + light, GL_AMBIENT, ambient);
  }

  void cContext::SetShaderLightDiffuseColour(size_t light, const spitfire::math::cColour& colour)
  {
    const GLfloat diffuse[] = { colour.r, colour.g, colour.b, 1.0f };
    glLightfv(GL_LIGHT0 + light, GL_DIFFUSE, diffuse);
  }

  void cContext::SetShaderLightSpecularColour(size_t light, const spitfire::math::cColour& colour)
  {
    const GLfloat specular[] = { colour.r, colour.g, colour.b, 1.0f };
    glLightfv(GL_LIGHT0 + light, GL_SPECULAR, specular);
  }


  void cContext::EnableLighting()
  {
    glEnable(GL_LIGHTING);
  }

  void cContext::DisableLighting()
  {
    glDisable(GL_LIGHTING);
  }


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
    glEnable(GL_ALPHA_TEST);
  }

  void cContext::DisableAlphaTesting()
  {
    glDisable(GL_ALPHA_TEST);
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
    glClientActiveTexture(GL_TEXTURE0 + uTextureUnit);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.GetTexture());
  }

  void cContext::UnBindTexture(size_t uTextureUnit, const cTexture& texture)
  {
    // Activate the current texture unit
    glActiveTexture(GL_TEXTURE0 + uTextureUnit);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
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
      std::cout<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
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
      std::cout<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
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
      std::cout<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
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
      std::cout<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
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
      std::cout<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
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
      std::cout<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
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
      std::cout<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
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

  #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
  void cContext::DrawStaticVertexBufferObjectQuads2D(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderQuads2D();
  }
  #endif
}
