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

// Spitfire headers
#include <spitfire/util/log.h>

// libopenglmm headers
#include <libopenglmm/libopenglmm.h>
#include <libopenglmm/cContext.h>
#include <libopenglmm/cSystem.h>
#include <libopenglmm/cWindow.h>
#include <libopenglmm/opengl.h>

namespace opengl
{
  PIXELFORMAT GetPixelFormatFromSDLPixelFormat(const SDL_PixelFormat* pPixelFormat)
  {
    assert(pPixelFormat != nullptr);

    if ((pPixelFormat->BitsPerPixel == 32) && (pPixelFormat->BytesPerPixel == 4)) return PIXELFORMAT::R8G8B8A8;
    if ((pPixelFormat->BitsPerPixel == 24) && (pPixelFormat->BytesPerPixel == 3)) return PIXELFORMAT::R8G8B8;
    if ((pPixelFormat->BitsPerPixel == 16) && (pPixelFormat->BytesPerPixel == 3)) return PIXELFORMAT::R5G6B5;

    assert(false);
    return PIXELFORMAT::R5G6B5;
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

  cSystem::cSystem() :
    nContexts(0)
  {
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0) {
      LOGERROR<<"cSystem::cSystem SDL_Init FAILED error="<<SDL_GetError()<<std::endl;
      return;
    }

    UpdateResolutions();
  }

  cSystem::~cSystem()
  {
    assert(nContexts == 0);

    SDL_Quit();
  }

  std::string cSystem::GetErrorString(GLenum error)
  {
    switch (error) {
      case GL_NO_ERROR: return "GL_NO_ERROR";
      case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
      case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
      case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
      #if BUILD_LIBOPENGLMM_OPENGL_VERSION < 300
      // NOTE: These only makes sense for OpenGL 2 and earlier because the matrix stack has been removed in OpenGL 3
      case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
      case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
      #endif
      case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
    };

    std::ostringstream o;
    o<<"Unknown error ";
    o<<error;
    return o.str();
  }

  std::string cSystem::GetErrorString()
  {
    const GLenum error = glGetError();
    const std::string sError = GetErrorString(error);
    if (error == GL_NO_ERROR) return sError;

    LOG<<"cSystem::GetErrorString Error "<<sError<<std::endl;
    assert(error == GL_NO_ERROR);
    return sError;
  }

  bool cSystem::IsGPUATI() const
  {
    const GLubyte* szValue = glGetString(GL_VENDOR);
    if (szValue == nullptr) return false;

    std::ostringstream tVendor;
    tVendor<<szValue;

    const std::string sVendor(tVendor.str());
    return (sVendor.find("ATI") != std::string::npos) || (sVendor.find("AMD") != std::string::npos);
  }

  bool cSystem::IsGPUNVIDIA() const
  {
    std::ostringstream tVendor;
    tVendor<<glGetString(GL_VENDOR);

    const std::string sVendor(tVendor.str());
    return (sVendor.find("NVIDIA") != std::string::npos);
  }

  #define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
  #define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

  size_t cSystem::GetGPUMemoryTotalMB() const
  {
    size_t nTotalMB = 0;

    if (IsGPUATI()) {

    } else if (IsGPUNVIDIA()) {
      GLint total_mem_kb = 0;
      glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb);
      nTotalMB = total_mem_kb;
    }

