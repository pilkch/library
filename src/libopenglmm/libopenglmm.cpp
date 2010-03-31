// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

// OpenGL headers
#include <GL/GLee.h>
#include <GL/glu.h>

// SDL headers
#include <SDL/SDL_image.h>

// libopenglmm headers
#include <libopenglmm/libopenglmm.h>

namespace opengl
{
  size_t GetBitsForPixelFormat(PIXELFORMAT pixelFormat)
  {
    if (pixelFormat == PIXELFORMAT::R8G8B8A8) return 32;
    if (pixelFormat == PIXELFORMAT::R8G8B8) return 24;

    return 16;
  }

  PIXELFORMAT GetPixelFormatFromSDLPixelFormat(const SDL_PixelFormat* pPixelFormat)
  {
    assert(pPixelFormat != nullptr);

    if ((pPixelFormat->BitsPerPixel == 32) && (pPixelFormat->BytesPerPixel == 4)) return PIXELFORMAT::R8G8B8A8;
    if ((pPixelFormat->BitsPerPixel == 24) && (pPixelFormat->BytesPerPixel == 3)) return PIXELFORMAT::R8G8B8;
    if ((pPixelFormat->BitsPerPixel == 16) && (pPixelFormat->BytesPerPixel == 3)) return PIXELFORMAT::R5G6B5;

    assert(false);
    return PIXELFORMAT::R5G6B5;
  }

  // *** cResolution

  cResolution::cResolution() :
    width(0),
    height(0),
    pixelFormat(PIXELFORMAT::R8G8B8A8)
  {
  }

  // *** cCapabilities

  void cCapabilities::Clear()
  {
    bIsOpenGLTwoPointZeroOrLaterSupported = false;
    bIsOpenGLThreePointZeroOrLaterSupported = false;
    bIsShadersTwoPointZeroOrLaterSupported = false;
    bIsVertexBufferObjectSupported = false;
    bIsFrameBufferObjectSupported = false;
    bIsShadowsSupported = false;
    bIsCubemappingSupported = false;
    nTextureUnits = 0;
    iMaxTextureSize = 0;

    bIsFSAASupported = false;
    nMaxFSAALevels = 0;
  }


  // *** cSystem

  cSystem::cSystem()
  {
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0) {
      std::cout<<"cSystem::cSystem SDL_Init FAILED error="<<SDL_GetError()<<std::endl;
      return;
    }