    return nTotalMB;
  }

  size_t cSystem::GetGPUMemoryUsedMB() const
  {
    size_t nTotalMB = GetGPUMemoryTotalMB();
    size_t nUsedMB = 0;

    if (IsGPUATI()) {
      //int cur_avail_mem_kb = 0;
      //glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, &cur_avail_mem_kb);
      //size_t nAvailableMB = cur_avail_mem_kb / 1024;
      //assert(nAvailableMB <= nTotalMB);
      //nUsedMB = nTotalMB - nAvailableMB;
    } else if (IsGPUNVIDIA()) {
      GLint cur_avail_mem_kb = 0;
      glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb);
      size_t nAvailableMB = cur_avail_mem_kb / 1024;
      assert(nAvailableMB <= nTotalMB);
      nUsedMB = nTotalMB - nAvailableMB;
    }

    return nUsedMB;
  }

  std::string cSystem::GetExtensions() const
  {
    #if BUILD_LIBOPENGLMM_OPENGL_VERSION >= 300
    LOG<<"cSystem::GetExtensions"<<std::endl;
    std::ostringstream o;
    int iNumberOfExtensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &iNumberOfExtensions);
    o<<"Extensions not added";
    //for (int i = 0; i < iNumberOfExtensions; i++) {
    //  o<<glGetStringi(GL_EXTENSIONS, i);
    //  LOG<<"cSystem::FindExtension \""<<o.str()<<"\""<<std::endl;
    //  o<<" ";
    //}
    return o.str();
    #else
    return (const char*)glGetString(GL_EXTENSIONS);
    #endif
  }

  bool cSystem::FindExtension(const std::string& sExt) const
  {
    #if BUILD_LIBOPENGLMM_OPENGL_VERSION >= 300
    LOG<<"cSystem::FindExtension"<<std::endl;

    (void)sExt;

    // There is a bug in some drivers where glGetStringi returns NULL, so we just return true for all extensions (We aren't using anything special and if we are running OpenGL 3 we probably support all the basic extensions anyway)
    return true;

    /*int iNumberOfExtensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &iNumberOfExtensions);
    for (int i = 0; i < iNumberOfExtensions; i++) {
      std::ostringstream t;
      t<<glGetStringi(GL_EXTENSIONS, i);
      ASSERT(glGetStringi(GL_EXTENSIONS, i) != nullptr);
      ASSERT(glGetStringi(GL_EXTENSIONS, i)[0] != 0);
      LOG<<"cSystem::FindExtension \""<<t.str()<<"\""<<std::endl;
      if (sExt == t.str()) return true;
    }
    return false;*/
    #else
    std::ostringstream t;
    t<<const_cast<const unsigned char*>(glGetString(GL_EXTENSIONS));

    return (t.str().find(sExt) != std::string::npos);
    #endif
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
      LOG<<"cSystem::UpdateResolutions SDL_GetVideoInfo FAILED error="<<SDL_GetError()<<std::endl;
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
    LOG<<"cSystem::UpdateCapabilities glGetError="<<cSystem::GetErrorString()<<std::endl;

    const char* szValue = nullptr;

    szValue = (const char*)glGetString(GL_VENDOR);
    assert(szValue != nullptr);
    LOG<<"cSystem::UpdateCapabilities Vendor: "<<(szValue != nullptr ? szValue : "")<<std::endl;
    szValue = (const char*)glGetString(GL_RENDERER);
    assert(szValue != nullptr);
    LOG<<"cSystem::UpdateCapabilities Renderer: "<<(szValue != nullptr ? szValue : "")<<std::endl;
    szValue = (const char*)glGetString(GL_VERSION);
    assert(szValue != nullptr);
    LOG<<"cSystem::UpdateCapabilities Version: "<<(szValue != nullptr ? szValue : "")<<std::endl;
    szValue = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    assert(szValue != nullptr);
    LOG<<"cSystem::UpdateCapabilities GLSL Version: "<<(szValue != nullptr ? szValue : "")<<std::endl;
    const std::string sValue = GetExtensions();
    LOG<<"cSystem::UpdateCapabilities Extensions: "<<sValue<<std::endl;

    if (!IsGPUNVIDIA() && !IsGPUATI()) {
      std::ostringstream tVendor;
      tVendor<<glGetString(GL_VENDOR);

      const std::string sVendor(tVendor.str());
      LOG<<"cSystem::UpdateCapabilities Vendor is neither ATI nor NVIDIA, vendor="<<sVendor<<std::endl;
    }

    if (FindExtension("GL_ARB_multitexture")) LOG<<"cSystem::UpdateCapabilities Found GL_ARB_multitexture"<<std::endl;
    else {
      LOG<<"cSystem::UpdateCapabilities GL_ARB_multitexture is not present"<<std::endl;
      assert(false);
    }


    GLint iValue = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &iValue);
    assert(iValue > 0);
    capabilities.iMaxTextureSize = iValue;

     // Limit the max texture size to MAX_TEXTURE_SIZE
    if (capabilities.iMaxTextureSize > MAX_TEXTURE_SIZE) capabilities.iMaxTextureSize = MAX_TEXTURE_SIZE;

    LOG<<"cSystem::UpdateCapabilities Max texture size "<<capabilities.iMaxTextureSize<<std::endl;

    // Cube Map Support
    if (FindExtension("GL_ARB_texture_cube_map")) {
      LOG<<"cSystem::UpdateCapabilities Found GL_ARB_texture_cube_map"<<std::endl;
      capabilities.bIsCubemappingSupported = true;
    } else {
      LOG<<"cSystem::UpdateCapabilities Not Found GL_ARB_texture_cube_map"<<std::endl;
    }


    // GLSL Version
    float fShaderVersion = GetShaderVersion();
    {
      std::ostringstream stm;
      stm<<fShaderVersion;

      if (fShaderVersion >= 1.0f) {
        LOG<<"cSystem::UpdateCapabilities Found Shader"<<stm.str()<<std::endl;
        capabilities.bIsShadersTwoPointZeroOrLaterSupported = true;
      } else {
        LOG<<"cSystem::UpdateCapabilities Not Found Shader1.1, version found is Shader"<<stm.str()<<std::endl;
        capabilities.bIsShadersTwoPointZeroOrLaterSupported = false;
      }
    }

    if (capabilities.bIsShadersTwoPointZeroOrLaterSupported) LOG<<"cSystem::UpdateCapabilities Can use shaders, shaders turned on"<<std::endl;
    else LOG<<"cSystem::UpdateCapabilities Cannot use shaders, shaders turned off"<<std::endl;


    // How many textures can we access in a vertex shader (As opposed to in the fragment shader)
    GLint iTextureUnitsInVertexShader = 0;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &iTextureUnitsInVertexShader);
    LOG<<"cSystem::UpdateCapabilities "<<iTextureUnitsInVertexShader<<" texture units accessable in vertex shader"<<std::endl;


    // Frame Buffer Object Support
    if (FindExtension("GL_EXT_framebuffer_object")) {
      LOG<<"cSystem::UpdateCapabilities Found GL_EXT_framebuffer_object"<<std::endl;
      capabilities.bIsFrameBufferObjectSupported = true;
    } else LOG<<"cSystem::UpdateCapabilities Not Found GL_EXT_framebuffer_object"<<std::endl;


    //capabilities.bIsFSAASupported = (GL_ARB_multisample != 0);
    /*if (capabilities.bIsFSAASupported) {
      // Only sample at 2, 4, 8 or 16
      if (capabilities.nMaxFSAALevels > 16) capabilities.nMaxFSAALevels = 16;
      else if (capabilities.nMaxFSAALevels > 8) capabilities.nMaxFSAALevels = 8;
      else if (capabilities.nMaxFSAALevels > 4) capabilities.nMaxFSAALevels = 4;
      else capabilities.nMaxFSAALevels = 2;
    }*/
  }

  cWindow* cSystem::CreateWindow(const opengl::string_t& sCaption, const cResolution& resolution, bool bIsFullScreen)
  {
    cWindow* pWindow = new cWindow(*this, sCaption, resolution, bIsFullScreen);

    return pWindow;
  }

  void cSystem::DestroyWindow(cWindow* pWindow)
  {
    assert(pWindow != nullptr);
    delete pWindow;
  }


  cContext* cSystem::CreateSharedContextFromWindow(const cWindow& window)
  {
    nContexts++;

    return new cContext(*this, window);
  }

  cContext* cSystem::CreateSharedContextFromContext(const cContext& rhs)
  {
    nContexts++;

    return new cContext(*this, rhs.GetResolution());
  }

  void cSystem::DestroyContext(cContext* pContext)
  {
    delete pContext;
    pContext = nullptr;

    nContexts--;
  }
}