    UpdateResolutions();
  }

  cSystem::~cSystem()
  {
    SDL_Quit();
  }

  bool cSystem::FindExtension(const std::string& sExt) const
  {
    std::ostringstream t;
    t<<const_cast<const unsigned char*>(glGetString(GL_EXTENSIONS));

    return (t.str().find(sExt) != std::string::npos);
  }

  float cSystem::GetShaderVersion() const
  {
    float fGLVersion = 0.0f;
    {
      const std::string sVersion((const char*)glGetString(GL_VERSION));
      std::istringstream i(sVersion);
      i>>fGLVersion;
    }

    float fGLSLVersion = 0.0f;
    if (fGLVersion >= 2.0f) {
      const char* ptr = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

      if (ptr != nullptr) {
        std::string temp(ptr);

        std::istringstream stm(temp);
        stm>>fGLSLVersion;
      }
    }

    // Shader Model 4.0
    if (fGLSLVersion >= 4.0f) return fGLSLVersion;
    if (FindExtension("GL_NV_gpu_program4") || FindExtension("GL_NV_geometry_program4") ||
      FindExtension("GL_NV_vertex_program4") || FindExtension("GL_NV_fragment_program4") ||
      FindExtension("GL_EXT_gpu_shader4") || FindExtension("GL_EXT_geometry_shader4")) return 4.0f;

    // Shader Model 3.0
    if (fGLSLVersion >= 3.0f) return fGLSLVersion;
    if (FindExtension("GL_NV_vertex_program3") || FindExtension("GL_NV_fragment_program2") ||
      FindExtension("GL_ATI_shader_texture_lod")) return 3.0f;

    // Shader Model 2.0
    if (fGLSLVersion >= 2.0f) return fGLSLVersion;

    // Shader Model 1.0
    if (fGLSLVersion >= 1.0f) return fGLSLVersion;
    if (FindExtension("GL_ARB_shading_language_100")) return 1.0f;

    // Shader Model before 1.0
    return 0.0f;
  }

  void cSystem::UpdateResolutions()
  {
    const SDL_VideoInfo* pVideoInfo = SDL_GetVideoInfo();
    if ((pVideoInfo == nullptr) && (pVideoInfo->vfmt == nullptr)) {
      std::cout<<"cSystem::UpdateResolutions SDL_GetVideoInfo FAILED error="<<SDL_GetError()<<std::endl;
      return;
    }

    //for each resolution found {
    //  cResolution resolution;
    //  resolution.width = 1024;
    //  resolution.height = 768;
    //  resolution.pixelFormat = PIXELFORMAT::R8G8B8A8;
    //
    //  capabilities.AddResolution(resolution);
    //}

    cResolution resolution;
    resolution.width = pVideoInfo->current_w;
    resolution.height = pVideoInfo->current_h;
    resolution.pixelFormat = GetPixelFormatFromSDLPixelFormat(pVideoInfo->vfmt);

    capabilities.SetCurrentResolution(resolution);
  }

  void cSystem::UpdateCapabilities()
  {
    const char* szValue = nullptr;

    szValue = (const char*)glGetString(GL_VENDOR);
    assert(szValue != nullptr);
    std::cout<<"cContext::cContext Vendor     : "<<szValue<<std::endl;
    szValue = (const char*)glGetString(GL_RENDERER);
    assert(szValue != nullptr);
    std::cout<<"cContext::cContext Renderer   : "<<szValue<<std::endl;
    szValue = (const char*)glGetString(GL_VERSION);
    assert(szValue != nullptr);
    std::cout<<"cContext::cContext Version    : "<<szValue<<std::endl;
    szValue = (const char*)glGetString(GL_EXTENSIONS);
    assert(szValue != nullptr);
    std::cout<<"cContext::cContext Extensions : "<<szValue<<std::endl;

    std::ostringstream tVendor;
    tVendor<<glGetString(GL_VENDOR);

    const std::string sVendor(tVendor.str());
    bool bIsNVIDIA = (sVendor.find("NVIDIA") != std::string::npos);
    bool bIsATI = (sVendor.find("ATI") != std::string::npos) || (sVendor.find("AMD") != std::string::npos);
    if (!bIsNVIDIA && !bIsATI) {
      std::cout<<"cContext::cContext Vendor is neither ATI nor NVIDIA, vendor="<<sVendor<<std::endl;
    }

    if (FindExtension("GL_ARB_multitexture")) std::cout<<"cContext::cContext Found GL_ARB_multitexture"<<std::endl;
    else {
      std::cout<<"cContext::cContext GL_ARB_multitexture is not present"<<std::endl;
      assert(false);
    }


    GLint iValue = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &iValue);
    assert(iValue > 0);
    capabilities.iMaxTextureSize = iValue;

     // Limit the max texture size to MAX_TEXTURE_SIZE
    if (capabilities.iMaxTextureSize > MAX_TEXTURE_SIZE) capabilities.iMaxTextureSize = MAX_TEXTURE_SIZE;

    std::cout<<"cSystem::UpdateCapabilities Max texture size "<<capabilities.iMaxTextureSize<<std::endl;

    // Cube Map Support
    if (FindExtension("GL_ARB_texture_cube_map")) {
      std::cout<<"cSystem::UpdateCapabilities Found GL_ARB_texture_cube_map"<<std::endl;
      capabilities.bIsCubemappingSupported = true;
    } else {
      std::cout<<"cSystem::UpdateCapabilities Not Found GL_ARB_texture_cube_map"<<std::endl;
    }


    // GLSL Version
    float fShaderVersion = GetShaderVersion();
    {
      std::ostringstream stm;
      stm<<fShaderVersion;

      if (fShaderVersion >= 1.0f) {
        std::cout<<"cSystem::UpdateCapabilities Found Shader"<<stm.str()<<std::endl;
        capabilities.bIsShadersTwoPointZeroOrLaterSupported = true;
      } else {
        std::cout<<"cSystem::UpdateCapabilities Not Found Shader1.1, version found is Shader"<<stm.str()<<std::endl;
        capabilities.bIsShadersTwoPointZeroOrLaterSupported = false;
      }
    }

    if (capabilities.bIsShadersTwoPointZeroOrLaterSupported) std::cout<<"cSystem::UpdateCapabilities Can use shaders, shaders turned on"<<std::endl;
    else std::cout<<"cSystem::UpdateCapabilities Cannot use shaders, shaders turned off"<<std::endl;


    // Frame Buffer Object Support
    if (FindExtension("GL_EXT_framebuffer_object")) {
      std::cout<<"cSystem::UpdateCapabilities Found GL_EXT_framebuffer_object"<<std::endl;
      capabilities.bIsFrameBufferObjectSupported = true;
    } else std::cout<<"cSystem::UpdateCapabilities Not Found GL_EXT_framebuffer_object"<<std::endl;


    capabilities.bIsFSAASupported = (GL_ARB_multisample != 0);
    /*if (capabilities.bIsFSAASupported) {
      // Only sample at 2, 4, 8 or 16
      if (capabilities.nMaxFSAALevels > 16) capabilities.nMaxFSAALevels = 16;
      else if (capabilities.nMaxFSAALevels > 8) capabilities.nMaxFSAALevels = 8;
      else if (capabilities.nMaxFSAALevels > 4) capabilities.nMaxFSAALevels = 4;
      else capabilities.nMaxFSAALevels = 2;
    }*/
  }

  cWindow* cSystem::CreateWindow(const cResolution& resolution, bool bIsFullScreen)
  {
    cWindow* pWindow = new cWindow(*this, resolution, bIsFullScreen);

    return pWindow;
  }

  void cSystem::DestroyWindow(cWindow* pWindow)
  {
    assert(pWindow != nullptr);
    delete pWindow;
  }


  cContext* cSystem::CreateSharedContextFromWindow(const cWindow& window)
  {
    return new cContext(*this, window);
  }

  cContext* cSystem::CreateSharedContextFromContext(const cContext& rhs)
  {
    return new cContext(*this, rhs.GetResolution());
  }

  void cSystem::DestroyContext(cContext* pContext)
  {
    delete pContext;
    pContext = nullptr;
  }




  cContext::cContext(cSystem& _system, const cWindow& window) :
    system(_system),
    bIsValid(false),
    resolution(window.GetResolution()),
    pSurface(nullptr)
  {
    std::cout<<"cContext::cContext"<<std::endl;

    unsigned int uiFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;


    if (window.IsFullScreen()) {
      std::cout<<"cContext::cContext fullscreen"<<std::endl;
      uiFlags |= SDL_FULLSCREEN;
    } else {
      std::cout<<"cContext::cContext window"<<std::endl;
      uiFlags &= ~SDL_FULLSCREEN;
    }



    const SDL_VideoInfo* pVideoInfo = SDL_GetVideoInfo();
    if (pVideoInfo == nullptr) {
      std::cout<<"cContext::cContext SDL_GetVideoInfo FAILED error="<<SDL_GetError()<<std::endl;
      return;
    }


    // This checks to see if surfaces can be stored in memory
    if (pVideoInfo->hw_available) {
      std::cout<<"cContext::cContext Hardware surface"<<std::endl;
      uiFlags |= SDL_HWSURFACE;
      uiFlags &= ~SDL_SWSURFACE;
    } else {
      std::cout<<"cContext::cContext Software surface"<<std::endl;
      uiFlags |= SDL_SWSURFACE;
      uiFlags &= ~SDL_HWSURFACE;
    }

    // This checks if hardware blits can be done
    if (pVideoInfo->blit_hw) {
      std::cout<<"cContext::cContext Hardware blit"<<std::endl;
      uiFlags |= SDL_HWACCEL;
    } else {
      std::cout<<"cContext::cContext Software blit"<<std::endl;
      uiFlags &= ~SDL_HWACCEL;
    }

    // Sets up OpenGL double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // We definitely want the OpenGL flag for SDL_SetVideoMode
    assert(uiFlags & SDL_OPENGL);





    if (!GLeeInit()) {
      std::cout<<"cContext::cContext GLeeInit FAILED error="<<GLeeGetErrorString()<<std::endl;
    } else {
      std::cout<<"cContext::cContext GLeeInit succeeded"<<std::endl;
    }


    // Create an SDL surface
    std::cout<<"cContext::cContext Calling SDL_SetVideoMode"<<std::endl;
    pSurface = SDL_SetVideoMode(resolution.width, resolution.height, GetBitsForPixelFormat(resolution.pixelFormat), uiFlags);
    if (pSurface == nullptr) {
      std::cout<<"cContext::cContext SDL_SetVideoMode FAILED error="<<SDL_GetError()<<std::endl;
      return;
    }


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
    resolution(_resolution)
  {
    std::cout<<"cContext::cContext"<<std::endl;
  }

  cContext::~cContext()
  {
    assert(pSurface != nullptr);

    SDL_FreeSurface(pSurface);
    pSurface = nullptr;
  }

  cTexture* cContext::CreateTexture(const std::string& sFileName)
  {
    return nullptr;
  }

  cTexture* cContext::CreateTexture(size_t width, size_t height, PIXELFORMAT pixelFormat)
  {
    return nullptr;
  }

  void cContext::DestroyTexture(cTexture* pTexture)
  {
    assert(pTexture != nullptr);
    delete pTexture;
  }

  cShader* cContext::CreateShader(const std::string& sVertexShaderFileName, const std::string& sFragmentShaderFileName)
  {
    return nullptr;
  }

  void cContext::DestroyShader(cShader* pShader)
  {
    assert(pShader != nullptr);
    delete pShader;
  }

  cVertexBufferObject* cContext::CreateVertexBufferObject()
  {
    return nullptr;
  }

  void cContext::DestroyVertexBufferObject(cVertexBufferObject* pVertexBufferObject)
  {
    assert(pVertexBufferObject != nullptr);
    delete pVertexBufferObject;
  }



  // Under OpenGL 3.x we should use this method (We can probably do this under OpenGL 2.x too if we change the shaders)
  //glUniformMatrix4fv("projMat", 1, GL_FALSE, matProjection.GetOpenGLMatrixPointer());
  //glUniformMatrix4fv("???", 1, GL_FALSE, matModelView.GetOpenGLMatrixPointer());
  //glUniformMatrix4fv("???", 1, GL_FALSE, matTexture.GetOpenGLMatrixPointer());

  void cContext::SetProjectionMatrix(const spitfire::math::cMat4& matrix)
  {
    matProjection = matrix;

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(matProjection.GetOpenGLMatrixPointer());
  }

  void cContext::SetModelViewMatrix(const spitfire::math::cMat4& matrix)
  {
    matModelView = matrix;

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(matModelView.GetOpenGLMatrixPointer());
  }

  void cContext::SetTextureMatrix(const spitfire::math::cMat4& matrix)
  {
    matTexture = matrix;

    glMatrixMode(GL_TEXTURE);
    glLoadMatrixf(matTexture.GetOpenGLMatrixPointer());
  }
}
